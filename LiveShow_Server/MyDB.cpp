#include "MyDB.h"
#include"QDebug"

MyDB* MyDB::instance = 0;

MyDB* MyDB::getInstance(){
    if(instance==0){
        instance = new MyDB;
    }
    return instance;
}
MyDB::MyDB()
{
    db = QSqlDatabase::addDatabase("QMYSQL");
}

bool MyDB::createConnection(){
    db.setHostName("localhost");
    db.setPort(3306);   //?
    db.setUserName("root");
    db.setPassword("123456");
    db.setDatabaseName("userInfo");
    if(!db.open()){
        qDebug() << db.lastError().text();
        return false;
    }
    return true;
}
void MyDB::destroyConnection(){
    db.close();\
    db.removeDatabase("userInfo");
}

