#include "utils.h"
#include <assert.h>
#include <stdlib.h>
#include <chrono>
#include <thread>

void Utils::sleepMs(int timeMs) {
    std::this_thread::sleep_for(std::chrono::milliseconds(timeMs));
}

double Utils::randf() {
    return rand() / (RAND_MAX + 1.);
}

double Utils::randf(double a, double b) {
    assert(b >= a);
    return a + randf()*(b-a);
}
