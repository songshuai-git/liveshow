#ifndef ROOMMANAGE_H
#define ROOMMANAGE_H

#include"ManageAbstract.h"

class RoomManage:public ManageAbstract
{
public:
    RoomManage();

    virtual void manage(QTcpSocket* tcpSocket,Protocol& dataPack);

    void roomCreate(QTcpSocket* tcpSocket,Protocol& dataPack);
    void joinRoom(QTcpSocket* tcpSocket,Protocol& dataPack);
    void refreshRoomList();
    void refreshAudienceList(QTcpSocket* tcpSocket);
    void sendMsg(Protocol& dataPack);
    void backToHall(Protocol& dataPack);
    void sendGift(QTcpSocket* tcpSocket,Protocol& dataPack);
};

#endif // ROOMMANAGE_H
