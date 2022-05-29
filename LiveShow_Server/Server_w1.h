#ifndef SERVER_W1_H
#define SERVER_W1_H

#include <QWidget>
#include"QTcpServer"

namespace Ui {
class Server_w1;
}

class Server_w1 : public QWidget
{
    Q_OBJECT

public:
    explicit Server_w1(QWidget *parent = 0);
    ~Server_w1();

private slots:
    void on_btnListen_clicked();

    void newConnection_slot();

private:
    Ui::Server_w1 *ui;
    QTcpServer* tcpServer;
};

#endif // SERVER_W1_H
