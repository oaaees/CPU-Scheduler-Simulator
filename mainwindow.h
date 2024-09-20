#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <simulator.h>
#include <algorithmsinfo.h>

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTextEdit>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    vector<Process> current_processes;

private slots:
    void handle_simular();
    void handle_slider();
    void handle_algorithm_select();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
