#include "client.h"
#include <QString>
#include <QStringList>

Client::Client(QObject *parent)
{
     tcpSocket = new QTcpSocket(this);

     connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readResults()));
     connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(catchError(QAbstractSocket::SocketError)));
     connect(tcpSocket,SIGNAL(disconnected()), this, SLOT(serverDisconnected()));
}

void Client::sendMsg(QString message)
{
    QString command = "msg " + message;
    sendCommand(command);
}

void Client::status(QString status)
{
    QString command = "status " + status;
    sendCommand(command);
}

void Client::login(QString serverName, int port, QString name)
{
    if(!tcpSocket->isOpen()){
        tcpSocket->abort();
        tcpSocket->connectToHost(serverName,port);
        Q_ASSERT(tcpSocket->waitForConnected(5000));
    }

    QString command = "Login " + name;
    sendCommand(command);

    emit connected();
}

void Client::quitFromServer()
{
    tcpSocket->disconnect();
}

void Client::sendCommand(QString msg)
{
    QByteArray block;

    block.append(msg);
    block.append("\n");

    tcpSocket->write(block);
    tcpSocket->waitForBytesWritten();
}

void Client::readResults()
{
        char message[1024];

        int lenRead;
        do {
            lenRead = tcpSocket->readLine(message, sizeof message);
        } while (lenRead == 0);
        if (lenRead == -1) {
            // An error occurred!
            qDebug() << "An error occurred on the read of the socket. Terminating connection.";
            tcpSocket->close();
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

        if (space != -1 && command.compare("error", Qt::CaseInsensitive) == 0) {
            emit sendError(params);
        }
        else if(space != -1 && command.compare("msg", Qt::CaseInsensitive) == 0)
        {
            int paramBreak = msg.indexOf(QChar('\0'));
            QString from;
            QString message;
            from = msg.left(space);
            message = msg.mid(space + 1);

            emit newMessage(from,message);
        }
        else if(space != -1 && command.compare("list", Qt::CaseInsensitive) == 0)
        {
            QStringList userList;
            userList = params.split(QChar('\0'));
            QMap<QString,QString> users;

            for(int i; i < userList.size(); i++)
            {
                QString user;
                QString status;
                user = userList[i];
                i++;
                if(i < userList.size())
                {
                    status = userList[i];
                }
                users.insert(user,status);
            }

            emit listUpdate(users);
        }
        else
        {
            QByteArray buf;
            buf.append("ERROR Unknown command: ");
            buf.append(command);
            buf.append('\n');
            qDebug() << buf;
        }
}

void Client::serverDisconnected()
{
    tcpSocket->close();
    emit sendServerDisconnect();
}

void Client::catchError(QAbstractSocket::SocketError socketError)
{
    int error = socketError;
    QString errorMessage = QString("ERROR %1 has occured. Please reference the documentation for specifics.").arg(error);
    emit sendError(errorMessage);
}
