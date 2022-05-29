#include "RoomInfo.h"

RoomInfo::RoomInfo()
{

}
RoomInfo::RoomInfo(QTcpSocket* tcpSocket,QString host,QString roomName)
    :tcpSocket(tcpSocket),host(host),roomName(roomName)
{

}
RoomInfo::RoomInfo(QTcpSocket* tcpSocket,QString host,QString roomName,
             QString address,int cameraPort,int audioPort)
    :tcpSocket(tcpSocket),host(host),roomName(roomName),
     address(address),cameraPort(cameraPort),audioPort(audioPort)
{

}

bool RoomInfo::eraseAudience(QString userAccount){
    QVector<Audience>::iterator it;
    for(it=audienceVect.begin();it!=audienceVect.end();it++){
        if(it->userAccount==userAccount){
            audienceVect.erase(it);
            return true;
        }
    }
    return false;
}
