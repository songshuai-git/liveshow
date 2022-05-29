#include "Client_w1.h"
#include "ui_Client_w1.h"
#include"QHostAddress"
#include"QMessageBox"
#include"protocol.h"

Client_w1::Client_w1(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Client_w1)
{
    ui->setupUi(this);

    //this->UiDesign();

    ui->lePswd->installEventFilter(this);
    ui->leAccount->installEventFilter(this);

    hall = ClientHall_w2::getInstance();    //包含二级窗体
    room = ClientRoom_w3::getInstance();    //包含三级窗体
    //与二级窗体的交互
    connect(hall,SIGNAL(exit_signal()),this,SLOT(exit_slot()));
    connect(hall,SIGNAL(delete_signal()),this,SLOT(delete_slot()));
    connect(hall,SIGNAL(createRoom_signal(QString)),this,SLOT(creatRoom_slot(QString)));
    connect(hall,SIGNAL(showUserInfo_signal()),this,SLOT(showUserInfo_slot()));
    connect(hall,SIGNAL(chargeMoney_signal(double)),this,SLOT(chargeMoney_slot(double)));
    connect(hall,SIGNAL(joinRoom_signal(QString)),this,SLOT(joinRoom_slot(QString)));
    //与三级窗体的交互
    connect(room,SIGNAL(backToHall_signal(QString,QString)),
            this,SLOT(backToHall_slot(QString,QString)));
    connect(room,SIGNAL(sendMsg_signal(QString,QString)),
            this,SLOT(sendMsg_slot(QString,QString)));
    connect(room,SIGNAL(sendGift_signal(QString,int)),
            this,SLOT(sendGift_slot(QString,int)));

    tcpSocket = new QTcpSocket(this);
    tcpSocket->connectToHost(QHostAddress("127.0.0.1"),8991);

    connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(readyRead_slot()));
}
Client_w1::~Client_w1()
{
    delete ui;
}

/*
//设置特效
void Client_w1::UiDesign()       //!==
{
    this->setWindowTitle("TCP客户端");
    this->setStyleSheet("color: black;");
    ui->btnRgst->setStyleSheet("QPushButton{"
                                 "background-color:blue;"   //背景颜色蓝色
                                 "color: black;"
                                 "border-radius: 10px;"     //圆角
                                 "border: 2px groove gray;"
                                 "border-style: outset;}"
                                 "QPushButton:pressed{background-color:rgb(85, 170, 255);"
                                 "border-style: inset; ""}");
    ui->btnRgst->setFlat (true);
    ui->btnLogin->setStyleSheet("QPushButton{"
                                 "background-color:green;"   //背景颜色绿色
                                 "color: black;"
                                 "border-radius: 10px;"     //圆角
                                 "border: 2px groove gray;"
                                 "border-style: outset;}"
                                 "QPushButton:pressed{background-color:rgb(85, 170, 255);"
                                 "border-style: inset; ""}");
    ui->btnLogin->setFlat (true);
}
*/

