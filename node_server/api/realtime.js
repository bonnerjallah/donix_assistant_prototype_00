import dotenv from "dotenv";
import path from "path";
import WebSocket from "ws";


dotenv.config({ path: path.resolve(process.cwd(), '.env') });

import {buildAIPromptRules} from "../lib/ai_rules.js";

let openAIRealtime = null;
let esp32Socket = null;

export const setESP32Socket = (socket) => {
    esp32Socket = socket;
}

export const connectToOpenAIRealtime = () => {
    openAIRealtime = new WebSocket("wss://api.openai.com/v1/realtime?model=gpt-realtime-2.1", {
        headers: {
            "Authorization": `Bearer ${process.env.OPENAI_API_KEY}`
        }
    });

    //===============================
    // OpenAI Realtime Event Handlers
    //===============================

    openAIRealtime.on("open", () => {
        console.log("Connected to OpenAI Realtime API");

        openAIRealtime.send(JSON.stringify({
            type: "session.update",
            session: {
                type: "realtime",
                instructions: buildAIPromptRules(),
                audio: {
                    input:{
                        format: {
                            type: "audio/pcm",
                            rate: 24000,
                        }
                    },
                    output: {
                        format: {
                            type: "audio/pcm",
                            rate: 24000,
                        },
                        voice: "alloy"
                    }
                }
            }
        }))
    });

    //===============================
    // OpenAI -> Node -> ESP32
    //===============================

    openAIRealtime.on("message", (message) => {

        const event = JSON.parse(message.toString());

        // OPENAI AUDIO -> ESP32
        if(event.type === "response.output_audio.delta" && event.delta) {

            const audioBuffer = Buffer.from(event.delta, "base64");

            const CHUNCK_SIZE = 1024; // Example chunk size in bytes

            if(esp32Socket && esp32Socket.readyState === WebSocket.OPEN) {

                for (let i = 0; i < audioBuffer.length; i += CHUNCK_SIZE) {
                    const chunk = audioBuffer.subarray(i, Math.min(i + CHUNCK_SIZE, audioBuffer.length));
                    esp32Socket.send(chunk);
                }
                console.log(`Finished sending all audio chunks to ESP32:`, audioBuffer.length, "bytes");

            } else {
                console.error("ESP32 WebSocket is not open. Cannot send audio.");
            }
        } 
    });

    //===============================
    // Close 
    //===============================

    openAIRealtime.on("close", (code, reason) => {

        console.log("Disconnected from OpenAI Realtime API");
        console.log("Close code:", code);
        console.log("Close reason:", reason.toString());
    });

    //===============================
    // Error
    //===============================

    openAIRealtime.on("error", (error) => {
        console.error("Error in OpenAI Realtime connection:", error);
    });

    return openAIRealtime;
}

// ==========================================
// ESP32 PCM → OPENAI REALTIME
// ==========================================

export const sendAudioToRealtime = (audioChunk) => {
    if (openAIRealtime && openAIRealtime.readyState === WebSocket.OPEN) {
        const base64Audio = audioChunk.toString('base64');
        openAIRealtime.send(JSON.stringify({ type: "input_audio_buffer.append", audio: base64Audio }));
    } else {
        console.error("OpenAI Realtime connection is not open. Cannot send audio chunk.");
    }
}

// ==========================================
// FINISH USER AUDIO
// ==========================================

export const finishRealTimeAudio = () => {
    if (openAIRealtime && openAIRealtime.readyState === WebSocket.OPEN) {
        openAIRealtime.send(JSON.stringify({ type: "input_audio_buffer.commit" }));
        openAIRealtime.send(JSON.stringify({ type: "response.create"}));
        console.log("Finished sending user audio to OpenAI Realtime.");
    } else {
        console.error("OpenAI Realtime connection is not open. Cannot finish audio input.");
    }
}