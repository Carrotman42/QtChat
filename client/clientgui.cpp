#include "clientgui.h"


clientGUI::clientGUI(QWidget *parent) :
    QDialog(parent)
{
    buildConnectPage();
}


void clientGUI::enableConnectButton(){
    connectButton->setEnabled((  !hostLineEdit->text().isEmpty() &&
                                 !portLineEdit->text().isEmpty() &&
                                 !userNameLineEdit->text().isEmpty()));
}

void clientGUI::enableSendButton(){
    sendButton->setEnabled(!newMessageWindow->toPlainText().isEmpty());
}


void clientGUI::buildConnectPage(){
    isConnectionsPage = true;

    if(layout())
        delete layout();
    hostLabel = new QLabel(tr("&Server name:"));
    portLabel = new QLabel(tr("S&erver port:"));
    userNameLabel = new QLabel(tr("&User Name: "));

    hostLineEdit = new QLineEdit;
    portLineEdit = new QLineEdit;
    portLineEdit->setValidator(new QIntValidator(1, 65535, this));
    userNameLineEdit = new QLineEdit;

    hostLabel->setBuddy(hostLineEdit);
    portLabel->setBuddy(portLineEdit);
    userNameLabel->setBuddy(userNameLineEdit);

    statusLabel = new QLabel(tr("Input the host name, port number, "
                                "and your desired user name."));

    connectButton = new QPushButton(tr("Log in"));
    connectButton->setDefault(true);
    connectButton->setEnabled(false);

    quitButton = new QPushButton(tr("Quit"));

    buttonBox = new QDialogButtonBox;
    buttonBox->addButton(connectButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

    connect(hostLineEdit, SIGNAL(textChanged(QString)),
            this, SLOT(enableConnectButton()));
    connect(portLineEdit, SIGNAL(textChanged(QString)),
            this, SLOT(enableConnectButton()));
    connect(userNameLineEdit, SIGNAL(textChanged(QString)),
            this, SLOT(enableConnectButton()));
    connect(connectButton, SIGNAL(clicked()),
            this, SLOT(connectButtonPushed()));
    connect(quitButton, SIGNAL(clicked()), this, SLOT(quitButtonPushed()));
    connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));

    QGridLayout *connectLayout = new QGridLayout;
    connectLayout->addWidget(hostLabel, 0, 0);
    connectLayout->addWidget(hostLineEdit, 0, 1);
    connectLayout->addWidget(portLabel, 1, 0);
    connectLayout->addWidget(portLineEdit, 1, 1);
    connectLayout->addWidget(userNameLabel, 2, 0);
    connectLayout->addWidget(userNameLineEdit, 2, 1);
    connectLayout->addWidget(statusLabel, 3, 0, 1, 2);
    connectLayout->addWidget(buttonBox, 4, 0, 1, 2);
    connectLayout->setAlignment(Qt::AlignCenter);
    setLayout(connectLayout);

    setMinimumWidth(400);   //Makes sure that the window will always look at least wide
                            //enough to input credentials
    adjustSize();

    setWindowTitle(tr("Chat Client Connection"));
    hostLineEdit->setFocus();

    connectButton->setEnabled(false);
}

