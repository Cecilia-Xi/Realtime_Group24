#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QThread>
#include <QDateTime>

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
    object = new MyObject;
    object->moveToThread(thread);
    connect(this,&MainWindow::signalStart,object,&MyObject::slotStart);
    connect(thread,&QThread::finished,object,&QObject::deleteLater);
    thread->start();

    emit signalStart();
}

void MainWindow::on_pushButton_2_clicked()
{
    qDebug()<<"click stop and the thread stop"<<QDateTime::currentDateTime();
    thread->quit();
    thread->wait();
    qDebug()<<"release thread";
    delete thread;
    thread = NULL;
}
