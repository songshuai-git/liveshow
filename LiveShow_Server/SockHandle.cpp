#include "SockHandle.h"
#include"protocol.h"
#include"QJsonValue"
#include"DataPack_Handle.h"

SockHandle::SockHandle(QObject* parent)
    :QObject(parent)
{

}
SockHandle::SockHandle(QObject* parent,QTcpSocket* tcpSocket)
    :QObject(parent),tcpSocket(tcpSocket)
{
    connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(readyRead_slot()));
}

//通信
void SockHandle::readyRead_slot(){
    QByteArray byte;
    byte = tcpSocket->readAll();

    Protocol dataPack;
    if(dataPack.unpack(byte) >0){   //??用while包一次拆不完
        qDebug() << "服务器收到数据包 类型:" << dataPack.getType() << ";账户名:"
                 << dataPack["userAccount"].toString() << ";密码:"
                 << dataPack["userPswd"].toString() << ";房间名:"
                 << dataPack["roomName"].toString();

        DataPack_Handle* handle = new DataPack_Handle;
        handle->strategy(tcpSocket,dataPack);

        delete handle;  //?
    }
}
