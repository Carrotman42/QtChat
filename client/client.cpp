
 #include <QtGui>
 #include <QtNetwork>

 #include "client.h"

 Client::Client(QWidget *parent)
 :   QDialog(parent), networkSession(0)
 {
     hostLabel = new QLabel(tr("&Server name:"));
     portLabel = new QLabel(tr("S&erver port:"));
     commandLabel = new QLabel(tr("&Command: "));

     // find out which IP to connect to
     QString ipAddress;
     QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
     // use the first non-localhost IPv4 address
     for (int i = 0; i < ipAddressesList.size(); ++i) {
         if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
             ipAddressesList.at(i).toIPv4Address()) {
             ipAddress = ipAddressesList.at(i).toString();
             break;
         }
     }
     // if we did not find one, use IPv4 localhost
     if (ipAddress.isEmpty())
         ipAddress = QHostAddress(QHostAddress::LocalHost).toString();

     hostLineEdit = new QLineEdit(ipAddress);
     portLineEdit = new QLineEdit;
     portLineEdit->setValidator(new QIntValidator(1, 65535, this));
     commandLineEdit = new QLineEdit;

     hostLabel->setBuddy(hostLineEdit);
     portLabel->setBuddy(portLineEdit);
     commandLabel->setBuddy(commandLineEdit);

     statusLabel = new QLabel(tr("This requires that you run the "
                                 "Gate Server example as well."));

     sendCmdButton = new QPushButton(tr("Send Command"));
     sendCmdButton->setDefault(true);
     sendCmdButton->setEnabled(false);

     quitButton = new QPushButton(tr("Quit"));

     buttonBox = new QDialogButtonBox;
     buttonBox->addButton(sendCmdButton, QDialogButtonBox::ActionRole);
     buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

     tcpSocket = new QTcpSocket(this);

     connect(hostLineEdit, SIGNAL(textChanged(QString)),
             this, SLOT(enableSendCmdButton()));
     connect(portLineEdit, SIGNAL(textChanged(QString)),
             this, SLOT(enableSendCmdButton()));
     connect(commandLineEdit, SIGNAL(textChanged(QString)),
             this, SLOT(enableSendCmdButton()));
     connect(sendCmdButton, SIGNAL(clicked()),
             this, SLOT(sendCmd()));
     connect(quitButton, SIGNAL(clicked()), this, SLOT(quitFromServer()));
     connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));
     connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readResults()));
     connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
             this, SLOT(displayError(QAbstractSocket::SocketError)));
     connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(serverDisconnected()));

     QGridLayout *mainLayout = new QGridLayout;
     mainLayout->addWidget(hostLabel, 0, 0);
     mainLayout->addWidget(hostLineEdit, 0, 1);
     mainLayout->addWidget(portLabel, 1, 0);
     mainLayout->addWidget(portLineEdit, 1, 1);
     mainLayout->addWidget(commandLabel, 2, 0);
     mainLayout->addWidget(commandLineEdit, 2, 1);
     mainLayout->addWidget(statusLabel, 3, 0, 1, 2);
     mainLayout->addWidget(buttonBox, 4, 0, 1, 2);
     setLayout(mainLayout);

     setWindowTitle(tr("Gate Client"));
     portLineEdit->setFocus();

     QNetworkConfigurationManager manager;
     if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
         // Get saved network configuration
         QSettings settings(QSettings::UserScope, QLatin1String("Trolltech"));
         settings.beginGroup(QLatin1String("QtNetwork"));
         const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
         settings.endGroup();

         // If the saved network configuration is not currently discovered use the system default
         QNetworkConfiguration config = manager.configurationFromIdentifier(id);
         if ((config.state() & QNetworkConfiguration::Discovered) !=
             QNetworkConfiguration::Discovered) {
             config = manager.defaultConfiguration();
         }

         networkSession = new QNetworkSession(config, this);
         connect(networkSession, SIGNAL(opened()), this, SLOT(sessionOpened()));

         sendCmdButton->setEnabled(false);
         statusLabel->setText(tr("Opening network session."));
         networkSession->open();
     }
 }

 void Client::sendCmd()
 {
     sendCmdButton->setEnabled(false);
     blockSize = 0;
     if(!tcpSocket->isOpen()){
         tcpSocket->abort();
         tcpSocket->connectToHost(hostLineEdit->text(),
                                  portLineEdit->text().toInt());
         Q_ASSERT(tcpSocket->waitForConnected(1000));
     }
     writeCommand(commandLineEdit->text());
     sendCmdButton->setFocus();
 }

 void Client::writeCommand(QString cmd){
     QByteArray block;
     QDataStream out(&block, QIODevice::WriteOnly);
     out.setVersion(QDataStream::Qt_4_0);
     out << (quint16)0;
     out << cmd;
     out.device()->seek(0);
     out << (quint16)(block.size() - sizeof(quint16));


     tcpSocket->write(block);
     tcpSocket->waitForBytesWritten();
 }

 void Client::readResults()
 {
     QDataStream in(tcpSocket);
     in.setVersion(QDataStream::Qt_4_0);

     if (blockSize == 0) {
         if (tcpSocket->bytesAvailable() < (int)sizeof(quint16))
             return;

         in >> blockSize;
     }

     if (tcpSocket->bytesAvailable() < blockSize)
         return;

     QString results;
     in >> results;

     statusLabel->setText(results);
     sendCmdButton->setEnabled(true);
     sendCmdButton->setFocus();
 }

 void Client::displayError(QAbstractSocket::SocketError socketError)
 {
     switch (socketError) {
     case QAbstractSocket::RemoteHostClosedError:
         break;
     case QAbstractSocket::HostNotFoundError:
         QMessageBox::information(this, tr("Gate Client"),
                                  tr("The host was not found. Please check the "
                                     "host name and port settings."));
         break;
     case QAbstractSocket::ConnectionRefusedError:
         QMessageBox::information(this, tr("Gate Client"),
                                  tr("The connection was refused by the peer. "
                                     "Make sure the gate server is running, "
                                     "and check that the host name and port "
                                     "settings are correct."));
         break;
     default:
         QMessageBox::information(this, tr("Gate Client"),
                                  tr("The following error occurred: %1.")
                                  .arg(tcpSocket->errorString()));
     }

     sendCmdButton->setEnabled(true);
 }

 void Client::enableSendCmdButton()
 {
     sendCmdButton->setEnabled((!networkSession || networkSession->isOpen()) &&
                                  !hostLineEdit->text().isEmpty() &&
                                  !portLineEdit->text().isEmpty() &&
                                  !commandLineEdit->text().isEmpty());

 }

 void Client::sessionOpened()
 {
     // Save the used configuration
     QNetworkConfiguration config = networkSession->configuration();
     QString id;
     if (config.type() == QNetworkConfiguration::UserChoice)
         id = networkSession->sessionProperty(QLatin1String("UserChoiceConfiguration")).toString();
     else
         id = config.identifier();

     QSettings settings(QSettings::UserScope, QLatin1String("Trolltech"));
     settings.beginGroup(QLatin1String("QtNetwork"));
     settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
     settings.endGroup();

     statusLabel->setText(tr("This examples requires that you run the "
                             "Gate Server example as well."));

     enableSendCmdButton();
 }

 void Client::quitFromServer(){
     tcpSocket->disconnect();
 }

 void Client::serverDisconnected(){
     tcpSocket->close();
     statusLabel->setText(tr("The server was shutdown.  Closing socket."));
 }
