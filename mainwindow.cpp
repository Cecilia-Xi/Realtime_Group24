#include "mainwindow.h"
#include <QApplication>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QFileDialog>
#include <QDebug>
#include <QToolBar>
#include <QPainter>
#include <QPixmap>
#include <QMouseEvent>
#include <QCursor>
#include <QIcon>
#include <QEvent>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    
    QMenuBar * pMenuBar = menuBar();
    QMenu *menu = pMenuBar->addMenu("File");
    _menu = menu;
 
    QAction *openAction = menu->addAction("&Open", this, SLOT(slotOpen()), QKeySequence::Open);
    QAction *saveAction = menu->addAction("&Save", this, SLOT(slotSave()), QKeySequence::Save);
    menu->addSeparator();
    QAction *closeAction = menu->addAction("&Exit", this, SLOT(close()), QKeySequence::Close);
 
    
    QToolBar *toolBar = this->addToolBar("MyToolBar");
    toolBar->addAction(openAction);
    toolBar->addAction(saveAction);
    toolBar->addAction(closeAction);
 
 
 
 
    _view = new MyView;
    this->setCentralWidget(_view);
 

}
 

void MainWindow::mousePressEvent(QMouseEvent *ev)
{
    if(ev->button() == Qt::RightButton)
        _menu->exec(QCursor::pos());
}
 
void MainWindow::slotSave()
{
    QFileDialog::getSaveFileName();
 
}
 
void MainWindow::slotOpen()
{
    QString strFile = QFileDialog::getOpenFileName();
    qDebug() << strFile;
}
 
 
int main(int argc, char **argv)
{
    QApplication app(argc, argv);
 
    MainWindow w;
    w.setGeometry(300, 130, 700, 500);
    w.show();
 
    app.exec();
}

