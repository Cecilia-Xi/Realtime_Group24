#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QMessageBox"

#include <wiringPi.h>
#include <wiringSerial.h>
#include "../Executive.h"

class Detect_Thread : public Executive {
    void run(){
        run_plain();
    }

};

class Add_Thread : public Executive{
    void run(){
        add_withQT();
    }
};


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


void MainWindow::qmessage()
{
    QMessageBox::information(this, "Detect", "put your finger!");
}
void MainWindow::qmessage2()
{
    QMessageBox::information(this, "Add", "put your finger!");
}
void MainWindow::on_pushButton_clicked()
{
    Detect_Thread dtct_thd;
    emit dtct_thd.start();
    connect(this, SIGNAL(dtct_thd.start()), this, SLOT(qmessage()),Qt::QueuedConnection);

    //dtct_thd.start();
    dtct_thd.join();
}

void MainWindow::on_pushButton_2_clicked()
{
    Add_Thread add_thd;
    emit add_thd.start();
    connect(this, SIGNAL(dtct_thd.start()), this, SLOT(qmessage2()),Qt::QueuedConnection);


    //add_thd.start();
    add_thd.join();
}



