#ifndef CANVAS_H
#define CANVAS_H

#include <vector>
#include <tuple>

#include <QWidget>
#include <QPainter>

using namespace std;

class Canvas : public QWidget
{
    Q_OBJECT
public:
    explicit Canvas(QWidget *parent = nullptr);
    vector<tuple<int, int>> log;
protected:
    void paintEvent(QPaintEvent *) override;
signals:

};

#endif // CANVAS_H
