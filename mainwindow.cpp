#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "timeslice.h"
#include "simulator.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->sliderNumProcesos->setRange(1, 10);
    ui->sliderNumProcesos->setValue(5);
    current_processes = {};

    connect(ui->btnSimular, &QPushButton::released, this, &MainWindow::handle_simular);
    connect(ui->sliderNumProcesos, &QSlider::valueChanged, this, &MainWindow::handle_slider);
    connect(ui->AlgorithmSelect, &QComboBox::currentIndexChanged, this, &MainWindow::handle_algorithm_select);

    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->addText("Para empezar dale click a Nueva Simulación.");

    QGraphicsView *view = new QGraphicsView(scene);
    view->setScene(scene);
    ui->tabWidget->clear();
    ui->tabWidget->addTab(view, "Simulación");

    ui->tabWidget->addTab(new QWidget(), "Procesos");
    ui->tabWidget->addTab(new QWidget(), "Log");
    ui->tabWidget->setTabEnabled(1, false);
    ui->tabWidget->setTabEnabled(2, false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handle_slider(){
    ui->numProcesos->setText(QString::number(ui->sliderNumProcesos->value()));
}

void MainWindow::handle_algorithm_select() {
    if (current_processes.empty()) return;

    int current_tab = ui->tabWidget->currentIndex();

    CPU cpu;
    Statistics stats;

    switch(ui->AlgorithmSelect->currentIndex()){
        case 0:
            cpu.first_come_first_serve(stats, current_processes);
            break;
        case 1:
            cpu.shortest_job_first(stats, current_processes);
            break;
        case 2:
            cpu.random_selection(stats, current_processes);
            break;
        case 3:
            cpu.non_preemptive_priority(stats, current_processes);
            break;
        case 4:
            cpu.round_robin(stats, current_processes);
            break;
        case 5:
            cpu.shortest_remaining_time_first(stats, current_processes);
            break;
        case 6:
            cpu.preemptive_priority(stats, current_processes);
            break;
        default:
            cout << "HOLY COW YOU FOUND A NEW ALGORITHM\n";
            break;
    }

    QGraphicsScene *scene = new QGraphicsScene(this);

    double canvas_h = ui->tabWidget->height();
    double canvas_w = ui->tabWidget->width();

    double timeline_h = canvas_h * 0.4;
    double timeline_w = canvas_w * 0.9;
    double last_x = (canvas_w - timeline_w) / 2;

    for(int i = 0; i < stats.log.size(); i++){
        QColor color = (get<0>(stats.log[i]) != -1) ? QColor(60, 91, 111) : QColor(21, 52, 72);
        double process_w = get<1>(stats.log[i]) * timeline_w / stats.total_time;
        scene->addItem(new TimeSlice(color, last_x, (canvas_h - timeline_h) / 2, (process_w <= 2) ?  2 : process_w, timeline_h, get<0>(stats.log[i])));
        last_x += process_w;
    }

    QGraphicsView *view = new QGraphicsView(scene);
    view->setScene(scene);
    ui->tabWidget->clear();
    ui->tabWidget->insertTab(0, view, "Simulación");


    QTextEdit *processes_info = new QTextEdit(ui->tabWidget);
    processes_info->setCurrentFont(QFont("Monospace"));

    for(int i = 0; i < current_processes.size(); i++){
        processes_info->append(current_processes[i].get_info());
    }

    ui->tabWidget->insertTab(1, processes_info, "Procesos");

    QTextEdit *log = new QTextEdit(ui->tabWidget);
    log->setCurrentFont(QFont("Monospace"));
    int last_timestamp = 0;

    for(int i = 0; i < stats.log.size(); i++){
        if (get<0>(stats.log[i]) == -1) {
            log->append("De " + QString::number(last_timestamp) + "ms a " + QString::number(last_timestamp + get<1>(stats.log[i])) + "ms el CPU estuvo inactivo por [" + QString::number(get<1>(stats.log[i])) + "ms]");
        } else {
            log->append("De " + QString::number(last_timestamp) + "ms a " + QString::number(last_timestamp + get<1>(stats.log[i])) + "ms el proceso con ID: " + QString::number(get<0>(stats.log[i])) + " corrió por [" + QString::number(get<1>(stats.log[i])) + "ms]");
        }
        last_timestamp += get<1>(stats.log[i]);
    }

    ui->tabWidget->insertTab(2, log, "Log");
    ui->tabWidget->setCurrentIndex(current_tab);

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

void MainWindow::handle_simular(){
    current_processes = create_random_processes(ui->sliderNumProcesos->value());
    handle_algorithm_select();
}
