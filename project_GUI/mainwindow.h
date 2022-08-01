#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDebug>
#include <QThread>
#include <QObject>
#include <QMessageBox>
#include <QMainWindow>
#include "QThread"
#include "../Executive.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT
public:
    QThread *detect_thread;//QThread pointer
    QThread *add_thread;//QThread pointer
    //Executive e1;//Executive pointer
    /*
     * public member function
    * intro: constructor
    * param: parentpointer, default value = nullptr
    * return: none
    */
    MainWindow(QWidget *parent = nullptr);
    /*
     * public member function
    * intro:
    * param: none
    * return: none
    */
    ~MainWindow();

public slots:
    /*
     * public member slots function
    * intro: once clicked the button, then goto search thread
    * param: none
    * return: none
    */
    void on_pushButton_clicked();

    /*
     * public member slots function
    * intro:once clicked the button, then goto add thread
    * param: none
    * return: none
    */
    void on_pushButton_2_clicked();

signals:
    /*
     * signal function
    * intro: sendout signal of finger on the sensor
    * param: none
    * return: none
    */
    void detect_signalStart();
    void add_signalStart();

private:
    Executive m_exec;
    Ui::MainWindow *ui;//MainWindow pointer


};

#endif // MAINWINDOW_H
