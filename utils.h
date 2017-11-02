#ifndef UTILS_H
#define UTILS_H

#include <QString>

namespace Utils {

void sleepMs(int timeMs);

double randf();
double randf(double a, double b);

QString readTextFile(QString fileName);

}

#endif // UTILS_H
