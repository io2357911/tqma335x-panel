#include "utils.h"
#include <assert.h>
#include <stdlib.h>
#include <chrono>
#include <thread>
#include <QFile>
#include <QTextStream>

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

QString Utils::readTextFile(QString fileName, QString codec) {
    QFile file(fileName);
    if (file.open(QFile::ReadOnly)) {
        QTextStream stream(&file);
        stream.setCodec(codec.toStdString().c_str());
        return stream.readAll();
    }

    return QString();
}
