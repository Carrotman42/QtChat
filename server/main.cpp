

#include <QApplication>
#include <QtCore>

#include <stdlib.h>

#include "server.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Server server;
    server.show();
    return server.exec();
}
