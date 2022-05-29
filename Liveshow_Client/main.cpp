#include "Client_w1.h"
#include <QApplication>

/*
TCP通信:  //tcp没有readDatagram(),writeDatagram()
    1.tcpSocket->read(),tcpSocket->write()               //char*传输
    2.json数据包 tcpSocket->readAll(),tcpSocket.wirte()   //QByteArray传输

UDP通信:  //udp没有write(),read()
    udpSocket.readDatagram(data.data(),len,&host,&port); //char*传输 QByteArray data;
    m_udpSocket.writeDatagram(data.toStdString().c_str(),//QString data
                              data.size(),
                              QHostAddress(ip),
                              port.toInt()
                              );
*/

/*
服务器与客户端之间的通信:
1.数据较少：         不用数据包
2.数据较多：         可以用定长包
3.数据多，变化灵活：   用变长包---(1)吴老师讲的变长包(2)jason包
*/

/*
tcp套接字通信原理(双向的):
服务端套接字 client (192.168.191.1,8990)--本身  (192.168.191.2,51781)--伙伴
客户端套接字 client (192.168.191.2,51781)--本身 (192.168.191.1,8990)--伙伴
服务端写 自动写到伙伴里客户端从本身里读
客户端写 自动写到伙伴里服务端从本身里读

udp套接字通信原理(单向的):
主持人 没有绑定
观众   udpSocket绑定如(192.168.191.1,8990)
主持人写(广播) 直接写到udpSocket里，观众直接从udpSocket里读
观众写(不常用) 要现将主持人的ip,port存下，再往主持人里写(类似于tcp的双向套接字)
*/
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Client_w1 w;
    w.show();

    return a.exec();
}
