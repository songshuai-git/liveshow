#ifndef MYDB_H
#define MYDB_H

#include"QSqlDatabase"
#include"QSqlError"
#include"QSqlQuery"

class MyDB
{
public:
    static MyDB* getInstance();
    bool createConnection();
    void destroyConnection();

private:
    MyDB();
    static MyDB* instance;
    QSqlDatabase db;
};

#endif // MYDB_H
