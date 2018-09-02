#ifndef UTILS_H
#define UTILS_H

#include <QString>

/**
 * @brief содержит вспомогательные функции
 */
namespace Utils {

void sleepMs(int timeMs);

double randf();
double randf(double a, double b);

QString readTextFile(QString fileName, QString codec = "UTF-8");
void writeTextFile(QString fileName, QString text,  QString codec = "UTF-8");

int getInt(uint8_t *data, uint dataSize, uint bitOffset, uint bitSize);

}

#endif // UTILS_H
