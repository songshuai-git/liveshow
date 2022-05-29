#ifndef ACCOUNTDAO_H
#define ACCOUNTDAO_H

#include"protocol.h"

class AccountDao
{
public:
    AccountDao();
    virtual ~AccountDao();

    virtual bool hasAccount(Protocol& dataPack) = 0;
    virtual bool rgst(Protocol& dataPack) = 0;
    virtual bool ModifyOnline(Protocol& dataPack) = 0;
    virtual bool checkInfo(Protocol& dataPack) = 0;
    virtual bool deleteClient(Protocol& dataPack) = 0;
    virtual bool chargeMoney(Protocol& dataPack) = 0;
};

#endif // ACCOUNTDAO_H
