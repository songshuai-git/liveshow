#ifndef SOCKCONTAINER_H
#define SOCKCONTAINER_H

#include"QObject"
#include"QVector"
#include"QTcpSocket"
class SockContainer:public QObject
{
    Q_OBJECT
public:
    static SockContainer* getInstance();
    void insertInto_SockContainer(QTcpSocket* tcpSocket);
    QVector<QTcpSocket*> get_allSockets()const{return tcpSocket_vect;}
private:
    static SockContainer* instance;
    SockContainer(QObject* parent = 0);

    QVector<QTcpSocket*> tcpSocket_vect;    //???编号
};

#endif // SOCKCONTAINER_H
