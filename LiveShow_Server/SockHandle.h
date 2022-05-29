#ifndef SOCKHANDLE_H
#define SOCKHANDLE_H

#include"QTcpSocket"
#include"QObject"

class SockHandle:public QObject
{
    Q_OBJECT
public:
    explicit SockHandle(QObject* parent = 0);
    explicit SockHandle(QObject* parent = 0,QTcpSocket* tcpSocket = 0);

private slots:
    void readyRead_slot();

private:
    QTcpSocket* tcpSocket;
};

#endif // SOCKHANDLE_H
