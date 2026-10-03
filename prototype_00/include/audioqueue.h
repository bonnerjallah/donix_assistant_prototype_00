#pragma once

#include <Arduino.h>
#include <vector>
#include <atomic>

class PcmAudioQueue {

    private:

        std::vector<int16_t> buffer;
        size_t capacity;

        std::atomic<size_t> write_ptr{0};
        std::atomic<size_t> read_ptr{0};

    public:

        PcmAudioQueue(size_t size);

        bool push(
            const int16_t* data,
            size_t count
        );

        bool pop(
            int16_t* output,
            size_t count
        );

        size_t available();

        size_t freeSpace();

        void clear();
};


// Global audio queue
extern PcmAudioQueue audioQueue;