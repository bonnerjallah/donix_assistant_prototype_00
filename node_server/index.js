import dotenv from 'dotenv';
import path from "path"
import express from 'express';
import { WebSocketServer } from 'ws';
import http from 'http';

dotenv.config({path: path.resolve(process.cwd(), '.env')});


const app = express();
const server = http.createServer(app);
const wss = new WebSocketServer({ server, path: '/ws' });

wss.on('connection', (socket) => {
  console.log('ESP32 WebSocket connection established');

  socket.send(JSON.stringify({ message: 'Hello from Node_Server' }));

  let audioChunck = [];

  socket.on('message', async (data, isBinary) => {
    console.log(`Received message: ${data.length}`);

    if (isBinary) {

      console.log("Received binary audio chunk");

      // const message = data.toString();

      // console.log("TEXT:", message);

      // if(message === "RECORDING_COMPLETE") {
      //   const pcmData = Buffer.concat(audioChunck);

      //   console.log("PCM Data Length:", pcmData.length);

      //   audioChunck = []; // Clear the audio chunk array after processing

      //   const result = sendToOpenAI(pcmData);

      //   if(!result) {
      //     console.error("Failed to process PCM data with OpenAI"); 
      //     return;
      //   }

      //   console.log("AI RESPONSE:", result.aiResponse)

      //   sendToESP32(socket, result.aiResponse);

      //   console.log("Sent AI response to ESP32")
      // }
      // return;
    }
    console.log('BINARY AUDIO:', data.length);
    // audioChunck.push(Buffer.from(data));
  });

  socket.on('close', (code, reason) => {
    console.log('================================');
    console.log('WebSocket connection closed');
    console.log('Close code:', code);
    console.log('Close reason:', reason.toString());
    console.log('================================');
  });

  socket.on("error", (error) => {
    console.error(`WebSocket error: ${error}`);
  });

});




server.listen(process.env.PORT, () => {
  console.log(`Server is listening on port ${process.env.PORT}`);
});
