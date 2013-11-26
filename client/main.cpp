#include <QApplication>
#include "client.h"
#include "clientgui.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Client* client = new Client;
    clientGUI* gui = new clientGUI;

    QObject::connect(gui, SIGNAL(sendConnectInfo(QString,int,QString)), client, SLOT(login(QString,int,QString)));
    QObject::connect(client, SIGNAL(connected()), gui, SLOT(wasConnected()));
    QObject::connect(gui, SIGNAL(sendMsg(QString)), client, SLOT(sendMsg(QString)));
    QObject::connect(client, SIGNAL(newMessage(QString,QString)), gui, SLOT(gotMessage(QString,QString)));
    QObject::connect(gui, SIGNAL(updateStatus(QString)), client, SLOT(status(QString)));
    QObject::connect(client, SIGNAL(listUpdate(QMap<QString,QString>&)), gui, SLOT(updateList(QMap<QString,QString>&)));
    QObject::connect(client, SIGNAL(sendError(QString)), gui, SLOT(catchError(QString)));
    QObject::connect(client, SIGNAL(sendServerDisconnect()), gui, SLOT(onServerDisconnect()));

    gui->show();
    return gui->exec();
}
