import dotenv from "dotenv";
import path from "path";
import {File} from "node:buffer";

dotenv.config({ path: path.resolve(process.cwd(), '.env') });

import {createWaveData} from "../lib/crateWaveFile.js";
import {generateAIResponse} from "../services/generate_ai_response.js";
import {ttsToBinary} from "../lib/tts_to_binary.js";

// =====================================================
// Send audio to OpenAI
// =====================================================

export const sendToOpenAI = async (pcmData) => {
    
    let userInput = "";

    try{

        // Convert PCM data to WAV format before sending to OpenAI

        const waveData = createWaveData(pcmData, 16000, 1, 16); // Assuming 16-bit PCM data

        const form = new FormData();

        const audioFile = new File([waveData], "audio.wav", { type: "audio/wav" });

        form.append("file", audioFile);
        form.append("model", "gpt-4o-mini-transcribe");

        const response = await fetch("https://api.openai.com/v1/audio/transcriptions", {
            method: "POST",
            headers: {
                "Authorization": `Bearer ${process.env.OPENAI_API_KEY}`
            },
            body: form
        });

        const responseText = await response.text();

        if (!response.ok) {
            throw new Error(`Failed to transcribe audio: ${responseText}`);
        }

        const data = JSON.parse(responseText);

        console.log("Transcription result:", data);

        userInput = data.text || "";

        console.log("Transcribed text:", userInput);

    } catch (error) {
        console.error("Error sending PCM data to OpenAI:", error);
        return null;
    }


    //Generate AI response using the transcribed text

    if(!userInput.trim()) {
        console.log("No speach detected in the audio input.");
        return null;
    }

    const aiResponse = await generateAIResponse(userInput);

    if(!aiResponse) {
        console.error("Failed to generate AI response.");
        return null;
    }

    //Convert AI response to Binary
   
    const binaryAudio = await ttsToBinary(aiResponse);

    if(!binaryAudio) {
        console.error("Failed to convert AI response to binary audio.");
        return null;
    }

    return { aiResponse, binaryAudio };
}

//===================================================
// Send to ESP32
//===================================================

export const sendToESP32 = (socket, binaryAudio) => {

    if(!socket || !binaryAudio) {
        console.error("Socket or binary audio data is missing.");
        return;
    }

    const CHUNK_SIZE = 2048; // Define the chunk size for sending data

    console.log("Sending tts audio:", binaryAudio.length, "bytes");

    for (let offset = 0; offset < binaryAudio.length; offset += CHUNK_SIZE) {

        const chunk = binaryAudio.subarray(offset, Math.min(offset + CHUNK_SIZE, binaryAudio.length));

        socket.send(chunk, { binary: true });
    }

    console.log("Finished sending tts audio to ESP32");
}