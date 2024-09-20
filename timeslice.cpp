#include "timeslice.h"

TimeSlice::TimeSlice(const QColor &color, int x, int y, int w, int h, int pid) {
    this->color = color;
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    this->pid = pid;

    setAcceptHoverEvents(true);
}

QRectF TimeSlice::boundingRect() const
{
    return QRectF(x, y, w, h);
}

void TimeSlice::paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget)
{
    Q_UNUSED(item);
    Q_UNUSED(widget);

    painter->fillRect(x, y, w, h, color);
    if (pid != -1) painter->drawText(x + (w / 2), y + (h / 2),QString::number(this->pid));
}
