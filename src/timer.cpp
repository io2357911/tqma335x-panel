#include "timer.h"

Timer::Timer() {
}

void Timer::start() {
    _startTime = QDateTime::currentDateTime();
}

double Timer::time() const {
    QDateTime currentTime = QDateTime::currentDateTime();

    qint64 seconds = _startTime.secsTo(currentTime);
    qint64 mseconds = _startTime.msecsTo(currentTime);;

    double time = 0;
    time += seconds;
    time += (double)(mseconds - seconds*1000)/1000.0;
    return time;
}
