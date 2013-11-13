
#include "gateserver.h"
#include "session.h"
#include <QThread>

gateserver::gateserver() : QTcpServer(0)
{
    qDebug() << "Server instantiated";
}

void gateserver::incomingConnection(int socketDescriptor)
{
    qDebug() << "Got connection";
    session *sesh = session::createSession(socketDescriptor, db);
    if (!sesh)
        return;

    QObject::connect(sesh->thread(), SIGNAL(destroyed()), this, SLOT(childThreadDestroyed()));
    QObject::connect(sesh->getSocket(), SIGNAL(destroyed()), this, SLOT(childSocketDestroyed()));
}
