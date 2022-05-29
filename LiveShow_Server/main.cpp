#include "Server_w1.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Server_w1 w;
    w.show();

    return a.exec();
}
