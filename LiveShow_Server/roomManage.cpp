#include "RoomManage.h"
#include"RoomContainer.h"
#include"SockContainer.h"
#include"MyDB.h"

RoomManage::RoomManage()
{

}

//策略选取
void RoomManage::manage(QTcpSocket* tcpSocket,Protocol& dataPack){
    switch(dataPack.getType()){
        case Protocol::RoomCreate:
            roomCreate(tcpSocket,dataPack);
            break;
        case Protocol::refreshRoomList:
            refreshRoomList();
            break;
        case Protocol::refreshAudienceList:
            refreshAudienceList(tcpSocket);
            break;
        case Protocol::JoinRoom:
            joinRoom(tcpSocket,dataPack);
            break;
        case Protocol::sendMsg:
            sendMsg(dataPack);
            break;
        case Protocol::backToHall:
            backToHall(dataPack);
            break;
        case Protocol::sendGift:
            sendGift(tcpSocket,dataPack);
            break;
        default:
            break;
    }
}
//创建房间
void RoomManage::roomCreate(QTcpSocket* tcpSocket,Protocol& dataPack){
    RoomContainer* single = RoomContainer::getInstance();

    //设置该组ip,cameraPort,audioPort
    //224.0.0.0---239.255.255.255
    QString _address = "224.0.0.";
    static int ipPart = 0;
    _address += QString::number(ipPart++);  //!==
    int _cameraPort = 1001;
    int _audioPort = 1002;
    dataPack["address"] = _address;
    dataPack["cameraPort"] = _cameraPort;
    dataPack["audioPort"] = _audioPort;

    int ret = single->insertInto_roomVect(tcpSocket,dataPack);
    if(ret){
        dataPack["result"] = "createTrue";
    }else{
        qDebug() << "房间名重复，房间创建失败";
        dataPack["result"] = "createFalse";
    }

    //取出主播信息
    QString userAccount = dataPack["userAccount"].toString();
    double _money = 0;
    int _score = 0;

    MyDB* db = MyDB::getInstance();
    db->createConnection();
    QSqlQuery query;
    query.prepare("select * from info where account = ?");
    query.addBindValue(userAccount);
    ret = query.exec();
    if(!ret){
        qDebug() << query.lastError().text();
    }
    while(query.next()){
        _money = query.value(4).toDouble();  //取出该用户余额
        _score = query.value(5).toInt();     //取出改用户积分
    }
    dataPack["money"] = _money;
    dataPack["score"] = _score;
    db->destroyConnection();

    tcpSocket->write(dataPack.pack());
    //刷新直播间列表
    //refreshRoomList();
}
//刷新房间列表
void RoomManage::refreshRoomList(){
    RoomContainer* roomSingle = RoomContainer::getInstance();
    SockContainer* sockSingle = SockContainer::getInstance();
    QVector<RoomInfo> roomVect = roomSingle->get_allRooms();
    QVector<QTcpSocket*> sockContainer = sockSingle->get_allSockets();

    Protocol dataPack2(Protocol::refreshRoomList);     //!???数据包不能同名，否则只会发一个

    QVector<RoomInfo>::iterator it;                    //将所有的直播间名字压入到数据包
    int count = 0;
    for(it=roomVect.begin();it!=roomVect.end();it++){
        dataPack2[QString::number(count++)] = (*it).get_roomName();
    }

    QVector<QTcpSocket*>::iterator it2;               //给所有客户端发包 ???给自己发包呢
    for(it2=sockContainer.begin();it2!=sockContainer.end();it2++){
        (*it2)->write(dataPack2.pack());
    }
}
//刷新游客列表
void RoomManage::refreshAudienceList(QTcpSocket* tcpSocket){
    RoomContainer* rc = RoomContainer::getInstance();
    QVector<RoomInfo> ri = rc->get_allRooms();          //取出所有直播间
    QVector<Audience> ad;

    int i = 0,j = 0;
    int flag = 0;
    for(i=0;i<ri.size();i++){
        ad = ri[i].get_audience();                      //取出每个直播间的游客
        for(j=0;j<ad.size();j++){
            if(ad[j].tcpSocket==tcpSocket){             //找到游客进入的直播间
                flag = 1;
                break;
            }
        }
        if(flag==1) break;
    }

    Protocol dataPack(Protocol::refreshAudienceList);
    int count = 0;
    for(int k=0;k<ad.size();k++){                       //将该直播间下的所有观众放入数据包
        dataPack[QString::number(count++)] = ad[k].userAccount;
    }

    ri[i].get_tcpSocket()->write(dataPack.pack());      //主播的游客列表更新
    for(int k=0;k<ad.size();k++){
        ad[k].tcpSocket->write(dataPack.pack());        //所有观众的游客列表更新
    }
}
//加入房间
void RoomManage::joinRoom(QTcpSocket* tcpSocket,Protocol& dataPack){
    //添加游客
    RoomContainer* single = RoomContainer::getInstance();
    bool ret = single->insertInto_roomInfo(tcpSocket,dataPack);
    if(ret){
        dataPack["result"] = "joinTrue";
    }else{
        qDebug() << "加入房间失败";
        dataPack["result"] = "joinFalse";
    }

    //取出游客信息
    QString userAccount = dataPack["userAccount"].toString();
    QString roomName = dataPack["roomName"].toString();
    double _money = 0;
    int _score = 0;

    MyDB* db = MyDB::getInstance();
    db->createConnection();
    QSqlQuery query;
    query.prepare("select * from info where account = ?");
    query.addBindValue(userAccount);
    ret = query.exec();
    if(!ret){
        qDebug() << query.lastError().text();
    }
    while(query.next()){
        _money = query.value(4).toDouble();  //取出该用户余额
        _score = query.value(5).toInt();     //取出改用户积分
    }
    dataPack["money"] = _money;
    dataPack["score"] = _score;
    db->destroyConnection();

    //取出主播账号，该组ip和port
    QString _hostAccount;
    QString _address;
    int _cameraPort;
    int _audioPort;
    QVector<RoomInfo> roomVect = single->get_allRooms();
    for(int i=0;i<roomVect.size();i++){
        if(roomVect[i].get_roomName()==roomName){
            _hostAccount = roomVect[i].get_host();
            _address = roomVect[i].get_address();
            _cameraPort = roomVect[i].get_cameraPort();
            _audioPort = roomVect[i].get_audioPort();
            break;
        }
    }
    dataPack["hostAccount"] = _hostAccount;
    dataPack["address"] = _address;
    dataPack["cameraPort"] = _cameraPort;
    dataPack["audioPort"] = _audioPort;

    tcpSocket->write(dataPack.pack());
}
//群聊
void RoomManage::sendMsg(Protocol& dataPack){
    RoomContainer* rc = RoomContainer::getInstance();
    QVector<RoomInfo> ri= rc->get_allRooms();

    QString _roomName = dataPack["roomName"].toString();
    int i = 0;
    for(i=0;i<ri.size();i++){
        if(ri[i].get_roomName()==_roomName){            //找到该直播间
            break;
        }
    }

    QVector<Audience> ad = ri[i].get_audience();        //取出该直播间的所有观众

    ri[i].get_tcpSocket()->write(dataPack.pack());      //主播的聊天记录更新
    for(int j=0;j<ad.size();j++){
        ad[j].tcpSocket->write(dataPack.pack());        //该直播间的所有观众的聊天记录更新
    }
}
//退出直播
void RoomManage::backToHall(Protocol& dataPack){
    QString _hostAccount = dataPack["hostAccount"].toString();
    QString _userAccount = dataPack["userAccount"].toString();
    RoomContainer* rc = RoomContainer::getInstance();
    QVector<RoomInfo> ri = rc->get_allRooms();
    QVector<Audience> ad;

    if(_hostAccount==_userAccount){                            //当前用户为主播
        for(int i=0;i<ri.size();i++){
            if(ri[i].get_host()==_hostAccount){                //找到该直播间
                ad = ri[i].get_audience();                     //取出该直播间下的所有观众
                for(int j=0;j<ad.size();j++){
                    dataPack["isHost"] = "false";
                    ad[j].tcpSocket->write(dataPack.pack());   //给该直播间下所有观众发退出包
                }
                //???发包是进程结束之后再发包吗 那之前给观众发的包isHost是不是也变为true了呢
                dataPack["isHost"] = "true";
                ri[i].get_tcpSocket()->write(dataPack.pack()); //给该主播发退出包
            }
        }
        rc->eraseFrom_roomVect(_hostAccount);   //???tcpSocket都被擦出了，还能发包吗
    }else{                                                      //当前用户为游客
        dataPack.setType(Protocol::AudienceBackToHall);
        for(int i=0;i<ri.size();i++){
            if(ri[i].get_host()==_hostAccount){                 //找到该直播间
                ad=ri[i].get_audience();                        //取出该直播间下的所有观众
                for(int j=0;j<ad.size();j++){
                    if(ad[j].userAccount==_userAccount){        //找到该观众
                        dataPack["isThisUser"] = "true";
                        ad[j].tcpSocket->write(dataPack.pack());        // 给该观众发退出包
                        rc->eraseAudience(_hostAccount,_userAccount);   //!删除该观众 !!!若用引用的
                                                                        //!的方法可以更好解决删除问题
                        ri = rc->get_allRooms();                //取出该直播间改变后的观众
                        ad = ri[i].get_audience();

                        int count = 0;
                        for(int k=0;k<ad.size();k++){           //将该直播间下的所有观众放入数据包
                            dataPack[QString::number(count++)] = ad[k].userAccount;
                        }

                        dataPack["isThisUser"] = "false";
                        ri[i].get_tcpSocket()->write(dataPack.pack());   //主播的游客列表更新
                        for(int k=0;k<ad.size();k++){
                            ad[k].tcpSocket->write(dataPack.pack());     //所有观众的游客列表更新
                        }
                        return;
                    }
                }
            }
        }
    }
}
//送出礼物
void RoomManage::sendGift(QTcpSocket* tcpSocket,Protocol& dataPack){
    QString _hostName = dataPack["hostName"].toString();
    QString _userName = dataPack["userName"].toString();
    int _value = dataPack["value"].toInt();
    double moneyInDB = 0;
    bool ret;

    //修改数据库:主播的金钱增加，该游客的金钱减少
    MyDB* db = MyDB::getInstance();
    db->createConnection();

    QSqlQuery query;

    query.prepare("select * from info where account = ?");      //判断该游客余额是否充足
    query.addBindValue(_userName);
    ret = query.exec();
    if(!ret){
        qDebug() << query.lastError().text();
    }
    while(query.next()){
        moneyInDB = query.value(4).toDouble();
    }
    if(moneyInDB<_value){
        dataPack["result"] = "sendGiftFalse";
        tcpSocket->write(dataPack.pack());
        return;
    }

    query.prepare("update info set money = money - ? where account = ?");   //该游客金钱减少
    query.addBindValue(_value);
    query.addBindValue(_userName);
    ret = query.exec();
    if(!ret){
        qDebug() << query.lastError().text();
    }

    query.prepare("update info set money = money + ? where account = ?");   //该主播金钱增加
    query.addBindValue(_value);
    query.addBindValue(_hostName);
    ret = query.exec();
    if(!ret){
        qDebug() << query.lastError().text();
    }

    RoomContainer* rc = RoomContainer::getInstance();
    QVector<RoomInfo> ri = rc->get_allRooms();
    QVector<Audience> ad;
    QString tempName;
    double tempMoney;
    int tempScore;
    int i = 0;
    for(i=0;i<ri.size();i++){
        if(ri[i].get_host()==_hostName){            //找到该直播间
            ad = ri[i].get_audience();              //取出该直播间下的所有观众
            break;
        }
    }

    dataPack["isHost"] = "true";                    //给该主播发包
    dataPack["score"] = _value*10;
    ri[i].get_tcpSocket()->write(dataPack.pack());

    dataPack["isHost"] = "false";
    for(int j=0;j<ad.size();j++){                   //给该直播间下的所有观众发包
        tempName = ad[j].userAccount;
        query.prepare("select * from info where account = ?");
        query.addBindValue(tempName);
        ret = query.exec();
        if(!ret){qDebug()<<query.lastError().text();}
        while(query.next()){
            tempMoney = query.value(4).toDouble();
            tempScore = query.value(5).toInt();
        }
        dataPack["money"] = tempMoney;
        dataPack["score"] = tempScore;
        ad[j].tcpSocket->write(dataPack.pack());
    }

    db->destroyConnection();
}

