#include "DataPack_Handle.h"
#include"AccountManage.h"
#include"roomManage.h"

DataPack_Handle::DataPack_Handle()
{

}

void DataPack_Handle::strategy(QTcpSocket* tcpSocket,Protocol& dataPack){
    ManageAbstract* pBase = nullptr;

    switch(dataPack.getType()){
        case Protocol::none:
            break;
        case Protocol::rgst:                    //账户注册
            pBase = new AccountManage;//多态
            break;
        case Protocol::login:
            pBase = new AccountManage;          //账户登录
            break;
        case Protocol::ClientExit:
            pBase = new AccountManage;          //账户退出
            break;
        case Protocol::ClientDelete:            //账户注销
            pBase = new AccountManage;
            break;
        case Protocol::showUserInfo:            //显示信息
            pBase = new AccountManage;
            break;
        case Protocol::chargeMoney:             //账户充值
            pBase = new AccountManage;
            break;

        case Protocol::RoomCreate:              //创建房间
            pBase = new RoomManage;
            break;
        case Protocol::JoinRoom:                //加入房间
            pBase = new RoomManage;
            break;
        case Protocol::refreshRoomList:         //刷新房间列表
            pBase = new RoomManage;
            break;
        case Protocol::refreshAudienceList:     //刷新游客列表
            pBase = new RoomManage;
            break;
        case Protocol::sendMsg:                 //群聊
            pBase = new RoomManage;
            break;
        case Protocol::backToHall:              //退出直播
            pBase = new RoomManage;
            break;
        case Protocol::sendGift:              //退出直播
            pBase = new RoomManage;
            break;
        default:
            break;
    }

    if(pBase!=nullptr){
        pBase->manage(tcpSocket,dataPack);  //调用重写的方法
        delete pBase;
    }
}
