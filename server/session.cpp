
#include "session.h"
#include <QThread>

session* session::createSession(int socketDescriptor, GateDB& db)
{
    session *sesh = new session(socketDescriptor, db);

    QThread *th = new QThread;
    sesh->moveToThread(th);

    QObject::connect(&sesh->sock, SIGNAL(disconnected()), sesh, SLOT(sockDisconnected()));
    QObject::connect(&sesh->sock, SIGNAL(error(QAbstractSocket::SocketError)),
                     sesh, SLOT(sockError(QAbstractSocket::SocketError)));
    QObject::connect(&sesh->sock, SIGNAL(readyRead()), sesh, SLOT(gotClientMessage()));

    QObject::connect(sesh, SIGNAL(destroyed()), th, SLOT(quit()));
    QObject::connect(th, SIGNAL(finished()), th, SLOT(deleteLater()));

    th->start();
    return sesh;
}

session::session(int socketDescriptor, GateDB& db)
            : QObject(0), sock(this), db(db)
{
    if (!sock.setSocketDescriptor(socketDescriptor)) {
        qDebug() << "Error setting sock descriptor!";
        Q_ASSERT(FALSE);
    }

    qDebug() << "Made session on port " << sock.localPort() << " to port " << sock.peerPort();
}

QTcpSocket *session::getSocket()
{
    return &this->sock;
}

void session::sessionShutdown()
{
    this->deleteLater();
}

void session::sockDisconnected()
{
    qDebug() << "Socket disconnected!";
    sessionShutdown();
}

void session::sockError(QAbstractSocket::SocketError socketError)
{
    qDebug() << "Socket error:" << socketError;
    sessionShutdown();
}

session::~session()
{
    qDebug() << "Session deleted";
}

void session::gotClientMessage()
{
    qDebug() << "Got client message";
    char message[1024];

    int lenRead;
    do {
        lenRead = sock.readLine(message, sizeof message);
    } while (lenRead == 0);
    if (lenRead == -1) {
        // An error occurred!
        qDebug() << "An error occurred on the read of the socket. Terminating connection.";
        sock.close();
    }

    // Take off the newline characters
    lenRead--;
    while (message[lenRead] == '\n' || message[lenRead] == '\r') {
        message[lenRead--] = 0;
    }

    // Turn it into a QString for comparison convenience
    QString msg(message);
    qDebug() << "Got message: " << msg;

    QString ret = executeCommand(msg);
    sendResponse(ret);
}

QString session::executeCommand(QString cmd)
{
    QStringList line = cmd.split(" ", QString::SkipEmptyParts);
    if (line.count() == 0)
        return "No command found";

    QString command = line[0];
    if (command.compare(QString("create"), Qt::CaseInsensitive) == 0)
        return doCreate(line);
    else if (command.compare(QString("connect"), Qt::CaseInsensitive) == 0)
        return doConnect(line);
    else if (command.compare(QString("destroy"), Qt::CaseInsensitive) == 0)
        return doDestroy(line);
    else if (command.compare(QString("gateoutput"), Qt::CaseInsensitive) == 0)
        return doGateOutput(line);

    if (command.compare(QString("dumpgates"), Qt::CaseInsensitive) == 0) {
        db.dumpGates();
        return "Gates dumped to server's console";
    } else if (command.compare(QString("dumpconnections"), Qt::CaseInsensitive) == 0) {
        db.dumpConnections();
        return "Connections dumped to server's console";
    }

    // Unknown command!
    return "Unknown command";
}

QString session::doCreate(QStringList &line)
{
    if (line.count() != 2)
        return "Malformed";
    
    QString name = line[1];

    return db.create(line[1]);
}

QString session::doGateOutput(QStringList &line)
{
    if (line.count() != 2)
        return "Malformed";

    return db.gateOutput(line[1]);
}

QString session::doConnect(QStringList &line)
{
    if (line.count() != 3)
        return "Malformed";

    return db.connect(line[1], line[2]);
}

QString session::doDestroy(QStringList &line)
{
    if (line.count() != 2)
        return "Malformed";

    return db.destroy(line[1]);
}

void session::sendResponse(QString msg)
{
    QByteArray toWrite = (msg + "\n").toLocal8Bit();

    int retcode = sock.write(toWrite);
    if (retcode != toWrite.length()) {
        qDebug() << "Could not respond to the client!";
    }
}
