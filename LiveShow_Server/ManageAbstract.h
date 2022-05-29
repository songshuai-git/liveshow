#ifndef MANAGEABSTRACT_H
#define MANAGEABSTRACT_H

#include"QTcpSocket"
#include"protocol.h"

class ManageAbstract
{
public:
    ManageAbstract();
    virtual ~ManageAbstract();  //多态时基类的析构函数必须虚函数

    virtual void manage(QTcpSocket* tcpSocket,Protocol& dataPack)=0;
};

#endif // MANAGEABSTRACT_H
