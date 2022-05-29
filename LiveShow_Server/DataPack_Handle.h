#ifndef DATAPACK_HANDLE_H
#define DATAPACK_HANDLE_H

#include"QTcpSocket"
#include"protocol.h"

class DataPack_Handle
{
public:
    DataPack_Handle();    //const?
    void strategy(QTcpSocket* tcpSocket,Protocol& dataPack);
};

#endif // DATAPACK_HANDLE_H
