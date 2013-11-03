#include "threadedtcpserver.h"
#include "threadedtcpsocket.h"

ThreadedTCPServer::ThreadedTCPServer(QObject* parent): QTcpServer(parent)
{
}

void ThreadedTCPServer::incomingConnection(int socketDescriptor){
    ThreadedTCPSocket *socket = new ThreadedTCPSocket;

    if (!socket->setSocketDescriptor(socketDescriptor)) {
        delete socket;
        return;
    }
    QThread *thread = new QThread;
    connect(thread, SIGNAL(started()), socket, SLOT(handleStart()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(thread, SIGNAL(destroyed()), this, SLOT(handleDestroyedThread()));
    connect(thread, SIGNAL(finished()), socket, SLOT(deleteLater()));
    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteAllGates())); // quits the thread after deletion
    connect(socket, SIGNAL(destroyed()), this, SLOT(handleDestroyedSocket()));

    connect(this, SIGNAL(serverDisconnect()), socket, SLOT(disconnectSocket()));

    socket->moveToThread(thread);
    thread->start();
}


void ThreadedTCPServer::handleDestroyedThread(){
    qDebug() << "A thread was destroyed.";
}

void ThreadedTCPServer::handleDestroyedSocket(){
    qDebug() << "A socket was destroyed.";
}

void ThreadedTCPServer::broadcastDisconnected(){
    emit serverDisconnect();
}
