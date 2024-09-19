#ifndef TIMESLICE_H
#define TIMESLICE_H

#include <QColor>
#include <QGraphicsItem>
#include <QPainter>

class TimeSlice : public QGraphicsItem
{
public:
    TimeSlice(const QColor &color, int x, int y, int w, int h, int pid);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override;
    QRectF boundingRect() const override;

protected:


private:
    int x;
    int y;
    int w;
    int h;
    QColor color;
    int pid;
};

#endif // TIMESLICE_H
