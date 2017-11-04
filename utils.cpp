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

int Utils::getInt(uint8_t *data, uint dataSize, uint bitOffset, uint bitSize) {
    int64_t value = 0;
    if (bitSize == 0 || bitSize >= 32) return value;

    uint byteOffset = bitOffset/8;
    uint bytesAffected = bitSize/8 + 1;
    if (byteOffset + bytesAffected > dataSize) return value;

    memcpy((char*)&value, data + byteOffset, bytesAffected);

    int alignOffset = bitOffset - byteOffset*8;
    value = value >> alignOffset;
    value &= 0xFFFFFFFF >> (32-bitSize);

    return value;
}
