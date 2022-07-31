#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QThread"
#include <QMessageBox>
#include "Executive.h"
#include <QDebug>
#include <QThread>
#include <QObject>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    //QThread *thread;
    Executive *e1;
public slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

signals:
    void signalStart();
private:
    Ui::MainWindow *ui;
    QThread *thread;

};

#endif // MAINWINDOW_H
