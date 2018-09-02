#ifndef PLOT_H
#define PLOT_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QColor>
#include <QTimer>
#include "qcustomplot.h"
#include "tag.h"
#include "timer.h"

/**
 * @brief The Graph class класс для отображения графика
 * по x - вреия
 * по y - значение в реальных единицах (не АЦП)
 *
 * В классе выполняется перевод значения тега из единицы
 * АЦП в реальные единицы (ток, напряжение, etc)
 */
class Graph {

public:
    QString name() const;
    void setName(const QString &name);

    QString tagName() const;
    void setNameTag(const QString &tagName);

    /**
     * @brief adc1 значение АЦП в первой точке
     * @return значение АЦП в первой точке
     */
    int adc1() const;
    void setAdc1(int adc1);

    /**
     * @brief adc2 значение АЦП во второй точке
     * @return значение АЦП во второй точке
     */
    int adc2() const;
    void setAdc2(int adc2);

    /**
     * @brief real1 реальное значение в первой точке
     * @return реальное значение в первой точке
     */
    double real1() const;
    void setReal1(double real1);

    /**
     * @brief real2 реальное значение во второй точке
     * @return реальное значение во второй точке
     */
    double real2() const;
    void setReal2(double real2);

    QColor color() const;
    void setColor(const QColor &color);

    Tag *tag() const;
    void setTag(Tag *tag);

    /**
     * @brief update рассчитывает значение реальное значение тега по двум точкам
     *
     * y = k * x + b, где x - АЦП, y - реальное значение
     */
    void update();
    void reset();
    QVector<double> values() const;

private:
    // y = k * x + b, где x - АЦП, y - реальное значение
    double K(int x0, int y0, double x1, double y1);
    double B(int x0, int y0, double x1, double y1);
    void updateCoeffs();

private:
    QString         _name;
    QString         _tagName;
    int             _adc1   = 0;
    int             _adc2   = 0;
    double          _real1  = 0; ///<
    double          _real2  = 0; ///<
    QColor          _color  = QColor(0, 0, 255);

    Tag             *_tag   = 0;
    QVector<double> _values;
    double          _K      = 0,
                    _B      = 0;
};
typedef QVector<Graph*> Graphs;

/**
 * @brief The Plot class класс для управления отображением графиков
 */
class Plot : public QObject {
    Q_OBJECT

public:
    Plot(QObject *parent = 0);
    ~Plot();

    void start();
    void stop();
    void reset();

    int refreshMs() const;
    void setRefreshMs(int refreshMs);

    Graphs graphs() const;
    void setGraphs(const Graphs &graphs);

    void setTags(const Tags &tags);

    QCustomPlot *plot() const;
    void setPlot(QCustomPlot *plot);

private:
    QCustomPlot     *_plot          = 0;
    Graphs          _graphs;
    QVector<double> _time;
    QTimer          _refreshTimer;
    Timer           _timer;
};


#endif // PLOT_H
