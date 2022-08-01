#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);
}

MainWindow::~MainWindow() {

    delete ui;

}

void MainWindow::on_pushButton_clicked() {
    qDebug()<<"click start and main-thread id"<<QThread::currentThreadId();
    e1.start_Search();
    e1.join();

}

void MainWindow::on_pushButton_2_clicked() {
    qDebug()<<"click start and main-thread id"<<QThread::currentThreadId();
    e1.start_Add();
    e1.join();

}