//事件过滤
bool Client_w1::eventFilter(QObject *watched, QEvent *event){
    if(watched==ui->lePswd){
        if(event->type()==QEvent::KeyPress){
            //将QEvent*类型转化为QkeyEvent*
            QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(event);
            //判定是否是大键盘上的enter
            if(keyEvent->key()==Qt::Key_Return || keyEvent->key()==Qt::Key_Enter){
                on_btnLogin_clicked();
                return true;
            }
        }
    }else if(watched==ui->leAccount){
        if(event->type()==QEvent::KeyPress){
            QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(event);
            if(keyEvent->key()==Qt::Key_Return || keyEvent->key()==Qt::Key_Enter){
                //设置焦点
                ui->lePswd->setFocus();
                return true;
            }
        }
    }
    return false;
}
//收包
void Client_w1::readyRead_slot(){
    QByteArray byte;
    byte = tcpSocket->readAll();
    Protocol dataPack;

    if(dataPack.unpack(byte)>0){
        switch(dataPack.getType()){
            case Protocol::none:
                break;
            case Protocol::rgst:
                if(dataPack["result"].toString()=="rgstTrue"){
                    QMessageBox::information(this,"注册","注册成功");
                }else if(dataPack["result"].toString()=="rgstFalse"){
                    QMessageBox::critical(this,"注册","注册失败");
                }else{
                    QMessageBox::critical(this,"注册","未知错误");
                }
                break;
            case Protocol::login:
                if(dataPack["result"].toString()=="loginTrue"){
                    QMessageBox::information(this,"登录","登录成功");
                    refresh_roomList();
                    this->close();
                    hall->show();
                }else if(dataPack["result"].toString()=="loginFalse"){
                    QMessageBox::critical(this,"登录","登录失败");
                }else{
                    QMessageBox::critical(this,"登录","未知错误");
                }
                break;
            case Protocol::ClientExit:
                if(dataPack["result"].toString()=="exitTrue"){
                    QMessageBox::information(hall,"退出","退出成功");
                    hall->close();
                }else if(dataPack["result"].toString()=="exitFalse"){
                    QMessageBox::critical(hall,"退出","退出失败");
                }else{
                    QMessageBox::critical(hall,"退出","未知错误");
                }
                break;
            case Protocol::ClientDelete:
                if(dataPack["result"].toString()=="deleteTrue"){
                    QMessageBox::information(hall,"注销","注销成功");
                    hall->close();
                    this->show();
                }else if(dataPack["result"].toString()=="deleteFalse"){
                    QMessageBox::critical(hall,"注销","注销失败");
                }else{
                    QMessageBox::critical(hall,"注销","未知错误");
                }
                break;
            case Protocol::RoomCreate:
                if(dataPack["result"].toString()=="createTrue"){
                    QMessageBox::information(hall,"建房","创建成功");
                    refresh_roomList();
                    room->createRoom(dataPack);
                    hall->close();
                    room->show();
                }else if(dataPack["result"].toString()=="createFalse"){
                    QMessageBox::critical(hall,"建房","创建失败");
                }else{
                    QMessageBox::critical(hall,"建房","未知错误");
                }
                break;
            case Protocol::refreshRoomList:
                hall->refresh_RoomList(dataPack);
                break;
            case Protocol::refreshAudienceList:
                room->refresh_audienceList(dataPack);
                break;
            case Protocol::showUserInfo:
                if(dataPack["result"].toString()=="showUserInfoTrue"){
                    hall->showUserInfo(dataPack);
                }else if(dataPack["result"].toString()=="showUserInfoFalse"){
                    QMessageBox::critical(hall,"提示","显示失败");
                }else{
                    QMessageBox::critical(hall,"提示","未知错误");
                }
                break;
            case Protocol::chargeMoney:
                if(dataPack["result"].toString()=="chargeMoneyTrue"){
                    QMessageBox::information(hall,"充值","充值成功");
                }else if(dataPack["result"].toString()=="chargeMoneyFalse"){
                    QMessageBox::critical(hall,"充值","充值失败");
                }else{
                    QMessageBox::critical(hall,"充值","未知错误");
                }
                break;
            case Protocol::JoinRoom:
                if(dataPack["result"].toString()=="joinTrue"){
                    refresh_audienceList();
                    room->joinRoom(dataPack);
                    hall->close();
                    room->show();
                }else if(dataPack["result"].toString()=="joinFalse"){
                    QMessageBox::critical(hall,"加入房间","加入失败");
                }else{
                    QMessageBox::critical(hall,"加入房间","未知错误");
                }
                break;
            case Protocol::sendMsg:
                room->show_chatRecord(dataPack);
                break;
            case Protocol::backToHall:
//                if(dataPack["isHost"].toString()=="true"){
//                    QMessageBox::information(hall,"提示","下播成功");
//                }else{
//                    QMessageBox::information(hall,"提示","该主播已下播");
//                }
                //room->backToHall();
                if(dataPack["isHost"].toString()=="true"){refresh_roomList();}
                room->close();
                hall->show();
                //QMessageBox::information(hall,"提示","下播成功");//???
                break;
            case Protocol::AudienceBackToHall:
                //room->backToHall();
                if(dataPack["isThisUser"].toString()=="false"){
                    room->refresh_audienceList(dataPack);
                }else{
                    room->close();
                    hall->show();
                }
                break;
            case Protocol::sendGift:
                if(dataPack["result"].toString()=="sendGiftFalse"){
                    QMessageBox::critical(room,"送礼","余额不足，送礼失败");
                }else{
                    if(dataPack["isHost"]=="true"){
                        room->host_giftRefresh(dataPack);
                    }else if(dataPack["isHost"]=="false"){
                        room->user_giftRefresh(dataPack);
                    }
                    room->giftShow(dataPack);
                }
                break;
            default:
                break;
        }
    }
}

