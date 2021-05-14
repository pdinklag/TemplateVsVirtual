#pragma once

#include <chrono>

inline uint64_t time() {
    using namespace std::chrono;
    
    return uint64_t(duration_cast<milliseconds>(
        high_resolution_clock::now().time_since_epoch()).count());
}
