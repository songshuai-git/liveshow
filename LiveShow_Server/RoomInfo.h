#ifndef ROOMINFO_H
#define ROOMINFO_H

#include"QTcpSocket"
#include"QVector"

struct Audience{
    QTcpSocket* tcpSocket;
    QString userAccount;
};

class RoomInfo
{
public:
    RoomInfo();
    RoomInfo(QTcpSocket* tcpSocket,QString host,QString roomName);
    RoomInfo(QTcpSocket* tcpSocket,QString host,QString roomName,
             QString address,int cameraPort,int audioPort);

    void insertInto_audienceVect(Audience per){audienceVect.push_back(per);}

    QTcpSocket* get_tcpSocket()const{return tcpSocket;}
    QString get_host()const{return host;}
    QString get_address()const{return address;}
    int get_cameraPort()const{return cameraPort;}
    int get_audioPort()const{return audioPort;}
    QString get_roomName()const{return roomName;}
    QVector<Audience> get_audience()const{return audienceVect;}
    bool eraseAudience(QString userAccount);

private:
    QTcpSocket* tcpSocket;
    QString host;
    QString roomName;
    QString address;
    int cameraPort;
    int audioPort;
    QVector<Audience> audienceVect;
};

#endif // ROOMINFO_H
