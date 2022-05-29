#include "SockContainer.h"
#include"QDebug"

SockContainer* SockContainer::instance = 0;

SockContainer* SockContainer::getInstance(){
    if(instance==0){
        instance = new SockContainer;
    }
    return instance;
}
SockContainer::SockContainer(QObject* parent)
    :QObject(parent)
{

}

void SockContainer::insertInto_SockContainer(QTcpSocket* tcpSocket){
    tcpSocket_vect.push_back(tcpSocket);
    qDebug() << "已将套接字加入容器中";
}

