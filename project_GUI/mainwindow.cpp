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
    // Start a thread to run the search function.
    m_exec.start_Search(); 
    // join the thread
    m_exec.join(); 

}

void MainWindow::on_pushButton_2_clicked() {
    qDebug()<<"click start and main-thread id"<<QThread::currentThreadId();
    // Start a thread to run the add function.
    m_exec.start_Add(); 
    // join the thread
    m_exec.join(); 

}


