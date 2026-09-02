import dotenv from "dotenv";
import path from "path";

dotenv.config({ path: path.resolve(process.cwd(), '.env') });


export const ttsToBinary = async (ttsData) => {

    try {

        const response = await fetch("https://api.openai.com/v1/audio/speech", {
            method: "POST",
            headers: {
                "Content-Type": "application/json",
                "Authorization": `Bearer ${process.env.OPENAI_API_KEY}`
            },
            body: JSON.stringify({
                model: "gpt-4o-mini-tts",
                voice: "alloy",
                input: ttsData,
                response_format: "pcm"
            })
        });

        if (!response.ok) {
            const errorText = await response.text();
            throw new Error(`Failed to convert TTS data to binary: ${errorText}`);
        }

       const audioArrayBuffer = await response.arrayBuffer();

       const binaryAudioData = Buffer.from(audioArrayBuffer);

       console.log("Binary audio data:", binaryAudioData, "bytes");

       return binaryAudioData;

    } catch (error) {
        console.error("Error converting TTS data to binary:", error);
        return null;
    }
}