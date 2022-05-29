#ifndef CLIENT_W1_H
#define CLIENT_W1_H

#include <QWidget>
#include"QTcpSocket"
#include"ClientHall_w2.h"
#include"ClientRoom_w3.h"
#include"QKeyEvent"
#include"QEvent"

namespace Ui {
class Client_w1;
}

class Client_w1 : public QWidget
{
    Q_OBJECT

public:
    explicit Client_w1(QWidget *parent = 0);
    ~Client_w1();

    void refresh_roomList();
    void refresh_audienceList();
    //void UiDesign();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private slots:
    void readyRead_slot();
    void on_btnRgst_clicked();
    void on_btnLogin_clicked();

    void exit_slot();
    void delete_slot();
    void creatRoom_slot(QString);
    void backToHall_slot(QString,QString);
    void showUserInfo_slot();
    void chargeMoney_slot(double);
    void joinRoom_slot(QString);
    void sendMsg_slot(QString,QString);
    void sendGift_slot(QString,int);

private:
    Ui::Client_w1 *ui;
    QTcpSocket* tcpSocket;
    ClientHall_w2* hall;
    ClientRoom_w3* room;
};

#endif // CLIENT_W1_H
