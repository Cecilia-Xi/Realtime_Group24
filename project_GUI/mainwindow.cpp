#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    qDebug()<<"click start and main-thread id"<<QThread::currentThreadId();
    thread = new QThread;
    e1 = new Executive;
    e1->moveToThread(thread);
    connect(this,&MainWindow::signalStart,e1,&Executive::search_withQT);
    connect(thread,&QThread::finished,e1,&QObject::deleteLater);
    thread->start();

    emit signalStart();
}

void MainWindow::on_pushButton_2_clicked()
{
    qDebug()<<"click start and main-thread id"<<QThread::currentThreadId();
    thread = new QThread;
    e1 = new Executive;
    e1->moveToThread(thread);
    connect(this,&MainWindow::signalStart,e1,&Executive::add_withQT);
    connect(thread,&QThread::finished,e1,&QObject::deleteLater);
    thread->start();

    emit signalStart();
}

