#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "timeslice.h"
#include "simulator.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->sliderNumProcesos->setRange(1, 20);
    ui->sliderNumProcesos->setValue(5);

    connect(ui->btnSimular, &QPushButton::released, this, &MainWindow::handle_simular);
    connect(ui->sliderNumProcesos, &QSlider::valueChanged, this, &MainWindow::handle_slider);

    ui->view->addScrollBarWidget(ui->horizontalScrollBar, Qt::AlignCenter);

    QGraphicsScene *graphic = new QGraphicsScene(this);
    graphic->addText("Para empezar dale click a simular.");
    ui->view->setScene(graphic);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handle_slider(){
    ui->numProcesos->setText(QString::number(ui->sliderNumProcesos->value()));
}

void MainWindow::handle_simular() {
    CPU cpu;
    Statistics stats;
    vector<Process> processes = create_random_processes(ui->sliderNumProcesos->value());

    switch(ui->AlgorithmSelect->currentIndex()){
        case 0:
            cpu.first_come_first_serve(stats, processes);
            break;
        case 1:
            cpu.shortest_job_first(stats, processes);
            break;
        case 2:
            cpu.random_selection(stats, processes);
            break;
        case 3:
            cpu.non_preemptive_priority(stats, processes);
            break;
        case 4:
            cpu.round_robin(stats, processes);
            break;
        case 5:
            cpu.shortest_remaining_time_first(stats, processes);
            break;
        case 6:
            cpu.preemptive_priority(stats, processes);
            break;
        default:
            cout << "HOLY COW YOU FOUND A NEW ALGORITHM\n";
            break;
    }

    QGraphicsScene *graphic = new QGraphicsScene(this);

    double canvas_h = ui->view->height();
    double canvas_w = ui->view->width();

    double timeline_h = canvas_h * 0.4;
    double timeline_w = canvas_w * 0.9;

    QPen pen(QColor(255,255,255));
    //graphic->addRect((canvas_w - timeline_w) / 2, (canvas_h - timeline_h) / 2, timeline_w, timeline_h, pen);
    double last_x = (canvas_w - timeline_w) / 2;

    for(int i = 0; i < stats.log.size(); i++){
        QColor color = (get<0>(stats.log[i]) != -1) ? QColor(0,0,0) : QColor(255, 0, 0);
        double process_w = get<1>(stats.log[i]) * timeline_w / stats.total_time;
        graphic->addItem(new TimeSlice(color, last_x, (canvas_h - timeline_h) / 2, (process_w <= 2) ?  2 : process_w, timeline_h, get<0>(stats.log[i])));
        last_x += process_w;
    }

    ui->view->setScene(graphic);

    ui->txtEsperaMin->setText("Mínimo:  " + QString::number(stats.min_wait_time) + "ms");
    ui->txtEsperaAvg->setText("Promedio:  " + QString::number(stats.average_wait_time) + "ms");
    ui->txtEsperaMax->setText("Máximo:  " + QString::number(stats.max_wait_time) + "ms");

    ui->txtRespuestaMin->setText("Mínimo:  " + QString::number(stats.min_turnaround_time) + "ms");
    ui->txtRespuestaAvg->setText("Promedio:  " + QString::number(stats.average_turnaround_time) + "ms");
    ui->txtRespuestaMax->setText("Máximo:  " + QString::number(stats.max_turnaround_time) + "ms");

    ui->txtTiempoTotal->setText("Tiempo Total:  " + QString::number(stats.total_time) + "ms");
    ui->txtEjecutando->setText("Ejecutando CPU: " + QString::number(stats.CPU_usage_time) + "ms [" + QString::number(stats.CPU_usage_time * 100.0 / stats.total_time) + "%]");
    ui->txtInactivo->setText("Inactivo CPU: " + QString::number(stats.CPU_idle_time) + "ms [" + QString::number(stats.CPU_idle_time * 100.0 / stats.total_time) + "%]");

    cpu.restart();
}
