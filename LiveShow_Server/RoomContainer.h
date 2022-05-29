#ifndef ROOMCONTAINER_H
#define ROOMCONTAINER_H

#include"QVector"
#include"RoomInfo.h"
#include"QTcpSocket"
#include"protocol.h"

class RoomContainer
{
public:
    static RoomContainer* getInstance();

    QVector<RoomInfo> get_allRooms()const{return roomVect;}
    bool insertInto_roomVect(QTcpSocket* tcpSocket,Protocol& dataPack);
    bool eraseFrom_roomVect(QString hostAccount);
    bool hasRoom(Protocol& dataPack);
    bool insertInto_roomInfo(QTcpSocket* tcpSocket,Protocol& dataPack);
    bool eraseAudience(QString _hostAccount,QString _userAccount);

private:
    static RoomContainer* instance;
    RoomContainer();

    /*
     * roomInfo只是一种数据类型而已，是用来存东西的。而RoomContainer这个类更像是对这个类型的操作，
     * 类似于函数，即这里面有很多直播间对象，对这些直播间对象的处理由这类来完成。
     */
    QVector<RoomInfo> roomVect;
};

#endif // ROOMCONTAINER_H