//账户注册
void Client_w1::on_btnRgst_clicked()
{
    QString account = ui->leAccount->text();
    QString pswd = ui->lePswd->text();
    if(account.isEmpty() || pswd.isEmpty()){
        QMessageBox::critical(this,"账号注册","账号/密码为空");
        return;
    }

    Protocol dataPack(Protocol::rgst);  //?枚举在类外的应用
    dataPack["userAccount"] = account;
    dataPack["userPswd"] = pswd;

    tcpSocket->write(dataPack.pack());
}
//账户登录
void Client_w1::on_btnLogin_clicked()
{
    QString account = ui->leAccount->text();
    QString pswd = ui->lePswd->text();
    if(account.isEmpty() || pswd.isEmpty()){
        QMessageBox::critical(this,"账号登录","账号/密码为空");
        return;
    }

    Protocol dataPack(Protocol::login);  //?枚举在类外的应用
    dataPack["userAccount"] = account;   //dataPack["account"] = account; ×不能同名
    dataPack["userPswd"] = pswd;
    dataPack["online"] = 1;

    tcpSocket->write(dataPack.pack());
}
//账户退出
void Client_w1::exit_slot(){
     QString account = ui->leAccount->text();

     Protocol dataPack(Protocol::ClientExit);
     dataPack["userAccount"] = account;
     dataPack["online"] = 0;

     tcpSocket->write(dataPack.pack());
}
//账户注销
void Client_w1::delete_slot(){
    QString account = ui->leAccount->text();

    Protocol dataPack(Protocol::ClientDelete);
    dataPack["userAccount"] = account;

    tcpSocket->write(dataPack.pack());
}

//创建房间
void Client_w1::creatRoom_slot(QString _roomName){
    Protocol dataPack(Protocol::RoomCreate);
    dataPack["userAccount"] = ui->leAccount->text();
    dataPack["roomName"] =  _roomName;

    tcpSocket->write(dataPack.pack());
}
//退出直播
void Client_w1::backToHall_slot(QString _hostAccount,QString _userAccount){
    Protocol dataPack(Protocol::backToHall);
    dataPack["hostAccount"] = _hostAccount;
    dataPack["userAccount"] = _userAccount;

    tcpSocket->write(dataPack.pack());
}
//显示信息
void Client_w1::showUserInfo_slot(){
    Protocol dataPack(Protocol::showUserInfo);
    dataPack["userAccount"] = ui->leAccount->text();

    tcpSocket->write(dataPack.pack());
}
//账户充值
void Client_w1::chargeMoney_slot(double _money){
    Protocol dataPack(Protocol::chargeMoney);
    dataPack["userAccount"] = ui->leAccount->text();
    dataPack["money"] = _money;

    tcpSocket->write(dataPack.pack());
 }
//刷新房间列表
void Client_w1::refresh_roomList(){
    Protocol dataPack(Protocol::refreshRoomList);
    tcpSocket->write(dataPack.pack());
}
//刷新游客列表
void Client_w1::refresh_audienceList(){
    Protocol dataPack(Protocol::refreshAudienceList);
    tcpSocket->write(dataPack.pack());
}
//加入房间
void Client_w1::joinRoom_slot(QString room){
    Protocol dataPack(Protocol::JoinRoom);
    dataPack["roomName"] = room;
    dataPack["userAccount"] = ui->leAccount->text();

    tcpSocket->write(dataPack.pack());
}
//群聊
void Client_w1::sendMsg_slot(QString _msg,QString _roomName){
    Protocol dataPack(Protocol::sendMsg);
    _msg = QString("%1:%2").arg(ui->leAccount->text()).arg(_msg);
    dataPack["msg"] = _msg;
    dataPack["roomName"] = _roomName;
    tcpSocket->write(dataPack.pack());
}
//送礼物
void Client_w1::sendGift_slot(QString _hostName,int _value){
    Protocol dataPack(Protocol::sendGift);
    dataPack["hostName"] = _hostName;
    dataPack["userName"] = ui->leAccount->text();
    dataPack["value"] = _value;

    tcpSocket->write(dataPack.pack());
}



