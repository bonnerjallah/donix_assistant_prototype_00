#include "audioqueue.h"

PcmAudioQueue::PcmAudioQueue(size_t size) : capacity(size) {
    buffer.resize(capacity);
}


// ============================================================
// PUSH
// ============================================================

bool PcmAudioQueue::push(const int16_t* data, size_t count) {

    if (data == nullptr || count == 0) {
        return false;
    }

    if (count >= capacity) {
        return false;
    }

    size_t write = write_ptr.load(std::memory_order_relaxed);

    size_t read = read_ptr.load(std::memory_order_acquire);

    size_t used;

    if (write >= read) {
        used = write - read;
    } else {
        used = capacity - (read - write);
    }

    size_t freeSpace = capacity - used - 1;

    if (freeSpace < count) {
        return false;
    }

    for (size_t i = 0; i < count; i++) {

        buffer[(write + i) % capacity] = data[i];
    }

    write_ptr.store(
        (write + count) % capacity,
        std::memory_order_release
    );

    return true;
}


// ============================================================
// POP
// ============================================================

bool PcmAudioQueue::pop( int16_t* output, size_t count) {

    if (output == nullptr || count == 0) {
        return false;
    }

    size_t write = write_ptr.load(std::memory_order_acquire);

    size_t read = read_ptr.load(std::memory_order_relaxed);

    size_t availableSamples;

    if (write >= read) {

        availableSamples = write - read;

    } else {

        availableSamples = capacity - (read - write);
    }

    if (availableSamples < count) {
        return false;
    }

    for (size_t i = 0; i < count; i++) {

        output[i] = buffer[(read + i) % capacity];
    }

    read_ptr.store(
        (read + count) % capacity,
        std::memory_order_release
    );

    return true;
}


// ============================================================
// AVAILABLE
// ============================================================

size_t PcmAudioQueue::available() {
    size_t write = write_ptr.load(std::memory_order_acquire);

    size_t read = read_ptr.load(std::memory_order_acquire);

    if (write >= read) {
        return write - read;
    }

    return capacity - (read - write);
}


// ============================================================
// FREE SPACE
// ============================================================

size_t PcmAudioQueue::freeSpace() {
    return capacity - available() - 1;
}


// ============================================================
// CLEAR
// ============================================================

void PcmAudioQueue::clear() {
    read_ptr.store(0, std::memory_order_release);
    write_ptr.store(0, std::memory_order_release);
}