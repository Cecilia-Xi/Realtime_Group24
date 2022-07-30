#include "myobject.h"
#include <QDebug>
#include <QThread>
#include <QDateTime>
MyObject::MyObject(QObject *parent) : QObject(parent)
{

}

MyObject::~MyObject()
{
    qDebug()<<"MyObject Destructor";
}

void MyObject::slotStart()
{
    qDebug()<<"child-thread id"<<QThread::currentThreadId()<<QDateTime::currentDateTime();
    timer = new QTimer(this);
    connect(timer,&QTimer::timeout,this,[=](){
        for(int i = 0;i<5;i++)
        {
            QThread::sleep(1);
        }
        qDebug()<<"timer run"<<QDateTime::currentDateTime();
    });
    timer->start(2000);
}
