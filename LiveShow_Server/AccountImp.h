#ifndef ACCOUNTIMP_H
#define ACCOUNTIMP_H

#include"AccountDao.h"
#include"ManageAbstract.h"

class AccountImp:public AccountDao
{
public:
    AccountImp();

    virtual bool hasAccount(Protocol& dataPack);
    virtual bool rgst(Protocol& dataPack);
    virtual bool ModifyOnline(Protocol& dataPack);
    virtual bool checkInfo(Protocol& dataPack);
    virtual bool deleteClient(Protocol& dataPack);
    virtual bool chargeMoney(Protocol& dataPack);
};

#endif // ACCOUNTIMP_H
