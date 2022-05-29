#include "AccountManage.h"
#include"AccountImp.h"
#include"RoomManage.h"
#include"MyDB.h"
#include "roomManage.h"

AccountManage::AccountManage()
{

}

//策略选取
void AccountManage::manage(QTcpSocket* tcpSocket,Protocol& dataPack){
    switch (dataPack.getType()) {
    case Protocol::rgst:
        rgst(tcpSocket,dataPack);
        break;
    case Protocol::login:
        login(tcpSocket,dataPack);
        break;
    case Protocol::ClientExit:
        clientExit(tcpSocket,dataPack);
        break;
    case Protocol::ClientDelete:
        clientDelete(tcpSocket,dataPack);
        break;
    case Protocol::showUserInfo:
        showUserInfo(tcpSocket,dataPack);
        break;
    case Protocol::chargeMoney:
        chargeMoney(tcpSocket,dataPack);
        break;
    default:
        break;
    }
}

//账户注册
void AccountManage::rgst(QTcpSocket* tcpSocket,Protocol& dataPack){
    AccountDao* pBase = new AccountImp;
    if(pBase->hasAccount(dataPack)){
        qDebug() << "账号已存在";
        dataPack["result"] = "rgstFalse";
    }else{
        if(pBase->rgst(dataPack)){
            qDebug() << "注册账号成功";
            dataPack["result"] = "rgstTrue";
        }else{
            qDebug() << "账号注册失败";
            dataPack["result"] = "rgstFalse";
        }
    }

    tcpSocket->write(dataPack.pack());
}
//账户登录
void AccountManage::login(QTcpSocket* tcpSocket,Protocol& dataPack){
    AccountDao* pBase = new AccountImp;

    if(pBase->checkInfo(dataPack)){     //账号密码正确且账户不在线
        if(pBase->ModifyOnline(dataPack)){
            qDebug() << "登录账号成功";
            dataPack["result"] = "loginTrue";
        }else{
            qDebug() << "登录账号失败";
            dataPack["result"] = "loginFalse";
        }
    }else{
        qDebug() << "账号密码有误或账号已在线";
        dataPack["result"] = "loginFalse";
    }

    tcpSocket->write(dataPack.pack());

    //更新房间列表
    //RoomManage* rm = new RoomManage;
    //rm->refreshRoomList();
}
//账户退出
void AccountManage::clientExit(QTcpSocket* tcpSocket,Protocol& dataPack){
    AccountDao* pBase = new AccountImp;
    if(pBase->hasAccount(dataPack)){
        if(pBase->ModifyOnline(dataPack)){
            qDebug() << "客户端退出成功";
            dataPack["result"] = "exitTrue";
        }else{
            qDebug() << "客户端退出失败";
            dataPack["result"] = "exitFalse";
        }
    }else{
        qDebug() << "账户不存在，客户端退出失败";
        dataPack["result"] = "exitFalse";
    }

    tcpSocket->write(dataPack.pack());
}
//账户注销
void AccountManage::clientDelete(QTcpSocket* tcpSocket,Protocol& dataPack){
    AccountDao* pBase = new AccountImp;
    if(pBase->hasAccount(dataPack)){
        if(pBase->deleteClient(dataPack)){
            qDebug() << "客户端注销成功";
            dataPack["result"] = "deleteTrue";
        }else{
            qDebug() << "客户端注销失败";
            dataPack["result"] = "deleteFalse";
        }
    }else{
        qDebug() << "账户不存在，客户端注销失败";
        dataPack["result"] = "deleteFalse";
    }

    tcpSocket->write(dataPack.pack());
}
//显示信息
void AccountManage::showUserInfo(QTcpSocket *tcpSocket, Protocol &dataPack){
    MyDB* db = MyDB::getInstance();
    db->createConnection();

    QSqlQuery query;
    query.prepare("select * from info where account = ?");
    query.addBindValue(dataPack["userAccount"].toString());
    int ret = query.exec();
    if(!ret){
        qDebug() << query.lastError().text();
        dataPack["result"] = "showUserInfoFalse";
    }

    double _money = 0;
    int _score = 0;
    while(query.next()){
        _money = query.value(4).toDouble();
        _score = query.value(5).toInt();
    }

    dataPack["result"] = "showUserInfoTrue";
    dataPack["money"] = _money;
    dataPack["score"] = _score;

    db->destroyConnection();
    tcpSocket->write(dataPack.pack());
}
//账户充值
void AccountManage::chargeMoney(QTcpSocket* tcpSocket,Protocol& dataPack){
    AccountDao* pBase = new AccountImp;
    if(pBase->hasAccount(dataPack)){
        if(pBase->chargeMoney(dataPack)){
            qDebug() << "账户充值成功";
            dataPack["result"] = "chargeMoneyTrue";
        }else{
            qDebug() << "账户充值失败";
            dataPack["result"] = "chargeMoneyFalse";
        }
    }else{
        qDebug() << "账户不存在，充值失败";
        dataPack["result"] = "chargeMoneyFalse";
    }

    tcpSocket->write(dataPack.pack());
}





