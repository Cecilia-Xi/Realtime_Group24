#ifndef MAINWINDOW_H
#define MAINWINDOW_H
 
#include <QMainWindow>
#include <QLabel>
#include "myview.h"

 
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    bool event(QEvent *);
    QLabel * _label;
    MyView * _view;
    QMenu * _menu;
    void slotActivated(QSystemTrayIcon::ActivationReason);
signals:
    
public slots:
    void slotOpen();
    void slotSave();
};
 
#endif // MAINWINDOW_H

