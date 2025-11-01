#pragma once
#include <chrono>

struct GameTime {
    double deltaSeconds = 0.0;
    double totalSeconds = 0.0;
    double timeOfDay = 12.0;   // [0,24)
    double timeScale = 144.0;  // 1 real sec = 144 game secs => ~10 min/day

    void tick() {
        using clock = std::chrono::high_resolution_clock;
        static auto prev = clock::now();
        auto now = clock::now();
        std::chrono::duration<double> d = now - prev;
        prev = now;
        deltaSeconds = d.count();
        totalSeconds += deltaSeconds;
        timeOfDay += (deltaSeconds * timeScale) / 3600.0;
        if (timeOfDay >= 24.0) timeOfDay -= 24.0;
    }
};