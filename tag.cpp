#include "tag.h"
#include "ini.h"
#include <algorithm>

Tag::Tag() {

}

QString Tag::name() const
{
    return _name;
}

void Tag::setName(const QString &name)
{
    _name = name;
}

QString Tag::native() const
{
    return _native;
}

void Tag::setNative(const QString &native)
{
    _native = native;
}

int Tag::offset() const
{
    return _offset;
}

void Tag::setOffset(int offset)
{
    _offset = offset;
}

int Tag::segment() const
{
    return _segment;
}

void Tag::setSegment(int segment)
{
    _segment = segment;
}

int Tag::size() const
{
    return _size;
}

void Tag::setSize(int size)
{
    _size = size;
}

int Tag::step() const
{
    return _step;
}

void Tag::setStep(int step)
{
    _step = step;
}

QString Tag::device() const
{
    return _device;
}

void Tag::setDevice(const QString &device)
{
    _device = device;
}

QString Tag::type() const
{
    return _type;
}

void Tag::setType(const QString &type)
{
    _type = type;
}

int Tag::value() const
{
    return _value;
}

void Tag::setValue(int value)
{
    _value = value;
}

#include <QTextCodec>

Tags Tags::load(QString fileName) {
    Tags tags;

    QStringList keys;
    INI::Settings settings;

    keys = QStringList({ "HardID" });
    settings = INI::restore(keys, fileName, "Meta");
    tags.setHardId(settings.value("HardID", 0).toInt());

    keys = QStringList({ "SegS", "SegI", "SegQ" });
    settings = INI::restore(keys, fileName, "Segments");
    tags.setSegmentSizeS(settings.value("SegS", 0).toInt());
    tags.setSegmentSizeI(settings.value("SegI", 0).toInt());
    tags.setSegmentSizeQ(settings.value("SegQ", 0).toInt());

    QStringList groups = INI::groups(fileName, "Tag");

    keys = QStringList({ "Name", "Native", "Offset", "Segment", "Size", "Step", "Device", "Type" });
    for (QString group : groups) {
        settings = INI::restore(keys, fileName, group);

        Tag *tag = new Tag();

        tag->setName(settings["Name"].toString());
        tag->setNative(settings["Native"].toString());
        tag->setOffset(settings.value("Offset", 0).toInt());
        tag->setSegment(settings.value("Segment", 0).toInt());
        tag->setSize(settings.value("Size", 0).toInt());
        tag->setStep(settings.value("Step", 0).toInt());
        tag->setDevice(settings["Device"].toString());
        tag->setType(settings["Type"].toString());

        tags.append(tag);
    }

    std::sort(tags.begin(), tags.end(),
          [](const Tag* a, const Tag* b) -> bool { return a->offset() < b->offset(); }
    );

    return tags;
}

Tags::Tags() {}

Tag *Tags::find(QString name) const {
    for (Tag *tag : *this) {
        if (tag->name() == name) {
            return tag;
        }
    }
    return 0;
}

int Tags::segmentSizeS() const
{
    return _segmentSizeS;
}

int Tags::segmentSizeI() const
{
    return _segmentSizeI;
}

int Tags::segmentSizeQ() const
{
    return _segmentSizeQ;
}

int Tags::hardId() const
{
    return _hardId;
}

void Tags::setHardId(int hardId)
{
    _hardId = hardId;
}

void Tags::setSegmentSizeS(int segmentSizeS)
{
    _segmentSizeS = segmentSizeS;
}

void Tags::setSegmentSizeI(int segmentSizeI)
{
    _segmentSizeI = segmentSizeI;
}

void Tags::setSegmentSizeQ(int segmentSizeQ)
{
    _segmentSizeQ = segmentSizeQ;
}
