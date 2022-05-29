#include "AccountImp.h"
#include"MyDB.h"
#include"protocol.h"
#include"QTcpSocket"
#include"QSqlQuery"
#include"QSqlError"

AccountImp::AccountImp()
{

}

//账户查重
bool AccountImp::hasAccount(Protocol& dataPack){
    MyDB* db = MyDB::getInstance();
    db->createConnection();

    QSqlQuery query;
    query.prepare("select * from info where account = :account");
    query.bindValue(":account",dataPack["userAccount"].toString());

    if(!query.exec()){
        qDebug() << query.lastError().text();
    }
    bool ret = query.next();

    db->destroyConnection();
    return ret;
}
//账户注册
bool AccountImp::rgst(Protocol& dataPack){
    MyDB* db = MyDB::getInstance();
    db->createConnection();

    QSqlQuery query;
    query.prepare("insert into info(account,pswd) values(?,?)");
    query.addBindValue(dataPack["userAccount"].toString());
    query.addBindValue(dataPack["userPswd"].toString());

    int ret = query.exec();
    if(!ret){
        qDebug() << query.lastError().text();
    }

    db->destroyConnection();
    return ret;
}
//检查账号密码和在线状态
bool AccountImp::checkInfo(Protocol& dataPack){
    MyDB* db = MyDB::getInstance();
    db->createConnection();

    QSqlQuery query;
    query.prepare("select * from info where account = :account "
                  "and pswd = :pswd and online = 0");   //?null
    query.bindValue(":account",dataPack["userAccount"].toString());
    query.bindValue(":pswd",dataPack["userPswd"].toString());

    int ret = query.exec();
    if(!ret){
        qDebug() << query.lastError().text();
    }
    ret = query.next();     //返回值为1时，账号密码正确且账户不在线

    db->destroyConnection();
    return ret;
}
//修改在线状态
bool AccountImp::ModifyOnline(Protocol& dataPack){
    MyDB* db = MyDB::getInstance();
    db->createConnection();

    QSqlQuery query;
    query.prepare("update info set online = ? where account = ?");
    query.addBindValue(dataPack["online"].toInt());
    query.addBindValue(dataPack["userAccount"].toString());

    int ret = query.exec(); //!语法正确即可通过
    if(!ret){
        qDebug() << query.lastError().text();
    }

    db->destroyConnection();
    return ret;
}
//账户注销
bool AccountImp::deleteClient(Protocol& dataPack){
    MyDB* db = MyDB::getInstance();
    db->createConnection();

    QSqlQuery query;
    query.prepare("delete from info where account = ?");
    query.addBindValue(dataPack["userAccount"].toString());

    int ret = query.exec();
    if(!ret){
        qDebug() << query.lastError().text();
    }

    db->destroyConnection();
    return ret;
}
//账户充值
bool AccountImp::chargeMoney(Protocol& dataPack){
    MyDB* db = MyDB::getInstance();
    db->createConnection();

    QSqlQuery query;
    query.prepare("update info set money = money + ? where account = ?");
    query.addBindValue(dataPack["money"].toDouble());
    query.addBindValue(dataPack["userAccount"].toString());

    bool ret = query.exec();
    if(!ret){
        qDebug() << query.lastError().text();
    }

    db->destroyConnection();
    return ret;
}
