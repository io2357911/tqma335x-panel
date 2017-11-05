#ifndef TIMER_H
#define TIMER_H

#include <QDateTime>

class Timer {
public:
    Timer();

    void start();
    double time() const;

private:
    QDateTime _startTime;
};

#endif // TIMER_H
