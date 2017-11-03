#ifndef TAG_H
#define TAG_H

#include <QVector>

class Tag
{
public:
    Tag();
};

class Tags {
public:

    Tag *find(QString name);

private:
    QVector<Tag*> _tags;
};

#endif // TAG_H
