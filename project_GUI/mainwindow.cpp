#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QMessageBox"

#include <wiringPi.h>
#include <wiringSerial.h>
#include "../Executive.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}
/*
void MainWindow::detect(){
    Detect_Thread dtct_thd;
    dtct_thd.start();
    QMessageBox::information(this, "Detect", "put your finger!");
    dtct_thd.join();
}

void MainWindow::add(){
    Add_Thread add_thd;
    add_thd.start();
    QMessageBox::information(this, "Add", "put your finger!");
    add_thd.join();
}
*/
void MainWindow::on_pushButton_clicked()
{
	e1.search_withQT();
    QMessageBox::information(this, "Detect", "put your finger!");
    
}

void MainWindow::on_pushButton_2_clicked()
{
    e1.add_withQT();
    QMessageBox::information(this, "Add", "put your finger!");
   
}



