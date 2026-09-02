import dotenv from 'dotenv';
import path from "path"
import express from 'express';
import { WebSocketServer } from 'ws';
import http from 'http';

dotenv.config({path: path.resolve(process.cwd(), '.env')});

// import { sendToOpenAI, sendToESP32 } from './api/open_ai.js';

import { connectToOpenAIRealtime, sendAudioToRealtime, finishRealTimeAudio, setESP32Socket } from './api/realtime.js';


const app = express();
const server = http.createServer(app);
const wss = new WebSocketServer({ server, path: '/ws' });

//===============================
// Connect node -> OpenAI Realtime
//===============================

connectToOpenAIRealtime();

wss.on('connection', (socket) => {

  console.log('ESP32 WebSocket connection established');
  
  setESP32Socket(socket);

  socket.send(JSON.stringify({ message: 'Hello from Node_Server' }));

  // ==========================================
  // ESP32 -> NODE
  // ==========================================

  socket.on('message', async (data, isBinary) => {

    console.log(`Received message: ${data.length}`);

    // ==========================================
    // BINARY -> AUDIO 
    // ==========================================

    if (isBinary) {

      console.log("Received binary audio chunk:", data.length);

      sendAudioToRealtime(Buffer.from(data));

      return;
    }

    // ==========================================
    // TEXT = CONTROL MESSAGE
    // ==========================================

    const message = data.toString();

    console.log("TEXT MESSAGE:", message);

    // ==========================================
    // RECORDING FINISHED
    // ==========================================

    if (message === "RECORDING_COMPLETED") {

      console.log("Recording completed.");

      finishRealTimeAudio();
    }
  });

  // ==========================================
  // ESP32 DISCONNECTED
  // ==========================================

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
