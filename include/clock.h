#ifndef CLOCK_H
#define CLOCK_H

#include <chrono>

class Clock {
    public:
        Clock();

        void update();
        double delta();
        double sinceStart();
    
    private:
        std::chrono::time_point<std::chrono::system_clock> start;
        std::chrono::time_point<std::chrono::system_clock> previous;
        std::chrono::time_point<std::chrono::system_clock> latest;
};

#endif // CLOCK_H
