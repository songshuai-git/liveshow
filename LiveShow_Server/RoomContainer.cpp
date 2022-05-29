#include "RoomContainer.h"

RoomContainer* RoomContainer::instance = 0;

RoomContainer* RoomContainer::getInstance(){
    if(instance==0){
        instance = new RoomContainer;
    }
    return instance;
}
RoomContainer::RoomContainer()
{

}

//查重
bool RoomContainer::hasRoom(Protocol& dataPack){
    for(int i=0;i<roomVect.size();i++){
        if(roomVect[i].get_roomName()==dataPack["roomName"].toString()){
            return true;
        }
    }
    return false;
}
//增加直播间
bool RoomContainer::insertInto_roomVect(QTcpSocket* tcpSocket,Protocol& dataPack){
    if(hasRoom(dataPack)){
        return false;
    }
    RoomInfo room(tcpSocket,dataPack["userAccount"].toString(),dataPack["roomName"].toString(),
                  dataPack["address"].toString(),dataPack["cameraPort"].toInt(),
                  dataPack["audioPort"].toInt());
    roomVect.push_back(room);
    return true;
}
//删除直播间
bool RoomContainer::eraseFrom_roomVect(QString hostAccount){
    QVector<RoomInfo>::iterator it;
    for(it=roomVect.begin();it!=roomVect.end();it++){
        if(it->get_host()==hostAccount){       //!==
            roomVect.erase(it);
            return true;
        }
    }
    return false;
}
//删除观众
bool RoomContainer::eraseAudience(QString _hostAccount, QString _userAccount){
    QVector<RoomInfo>::iterator it;
    for(it=roomVect.begin();it!=roomVect.end();it++){
        if(it->get_host()==_hostAccount){
            it->eraseAudience(_userAccount);
            return true;
        }
    }
    return false;
}
//添加观众
bool RoomContainer::insertInto_roomInfo(QTcpSocket* tcpSocket,Protocol& dataPack){
    QString roomName;
    for(int i=0;i<roomVect.size();i++){
        roomName = roomVect[i].get_roomName();
        if(dataPack["roomName"]==roomName){     //找到游客要加入的直播间
            QString userAccount = dataPack["userAccount"].toString();
            Audience per = {tcpSocket,userAccount}; //结构体的初始化 ??tcpSocket不一样
            roomVect[i].insertInto_audienceVect(per);
            return true;
        }
    }
    return false;
}




