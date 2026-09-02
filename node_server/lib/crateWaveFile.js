export const createWaveData = (pcmData, sampleRate, channels, bitsPerSample) => {

    const header = Buffer.alloc(44);

    const byteRate = sampleRate * channels * bitsPerSample / 8;

    const blockAlign = channels * bitsPerSample / 8;

    // RIFF header
    header.write('RIFF', 0); // ChunkID
    header.writeUInt32LE(36 + pcmData.length, 4); // ChunkSize
    header.write('WAVE', 8); // Format

    // fmt subchunk
    header.write('fmt ', 12); // Subchunk1ID
    header.writeUInt32LE(16, 16); // Subchunk1Size
    header.writeUInt16LE(1, 20); // AudioFormat (PCM)

    // Channels
    header.writeUInt16LE(channels, 22); // NumChannels
    header.writeUInt32LE(sampleRate, 24); // SampleRate
    header.writeUInt32LE(byteRate, 28); // ByteRate
    header.writeUInt16LE(blockAlign, 32); // BlockAlign
    header.writeUInt16LE(bitsPerSample, 34); // BitsPerSample

    // data subchunk
    header.write('data', 36); // Subchunk2ID
    header.writeUInt32LE(pcmData.length, 40); // Subchunk2Size

    return Buffer.concat([header, pcmData]);
}