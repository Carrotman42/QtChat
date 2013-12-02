
#include "session.h"
#include <QThread>

session::session(int socketDescriptor, int desc)
            : QObject(0), sock(this), desc(desc), loggedIn(false)
{
    if (!sock.setSocketDescriptor(socketDescriptor)) {
        qDebug() << "Error setting sock descriptor!";
        Q_ASSERT(FALSE);
    }

    qDebug() << "Made session on port " << sock.localPort() << " to port " << sock.peerPort();
    
    QObject::connect(&sock, SIGNAL(disconnected()), this, SLOT(sockDisconnected()));
    QObject::connect(&sock, SIGNAL(readyRead()), this, SLOT(gotClientMessage()));
    QObject::connect(&sock, SIGNAL(error(QAbstractSocket::SocketError)),
                     this, SLOT(sockError(QAbstractSocket::SocketError)));
}

void session::sockDisconnected()
{
    qDebug() << "Socket disconnected!";
    emit clientDisconnected(desc);
    deleteLater();
}

void session::sockError(QAbstractSocket::SocketError socketError)
{
    qDebug() << "Socket error:" << socketError;
    emit clientDisconnected(desc);
    deleteLater();
}

session::~session()
{
    qDebug() << "Session deleted";
}

void session::gotClientMessage()
{
    qDebug() << "Got client me";
    char message[1024];

    int lenRead;
    do {
        lenRead = sock.readLine(message, sizeof message);
    } while (lenRead == 0);
    if (lenRead == -1) {
        // An error occurred!
        qDebug() << "An error occurred on the read of the socket. Terminating connection.";
        sock.close();
        return;
    }

    // Take off the newline characters
    lenRead--;
    while (message[lenRead] == '\n' || message[lenRead] == '\r') {
        message[lenRead--] = 0;
    }

    // Turn it into a QString for comparison convenience
    QString msg(message);
    qDebug() << "Got message: " << msg;
    
    int space = msg.indexOf(' ');
    QString command;
    QString params;
    if (space == -1) {
        command = msg;
    } else {
        command = msg.left(space);
        params = msg.mid(space + 1);
    }

    if (space != -1 && command.compare("login", Qt::CaseInsensitive) == 0) {
        qDebug() << "Login: " << command;
        loggedIn = true;
        emit recvLogin(desc, params);
    } else if (space != -1 && command.compare("msg", Qt::CaseInsensitive) == 0) {
        emit recvMsg(desc, params);
    } else if (space != -1 && command.compare(QString("status"), Qt::CaseInsensitive) == 0) {
        emit recvStatus(desc, params);
    } else {
        QByteArray buf;
        buf.append("ERROR Unknown command: ");
        buf.append(command);
        buf.append('\n');
        qDebug() << buf;
        sock.write(buf);
    }
}

void session::sendResponse(QByteArray response)
{
    if (loggedIn) {
        int retcode = sock.write(response);
        if (retcode != response.length()) {
            qDebug() << "Could not respond to the client!";
        }
    }
}
