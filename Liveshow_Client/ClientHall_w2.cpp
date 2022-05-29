#include "ClientHall_w2.h"
#include "ui_ClientHall_w2.h"
#include"QMessageBox"
#include"QDebug"

ClientHall_w2* ClientHall_w2::instance = 0;

ClientHall_w2* ClientHall_w2::getInstance(){
    if(instance==0){
        instance = new ClientHall_w2;
    }
    return instance;
}
ClientHall_w2::ClientHall_w2(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClientHall_w2)
{
    ui->setupUi(this);

    ui->le_roomName->setFocus();
    ui->le_roomName->installEventFilter(this);

    //void itemDoubleClicked(QListWidgetItem *item)
    connect(ui->listWidget,SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this,SLOT(itemDoubleClicked_slot(QListWidgetItem*)));
}
ClientHall_w2::~ClientHall_w2()
{
    delete ui;
}

//过滤事件
bool ClientHall_w2::eventFilter(QObject *watched, QEvent *event){
    if(watched==ui->le_roomName){
        if(event->type()==QEvent::KeyPress){
            QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(event);
            if(keyEvent->key()==Qt::Key_Return || keyEvent->key()==Qt::Key_Enter){
                on_btn_createRoom_clicked();
                return true;
            }
        }
    }
    return false;
}
//创建房间
void ClientHall_w2::on_btn_createRoom_clicked()
{
    QString roomName = ui->le_roomName->text();
    if(roomName.isEmpty()){
        QMessageBox::critical(this,"创建房间","房间名为空");
        return;
    }
    emit createRoom_signal(roomName);
}
//账户退出
void ClientHall_w2::on_btn_exit_clicked()
{
    emit exit_signal();
}
//账户注销
void ClientHall_w2::on_btn_deleteAccount_clicked()
{
    emit delete_signal();
}
//刷新列表
void ClientHall_w2::refresh_RoomList(Protocol& dataPack){
    ui->listWidget->clear();
    //ui->listWidget->addItem("正在直播...");
    int count = 0;
    while(dataPack[QString::number(count)].isNull() == false){
        QString roomName = dataPack[QString::number(count)].toString();
        ui->listWidget->insertItem(count,roomName);
        count++;
    }
}
//显示账户
void ClientHall_w2::on_btn_userInfo_clicked()
{
    emit showUserInfo_signal();
}
void ClientHall_w2::showUserInfo(Protocol& dataPack){
    QString _account = dataPack["userAccount"].toString();
    double _money = dataPack["money"].toDouble();
    int _score = dataPack["score"].toInt();
    ui->le_account->setText(_account);
    ui->le_money->setText(QString::number(_money,'f',2));
    ui->le_score->setText(QString::number(_score,10));
}
//账户充值
void ClientHall_w2::on_btn_chargeMoney_clicked()
{
    QString _money = ui->le_chargeMoney->text();
    double _money2 = _money.toDouble();
    //qDebug() << ("%.2lf",_money2);
    if(_money2<=0){         //double类型的比较!==
        QMessageBox::critical(this,"充值","充值金额为零/负值,充值失败");
        return;
    }

   emit chargeMoney_signal(_money2);
}
//加入房间
void ClientHall_w2::itemDoubleClicked_slot(QListWidgetItem* item){
    emit joinRoom_signal(item->text());
}



