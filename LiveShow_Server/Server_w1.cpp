#include "Server_w1.h"
#include "ui_Server_w1.h"
#include"QTcpSocket"
#include"SockContainer.h"
#include"SockHandle.h"

Server_w1::Server_w1(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Server_w1)
{
    ui->setupUi(this);

    tcpServer = new QTcpServer(this);
    connect(tcpServer,SIGNAL(newConnection()),this,SLOT(newConnection_slot()));
}
Server_w1::~Server_w1()
{
    delete ui;
}

//建立链接 断开链接!==
void Server_w1::on_btnListen_clicked()
{
    if(tcpServer->isListening()){
        ui->textBrowser->append("已在监听");
    }else{
        int retval = tcpServer->listen(QHostAddress("127.0.0.1"),8991);
        if(retval){
            ui->textBrowser->append("监听成功");
        }else{
            ui->textBrowser->append("监听失败!");
        }
    }
}
//当有客户端链接时，拿到套接字
void Server_w1::newConnection_slot(){
    QTcpSocket* tcpSocket = tcpServer->nextPendingConnection();

    QString ip = tcpSocket->peerAddress().toString();
    QString port = QString::number(tcpSocket->peerPort());
    QString text = QString("客户端ip:%1,port:%2 已连接").arg(ip).arg(port);
    ui->textBrowser->append(text);

    SockContainer* sockContainer = SockContainer::getInstance();    //将每个链接至服务器的套接字保存到容器
    sockContainer->insertInto_SockContainer(tcpSocket); //???断开时不用删吗

    new SockHandle(this,tcpSocket); //将通信工作交给SockHandle类生成对象的来处理 ???可以直接new吗
}


