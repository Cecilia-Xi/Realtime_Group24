#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    e1 = new Executive;
    ui->setupUi(this);
}

MainWindow::~MainWindow() {

    delete ui;

}

void MainWindow::on_pushButton_clicked() {
    qDebug()<<"click start and main-thread id"<<QThread::currentThreadId();
    detect_thread = new QThread;
    e1->moveToThread(detect_thread);
    connect(this,&MainWindow::detect_signalStart,e1,&Executive::search_withQT);
    connect(detect_thread,&QThread::finished,e1,&QObject::deleteLater);

    detect_thread->start();
    emit detect_signalStart();
}

void MainWindow::on_pushButton_2_clicked() {
    qDebug()<<"click start and main-thread id"<<QThread::currentThreadId();
    add_thread = new QThread;
    e1->moveToThread(add_thread);
    connect(this,&MainWindow::add_signalStart,e1,&Executive::add_withQT);
    connect(add_thread,&QThread::finished,e1,&QObject::deleteLater);
    emit add_signalStart();
    add_thread->start();

}

void MainWindow::on_pushButton_3_clicked() {

    qDebug()<<"click stop and the detect stop";
    detect_thread->quit();
    detect_thread->wait();
    qDebug()<<"release detect_thread";
    delete detect_thread;
    detect_thread = NULL;
}

void MainWindow::on_pushButton_4_clicked() {
    qDebug()<<"click stop and the add_thread stop";
    add_thread->quit();
    add_thread->wait();
    qDebug()<<"release add_thread";
    delete add_thread;
    add_thread = NULL;
}


