#include "clock.h"

Clock::Clock() :
    start(std::chrono::system_clock::now()),
    previous(start),
    latest(start)
{

}

void Clock::update() {
    previous = latest;
    latest = std::chrono::system_clock::now();
}

double Clock::delta() {
    std::chrono::duration<double> delta = latest - previous;
    return delta.count();
}

double Clock::sinceStart() {
    std::chrono::duration<double> dt = latest - start;
    return dt.count();
}
