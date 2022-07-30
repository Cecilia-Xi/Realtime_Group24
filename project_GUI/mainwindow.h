#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "../Executive.h"
#include "../cppThread/CppThread.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class Detect_Thread : public Executive {
public:
    void run(){
        search_withQT();
    }
};

class Add_Thread : public Executive{
    void run(){
        add_withQT();
    }
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


    void detect();
    void add();

private slots:

    void on_pushButton_clicked();


    void on_pushButton_2_clicked();
private:

    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
