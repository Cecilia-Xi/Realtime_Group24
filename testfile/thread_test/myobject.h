#ifndef MYOBJECT_H
#define MYOBJECT_H


#include <QObject>
#include <QTimer>
class MyObject : public QObject
{
    Q_OBJECT
public:
    explicit MyObject(QObject *parent = nullptr);
    ~MyObject();

    QTimer *timer;

public slots:
    void slotStart();


signals:

};
#endif
