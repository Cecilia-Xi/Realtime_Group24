#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    e1 = new Executive;
    e2 = new Executive;
    ui->setupUi(this);
}

MainWindow::~MainWindow() {


    qDebug()<<"release thread";

    ui = nullptr;
    delete ui;
    e1 = nullptr;
    e2 = nullptr;
    delete e1;
    delete e2;

    thread1 = nullptr;
    thread2 =nullptr;
    delete thread2;
    delete thread1;


}

void MainWindow::on_pushButton_clicked() {
    qDebug()<<"click start and main-thread id"<<QThread::currentThreadId();
    thread1 = new QThread;
    e1->moveToThread(thread1);
    connect(this,&MainWindow::signalStart,e1,&Executive::search_withQT);
    connect(thread1,&QThread::finished,e1,&QObject::deleteLater);
    emit signalStart();
    thread1->start();

}

void MainWindow::on_pushButton_2_clicked() {
    qDebug()<<"click start and main-thread id"<<QThread::currentThreadId();
    thread2 = new QThread;
    e1->moveToThread(thread2);
    connect(this,&MainWindow::signalStart2,e1,&Executive::add_withQT);
    connect(thread2,&QThread::finished,e1,&QObject::deleteLater);
    emit signalStart2();
    thread2->start();

}

void MainWindow::on_pushButton_3_clicked() {
    qDebug()<<"3"<<QThread::currentThreadId();
    thread1->quit();
//    qDebug()<<"release thread1";
//    thread2 = new QThread;
//    thread1->moveToThread(thread2);
//    //e1->moveToThread(thread2);
//    connect(this,&MainWindow::signalStart2,e1,&Executive::add_withQT);
//    connect(thread2,&QThread::finished,e1,&QObject::deleteLater);

//    emit signalStart2();
//    thread2->start();

}

void MainWindow::on_pushButton_4_clicked() {
    qDebug()<<"4"<<QThread::currentThreadId();
    thread2->quit();
//    qDebug()<<"release thread1";
//    thread1 = new QThread;
//    //e1->moveToThread(thread2);
//    thread2->moveToThread(thread1);
//    connect(this,&MainWindow::signalStart,e1,&Executive::add_withQT);
//    connect(thread1,&QThread::finished,e1,&QObject::deleteLater);

//    emit signalStart();
//    thread2->start();

}


