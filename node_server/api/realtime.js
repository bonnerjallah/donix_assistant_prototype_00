import dotenv from "dotenv";
import path from "path";
import WebSocket from "ws";

dotenv.config({ path: path.resolve(process.cwd(), ".env") });

import { buildAIPromptRules } from "../lib/ai_rules.js";

let openAIRealtime = null;
let esp32Socket = null;

export const setESP32Socket = (socket) => {
    esp32Socket = socket;
};

export const connectToOpenAIRealtime = () => {

    openAIRealtime = new WebSocket(
        "wss://api.openai.com/v1/realtime?model=gpt-realtime-2.1",
        {
            headers: {
                "Authorization": `Bearer ${process.env.OPENAI_API_KEY}`
            }
        }
    );

    // =====================================================
    // OpenAI Realtime Event Handlers
    // =====================================================

    openAIRealtime.on("open", () => {

        console.log("Connected to OpenAI Realtime API");

        openAIRealtime.send(
            JSON.stringify({
                type: "session.update",
                session: {
                    type: "realtime",

                    instructions: buildAIPromptRules(),

                    audio: {
                        input: {
                            format: {
                                type: "audio/pcm",
                                rate: 24000
                            }
                        },

                        output: {
                            format: {
                                type: "audio/pcm",
                                rate: 24000
                            },

                            voice: "alloy"
                        }
                    }
                }
            })
        );
    });

    // =====================================================
    // OpenAI -> Node -> ESP32
    // =====================================================

    // Temporary buffer used to assemble OpenAI deltas
    // into fixed-size chunks.
    let realtimeAudioBuffer = Buffer.alloc(0);

    // Total bytes generated for the current response.
    let totalAudioBytes = 0;

    // Node-side queue.
    //
    // OpenAI can produce audio faster than the ESP32
    // can play it. This queue absorbs those bursts.
    const esp32AudioQueue = [];

    // Prevent multiple sender loops from running
    // simultaneously.
    let sendingAudio = false;

    // Indicates that OpenAI has finished generating
    // the current response.
    let responseAudioFinished = false;

    // =====================================================
    // Audio pacing
    // =====================================================

    // 4096 bytes
    // = 2048 int16 samples
    // = 2048 / 24000 seconds
    // = approximately 85.33 ms of audio
    const SEND_CHUNK_SIZE = 4096;
    const CHUNK_INTERVAL_MS = 85;

    // =====================================================
    // Node -> ESP32 paced sender
    // =====================================================

    async function sendAudioToESP32() {

        // Another sender is already running.
        //
        // The existing sender will continue draining
        // the queue.
        if (sendingAudio) {
            return;
        }

        sendingAudio = true;

        while (esp32AudioQueue.length > 0) {

            // Make sure ESP32 is still connected.
            if (!esp32Socket || esp32Socket.readyState !== WebSocket.OPEN) {
                console.error("ESP32 WebSocket is not open.");

                break;
            }

            // Take the oldest audio chunk.
            const chunk = esp32AudioQueue.shift();

            // This is the ONLY place where generated
            // audio is sent to the ESP32.
            esp32Socket.send(chunk);

            // Wait approximately the amount of time
            // represented by this chunk of PCM audio.
            await new Promise((resolve) =>
                setTimeout(resolve, CHUNK_INTERVAL_MS)
            );
        }

        sendingAudio = false;

        // =================================================
        // Response finished AND all generated audio has
        // been sent to the ESP32.
        // =================================================

        if (responseAudioFinished && esp32AudioQueue.length === 0 && esp32Socket && esp32Socket.readyState === WebSocket.OPEN) {

            console.log("All generated audio sent to ESP32.");

            esp32Socket.send("AUDIO_PLAYBACK_DONE");

            responseAudioFinished = false;
        }
    }

    // =====================================================
    // OpenAI messages
    // =====================================================

    openAIRealtime.on("message", (message) => {

        const event = JSON.parse(message.toString());

        // =================================================
        // OPENAI AUDIO DELTA
        // =================================================

        if (event.type === "response.output_audio.delta" && event.delta) {

            const pcm = Buffer.from(event.delta, "base64");

            totalAudioBytes += pcm.length;

            // ---------------------------------------------
            // Add the new OpenAI audio to our temporary
            // assembly buffer.
            // ---------------------------------------------

            realtimeAudioBuffer = Buffer.concat([ realtimeAudioBuffer, pcm]);

            // ---------------------------------------------
            // Extract complete 4096-byte chunks.
            //
            // IMPORTANT:
            // We DO NOT send them directly to ESP32 here.
            //
            // We put them into the Node-side queue.
            // ---------------------------------------------

            while (realtimeAudioBuffer.length >= SEND_CHUNK_SIZE) {

                const chunk = realtimeAudioBuffer.subarray(0, SEND_CHUNK_SIZE);

                realtimeAudioBuffer = realtimeAudioBuffer.subarray(SEND_CHUNK_SIZE);

                // Put audio into the queue.
                esp32AudioQueue.push(chunk);
            }

            // ---------------------------------------------
            // Start the paced sender if it isn't already
            // running.
            // ---------------------------------------------

            sendAudioToESP32();
        }

        // =================================================
        // RESPONSE AUDIO FINISHED
        // =================================================

        if (event.type === "response.output_audio.done") {

            // ---------------------------------------------
            // There may be a partial chunk left over.
            // Put it into the same queue.
            // ---------------------------------------------

            if (realtimeAudioBuffer.length > 0) {

                esp32AudioQueue.push(Buffer.from(realtimeAudioBuffer));

                realtimeAudioBuffer = Buffer.alloc(0);
            }

            // ---------------------------------------------
            // OpenAI is finished generating audio.
            //
            // DO NOT send AUDIO_PLAYBACK_DONE here.
            //
            // The sender will send it only after every
            // queued audio chunk has been sent to ESP32.
            // ---------------------------------------------

            responseAudioFinished = true;

            // Make sure the sender is running.
            sendAudioToESP32();

            // Reset counter for the next response.
            totalAudioBytes = 0;
        }
    });

    // =====================================================
    // OpenAI connection closed
    // =====================================================

    openAIRealtime.on("close", (code, reason) => {

        console.log("Disconnected from OpenAI Realtime API");

        console.log("Close code:", code);

        console.log("Close reason:", reason.toString());
    });

    // =====================================================
    // OpenAI connection error
    // =====================================================

    openAIRealtime.on("error", (error) => {

        console.error("Error in OpenAI Realtime connection:", error);
    });

    return openAIRealtime;
};

// =========================================================
// ESP32 PCM -> OpenAI Realtime
// =========================================================

export const sendAudioToRealtime = (audioChunk) => {

    if (openAIRealtime && openAIRealtime.readyState === WebSocket.OPEN) {

        const base64Audio = audioChunk.toString("base64");

        openAIRealtime.send(
            JSON.stringify({
                type: "input_audio_buffer.append",
                audio: base64Audio
            })
        );

    } else {

        console.error(
            "OpenAI Realtime connection is not open. " +
            "Cannot send audio chunk."
        );
    }
};

// =========================================================
// FINISH USER AUDIO
// =========================================================

export const finishRealTimeAudio = () => {

    if ( openAIRealtime && openAIRealtime.readyState === WebSocket.OPEN) {

        openAIRealtime.send(
            JSON.stringify({type: "input_audio_buffer.commit"})
        );

        openAIRealtime.send(
            JSON.stringify({type: "response.create"})
        );

        console.log("Finished sending user audio to OpenAI Realtime.");

    } else {

        console.error("OpenAI Realtime connection is not open. " + "Cannot finish audio input.");
    }
};