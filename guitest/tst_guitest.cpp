#include <QString>
#include <QtTest>
#include <QApplication>
#include "../client/clientgui.h"

class Guitest : public QObject
{
    Q_OBJECT

public:
    Guitest();
    clientGUI* gui;

private Q_SLOTS:
    void init();
    void connectionsConnectButton();
    void connectionsQuitButton();
    void chatSendButton();
    void chatQuitButton();
    void writeStatus();
    void writeLongStatus();
    void simulateDisconnect();
    void simulateError();
    void cleanup();

private slots:
    void sendMsg(QString message);
    void status(QString status);
    void login(QString host, int port, QString user);

private:
    bool caught;
    QString message;
    QString st;
    QString host;
    int port;
    QString user;

signals:
    void connected();
    void newMessage(QString user, QString message);
    void listUpdate(QMap<QString,QString>& map);
    void sendError(QString error);
    void sendServerDisconnect();
};

Guitest::Guitest()
{
}

void Guitest::init(){
    gui = new clientGUI;

    QObject::connect(gui, SIGNAL(sendConnectInfo(QString,int,QString)), this, SLOT(login(QString,int,QString)));
    QObject::connect(this, SIGNAL(connected()), gui, SLOT(wasConnected()));
    QObject::connect(gui, SIGNAL(sendMsg(QString)), this, SLOT(sendMsg(QString)));
    QObject::connect(this, SIGNAL(newMessage(QString,QString)), gui, SLOT(gotMessage(QString,QString)));
    QObject::connect(gui, SIGNAL(updateStatus(QString)), this, SLOT(status(QString)));
    QObject::connect(this, SIGNAL(listUpdate(QMap<QString,QString>&)), gui, SLOT(updateList(QMap<QString,QString>&)));
    QObject::connect(this, SIGNAL(sendError(QString)), gui, SLOT(catchError(QString)));
    QObject::connect(this, SIGNAL(sendServerDisconnect()), gui, SLOT(onServerDisconnect()));

}

void Guitest::cleanup(){
    delete gui;
    //delete client;
}

void Guitest::connectionsConnectButton()
{
    QCOMPARE(gui->simulateConnectButtonPress(), false);
    gui->setHostText("host");
    QCOMPARE(gui->simulateConnectButtonPress(), false);
    gui->setPortNum(1);
    QCOMPARE(gui->simulateConnectButtonPress(), false);
    gui->setUserText("Dave");
    gui->simulateConnectButtonPress();
    emit connected(); //tells the gui that the connection to the server was made.
    QCOMPARE(gui->getIsConnectionsPage(), false);
    QCOMPARE(host, QString("host"));
    QCOMPARE(port, 1);
    QCOMPARE(user, QString("Dave"));
}

void Guitest::connectionsQuitButton(){
    QCOMPARE(gui->simulateQuitButtonPress(), true);
}

void Guitest::chatQuitButton(){
    connectionsConnectButton();
    QCOMPARE(gui->getIsConnectionsPage(), false);
    QCOMPARE(gui->simulateQuitButtonPress(), true);
    QCOMPARE(gui->getIsConnectionsPage(), true);
}

void Guitest::chatSendButton(){
    connectionsConnectButton();
    QString text = "Simulating text!";
    QCOMPARE(gui->simulateTypingInNewMessage(text), QString(text));
    QCOMPARE(gui->simulateSendButtonPress(),true);
    QCOMPARE(gui->getMessageWindowText().contains(text), QBool(true));
    QCOMPARE(gui->getNewMessageWindowText(), QString());
}

void Guitest::writeStatus(){
    connectionsConnectButton();
    QString text = "update";
    QCOMPARE(gui->simulateTypingInNewMessage("\\status " + text), QString("\\status " + text));
    QCOMPARE(gui->simulateSendButtonPress(),true);
    QCOMPARE(gui->getMessageWindowText().contains(text), QBool(false)); //shouldn't update message window
    QCOMPARE(gui->getNewMessageWindowText(), QString());
    QCOMPARE(gui->getUserWindowText().contains(text), QBool(true));
    QCOMPARE(gui->getUserWindowText().contains(user), QBool(true));
}

void Guitest::writeLongStatus(){
    connectionsConnectButton();
    QString text = "0123456789012345678901"; //longer than 20 characters
    QCOMPARE(gui->simulateTypingInNewMessage("\\status " + text), QString("\\status " + text));
    QCOMPARE(gui->simulateSendButtonPress(),true);
    QCOMPARE(gui->getMessageWindowText().contains(text), QBool(false)); //shouldn't update message window
    QCOMPARE(gui->getNewMessageWindowText(), QString());
    QCOMPARE(gui->getUserWindowText().contains(text), QBool(false));
    QCOMPARE(gui->getUserWindowText().contains(user), QBool(false));
    QCOMPARE(gui->getMessageWindowText().contains("The status message "
                                                  "was longer than 20 characters."), QBool(true));
}

void Guitest::simulateDisconnect(){
    connectionsConnectButton();
    QCOMPARE(gui->getIsConnectionsPage(), false);
    emit sendServerDisconnect();
    QCOMPARE(gui->getIsConnectionsPage(), true);
    QCOMPARE(gui->getStatusLabelText(), QString("You were disconnected from the server.  "
                                                "Insert credentials again to reconnect."));
}

void Guitest::simulateError(){
    QString error1 = "Testing error";
    emit sendError(error1);
    QCOMPARE(gui->getStatusLabelText(), error1);
    QCOMPARE(gui->getStatusLabelText().contains("anything else..."), QBool(false));
    connectionsConnectButton();
    QString error2 = "Error to be posted in the message window";
    emit sendError(error2);
    QCOMPARE(gui->getMessageWindowText().contains(error2), QBool(true));
    QCOMPARE(gui->getMessageWindowText().contains("anything else..."), QBool(false));
}

void Guitest::login(QString host, int port, QString user){
    this->host = host;
    this->port = port;
    this->user = user;
}

void Guitest::sendMsg(QString message){
    this->message = message;
    emit newMessage(user, message);
}

void Guitest::status(QString status){
    st = status;
    QMap<QString, QString> map;
    map.insert(user, st);
    emit listUpdate(map);
}

QTEST_MAIN(Guitest)

#include "tst_guitest.moc"
