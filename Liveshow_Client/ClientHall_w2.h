#ifndef CLIENTHALL_W2_H
#define CLIENTHALL_W2_H

#include <QWidget>
#include"protocol.h"
#include"QListWidget"
#include"QEvent"
#include"QKeyEvent"

namespace Ui {
class ClientHall_w2;
}

class ClientHall_w2 : public QWidget
{
    Q_OBJECT

public:
    static ClientHall_w2* getInstance();
    explicit ClientHall_w2(QWidget *parent = 0);    //???单例构造析构函数的处理
    ~ClientHall_w2();

    void refresh_RoomList(Protocol& dataPack);
    void showUserInfo(Protocol& dataPack);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

signals:
    void createRoom_signal(QString roomName);
    void exit_signal();
    void delete_signal();
    void showUserInfo_signal();
    void chargeMoney_signal(double _money2);
    void joinRoom_signal(QString room);
private slots:
    void on_btn_createRoom_clicked();
    void on_btn_exit_clicked();
    void on_btn_deleteAccount_clicked();
    void on_btn_userInfo_clicked();
    void on_btn_chargeMoney_clicked();

    void itemDoubleClicked_slot(QListWidgetItem*);

private:
    Ui::ClientHall_w2 *ui;
    static ClientHall_w2* instance;
};

#endif // CLIENTHALL_W2_H
