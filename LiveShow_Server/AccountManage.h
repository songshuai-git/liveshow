#ifndef ACCOUNTMANAGE_H
#define ACCOUNTMANAGE_H

#include"ManageAbstract.h"
#include"QTcpSocket"
#include"protocol.h"

class AccountManage:public ManageAbstract
{
public:
    AccountManage();

    virtual void manage(QTcpSocket* tcpSocket,Protocol& dataPack);

    void rgst(QTcpSocket* tcpSocket,Protocol& dataPack);
    void login(QTcpSocket* tcpSocket,Protocol& dataPack);
    void clientExit(QTcpSocket* tcpSocket,Protocol& dataPack);
    void clientDelete(QTcpSocket* tcpSocket,Protocol& dataPack);
    void showUserInfo(QTcpSocket* tcpSocket,Protocol& dataPack);
    void chargeMoney(QTcpSocket* tcpSocket,Protocol& dataPack);
};

#endif // ACCOUNTMANAGE_H
