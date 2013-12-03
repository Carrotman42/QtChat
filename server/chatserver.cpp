
#include "chatserver.h"
#include "session.h"
#include <QThread>

QByteArray makeListResponse(QMap<int, NameAndStatus> users)
{
    QByteArray ret;

    if (users.size() == 0)
        return ret;

    ret.append("LIST ");
    QMap<int, NameAndStatus>::iterator it = users.begin();
    while (true) {
        ret.append(it.value().name);
        ret.append('\0');
        ret.append(it.value().status);

        it++;
        if (it == users.end()) {
            break;
        }
        ret.append('\0');
    }

    qDebug() << "Made list response size=" << ret.length() << ": " << ret;
    ret.append('\n');

    return ret;
}

chatserver::chatserver() : QTcpServer(0)
{
    qDebug() << "Server instantiated";
}

void chatserver::incomingConnection(int socketDescriptor)
{
    qDebug() << "Got connection";
    session *sesh = new session(socketDescriptor, nextid);
    nextid++;

    QObject::connect(sesh, SIGNAL(recvLogin(int, QString) ), this, SLOT(userLogin(int, QString) ));
    QObject::connect(sesh, SIGNAL(recvMsg(int, QString)   ), this, SLOT(userMsg(int, QString)   ));
    QObject::connect(sesh, SIGNAL(recvStatus(int, QString)), this, SLOT(userStatus(int, QString)));
    QObject::connect(sesh, SIGNAL(clientDisconnected(int) ), this, SLOT(userLogout(int)         ));

    QObject::connect(this, SIGNAL(broadcastResponse(QByteArray)), sesh, SLOT(sendResponse(QByteArray)));
}

// So that I don't have to add this private method to the class's header and still don't have to
//   copy-paste this line three times
#define bcastList() emit broadcastResponse(makeListResponse(statuses))

void chatserver::userLogin(int desc, QString name)
{
    qDebug() << "Got login: " << name;
    NameAndStatus ns;
    ns.name = name;
    ns.status = "Just entered!";
    statuses[desc] = ns;

    bcastList();
}

void chatserver::userMsg(int desc, QString msg)
{
    QByteArray toSend;
    toSend.append("MSG ");
    toSend.append(statuses[desc].name);
    toSend.append('\0');
    toSend.append(msg);
    toSend.append('\n');

    emit broadcastResponse(toSend);
}

void chatserver::userStatus(int desc, QString status)
{
    statuses[desc].status = status;
    bcastList();
}

void chatserver::userLogout(int desc)
{
    statuses.remove(desc);
    bcastList();
}



