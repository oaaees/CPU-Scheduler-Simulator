#include "canvas.h"

Canvas::Canvas(QWidget *parent)
    : QWidget{parent}
{
    log = {};
}

void Canvas::paintEvent(QPaintEvent *)
{
    if (log.empty()) return;

    QPainter painter(this);

    double canvas_h = this->height();
    double canvas_w = this->width();

    double timeline_h = canvas_h * 0.4;
    double timeline_w = canvas_w * 0.9;

    painter.fillRect((canvas_w - timeline_w) / 2, (canvas_h - timeline_h) / 2, timeline_w, timeline_h, QColor(125, 188, 227));
    double last = (canvas_w - timeline_w) / 2;

    int total_time = 0;

    for(int i = 0; i < log.size(); i++) total_time += get<1>(log[i]);

    for(int i = 0; i < log.size(); i++){
        double process_w = get<1>(log[i]) * timeline_w / total_time;
        if ( get<0>(log[i]) != -1 ){
            painter.fillRect(last, (canvas_h - timeline_h) / 2, process_w, timeline_h, QColor(227, 200, 125));
            // painter.setPen(QColor(0,0,0));
            // painter.drawText(last, canvas_h / 2, "P" + QString::number(get<0>(log[i])));
        }
        last += process_w;
    }

    last = (canvas_w - timeline_w) / 2;

    for(int i = 0; i < log.size(); i++){
        double process_w = get<1>(log[i]) * timeline_w / total_time;
        if ( get<0>(log[i]) == -1 ){
            painter.fillRect(last, (canvas_h - timeline_h) / 2, 2, timeline_h, QColor(125, 188, 227));
        }
        last += process_w;
    }

}
