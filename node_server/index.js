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

  socket.on('message', (message) => {
    console.log(`Received message: ${message}`);

  });

  socket.on('close', () => {
    console.log('WebSocket connection closed');
  });

  socket.on("error", (error) => {
    console.error(`WebSocket error: ${error}`);
  });

});



server.listen(process.env.PORT, () => {
  console.log(`Server is listening on port ${process.env.PORT}`);
});