void clientGUI::buildChatPage(){
    isConnectionsPage = false;

    messageWindow = new QTextEdit;
    messageWindow->setReadOnly(true);
    messageWindow->setMinimumSize(500,300);

    userWindow = new QTextEdit;
    userWindow->setReadOnly(true);
    userWindow->setMinimumSize(150,300);

    QHBoxLayout* topLayout = new QHBoxLayout;
    topLayout->addWidget(messageWindow,4);
    topLayout->addWidget(userWindow, 1);


    newMessageWindow = new QTextEdit;
    newMessageWindow->setFixedHeight(50);
    newMessageWindow->setMinimumWidth(660);
    newMessageWindow->setTabChangesFocus(true);

    sendButton = new QPushButton(tr("Send Message"));
    sendButton->setDefault(true);
    sendButton->setEnabled(false);

    quitButton = new QPushButton(tr("Quit"));

    buttonBox = new QDialogButtonBox;
    buttonBox->addButton(sendButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

    connect(newMessageWindow, SIGNAL(textChanged()),
            this, SLOT(enableSendButton()));
    connect(sendButton, SIGNAL(clicked()),
            this, SLOT(sendButtonPushed()));
    connect(quitButton, SIGNAL(clicked()), this, SLOT(quitButtonPushed()));


    QGridLayout *chatLayout = new QGridLayout;
    chatLayout->addLayout(topLayout,0,0,5,5);
    chatLayout->addWidget(newMessageWindow, 5,0);
    chatLayout->addWidget(buttonBox, 6,0);
    setLayout(chatLayout);
    setMinimumWidth(700);

    messageWindow->setFocusPolicy(Qt::ClickFocus);
    userWindow->setFocusPolicy(Qt::ClickFocus);

    adjustSize();
    setWindowTitle("Chat Room");
}

void clientGUI::tearDownChatPage(){
    delete messageWindow;
    delete userWindow;
    delete newMessageWindow;
    delete sendButton;
    delete quitButton;
    delete buttonBox;
    delete layout();
}

void clientGUI::tearDownConnectPage(){
    delete hostLabel;
    delete portLabel;
    delete userNameLabel;
    delete hostLineEdit;
    delete portLineEdit;
    delete userNameLineEdit;
    delete statusLabel;
    delete quitButton;
    delete connectButton;
    delete buttonBox;
    delete layout();
}


void clientGUI::connectButtonPushed(){
    user = userNameLineEdit->text();
    emit sendConnectInfo(hostLineEdit->text(),
                         portLineEdit->text().toInt(),
                         userNameLineEdit->text());
}

void clientGUI::quitButtonPushed(){
    if(!isConnectionsPage){
        tearDownChatPage();
        buildConnectPage();
    }
}

void clientGUI::sendButtonPushed(){
    if(!isConnectionsPage){
        QString msg = newMessageWindow->toPlainText();
        if(msg.startsWith("\\status ")){
            msg.replace("\\status ","");
            if(msg.size()>20){
                postError("The status message "
                          "was longer than 20 characters.");
            }
            else
                emit updateStatus(msg);
        }
        else
            emit sendMsg(msg);
        newMessageWindow->clear();
    }
}

void clientGUI::wasConnected(){
    tearDownConnectPage();
    buildChatPage();
}

void clientGUI::gotMessage(QString user, QString msg){
    postMessageToWindow(messageWindow, user, msg);
}

void clientGUI::updateList(QMap<QString, QString>& map){
    userWindow->clear();
    QMap<QString, QString>::iterator i;
    for(i = map.begin(); i != map.end(); ++i){
        postMessageToWindow(userWindow, i.key(), i.value());
    }
}

void clientGUI::onServerDisconnect(){
    tearDownChatPage();
    buildConnectPage();
    postError("You were disconnected from the server.  "
              "Insert credentials again to reconnect.");
}

void clientGUI::catchError(QString error){
    postError(error);
}

void clientGUI::postMessageToWindow(QTextEdit *window, QString user, QString message){
    QString color;
    if(user == this->user)
        color = "red";
    else
        color = "blue";
    window->append(QString("<font color=%1> %2</font>: <font color=black> %3</font>")
                           .arg(color).arg(user).arg(message));
}

void clientGUI::postError(QString errorMessage){
    if(isConnectionsPage)
        statusLabel->setText(errorMessage);
    else
        messageWindow->append(QString("<font color=grey>%1</font>").arg(errorMessage));
}


#ifdef testing
bool clientGUI::simulateConnectButtonPress(){
    if(!hostLineEdit->text().isEmpty() &&
        !portLineEdit->text().isEmpty() &&
        !userNameLineEdit->text().isEmpty()){
        connectButtonPushed(); //exactly what happens with the push button
        return !isConnectionsPage;
    }
    return false;
}

bool clientGUI::simulateQuitButtonPress(){
    quitButtonPushed();
    return isConnectionsPage;
}

bool clientGUI::simulateSendButtonPress(){
    sendButtonPushed();
    return !isConnectionsPage;
}
QString clientGUI::setHostText(QString text){
    hostLineEdit->setText(text);
    return hostLineEdit->text();
}

QString clientGUI::setPortNum(int num){
    portLineEdit->setText(QString("%1").arg(num));
    return portLineEdit->text();
}

QString clientGUI::setUserText(QString text){
    userNameLineEdit->setText(text);
    return userNameLineEdit->text();
}

QString clientGUI::simulateTypingInNewMessage(QString message){
    newMessageWindow->setText(message);
    return newMessageWindow->toPlainText();
}

QString clientGUI::getMessageWindowText(){
    return messageWindow->toPlainText();
}

QString clientGUI::getNewMessageWindowText(){
    return newMessageWindow->toPlainText();
}

QString clientGUI::getUserWindowText(){
    return userWindow->toPlainText();
}

bool clientGUI::getIsConnectionsPage(){
    return isConnectionsPage;
}

QString clientGUI::getStatusLabelText(){
    return statusLabel->text();
}

#endif
