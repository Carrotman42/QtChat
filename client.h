#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>
#include <QString>
#include <QMap>

class QTcpSocket;
class QNetworkSession;

class Client : public QObject
{
    Q_OBJECT

public:
    Client(QObject *parent = 0);

public slots:
    /**
     * @brief sendMsg sends a command
     *
     * This sends a message to the server from the GUI
     *
     * @param message The message from the GUI
     */
    void sendMsg(QString message);

    /**
     * @brief quitFromServer quits from server
     *
     * This disconnects the client from the server and closes the socket.
     */
    void quitFromServer();

    /**
     * @brief login logs the user in
     *
     * This connects the user to the chat room and sends the username to the server.
     * It also takes the serverName and port number from the GUI to connect to the server.
     *
     * @param name the username from the GUI
     * @param serverName the server name the client is connecting to
     * @param port the port number the client needs to connect to the server
     */
    void login(QString name, QString serverName, int port);

    /**
     * @brief status updates the user's status
     *
     * Receives the user's status from the GUI and send its to the server.
     *
     * @param status the status entered by the user in the GUI
     */
    void status(QString status);

private slots:
    /**
     * @brief catchError Catches errors thrown
     *
     * Catches any errors thrown from the socket connection. Then emits sendError
     * to send the error to print on the GUI
     * @param socketError the error thrown from the socket connection
     */
    void catchError(QAbstractSocket::SocketError socketError);

    /**
     * @brief sessionOpened checks if connection is good
     *
     * Checks whether a successful connection with the server was established. If
     * the connection is good, it emits connected().
     */
    void sessionOpened();

    /**
     * @brief serverDisconnected slot to handle disconnects
     *
     * The slot handles disconnections. It shutdowns the socket if the server closes
     * before the client.
     */
    void serverDisconnected();

    /**
     * @brief readResults receives info from the socket
     *
     * The slot that actually receives the information from the socket. It then
     * decodes the information and calls appropriate slots or functions that
     * deal with the data sent.
     */
    void readResults();

signals:
    /**
     * @brief sendError sends errors to the GUI
     *
     * Emitted in catchError. It will send the error caught by catchError to be
     * printed in the GUI
     *
     * @param the error message to be printed
     */
    void sendError(QString error);

    /**
     * @brief newMessage a new message from another
     *
     * This is used to populate the GUI with a message generated from another user.
     *
     * @param user the username sending the message
     * @param msg the actual message
     */
    void newMessage(QString user,QString msg);

    /**
     * @brief listUpdate sends the list of users to the GUI
     *
     * Takes the QMap of users and their statuses received from the server and
     * sends it to the GUI
     *
     * @param map The QMap with all users and their statuses
     */
    void listUpdate(QMap<QString,QString>& map);

    /**
     * @brief connected tells GUI connected
     *
     * Emitted when a successful connection is established so the GUI can pop up and
     * accept user input.
     */
    void connected();

    /**
     * @brief sendServerDisconnect sends a message to the GUI saying the server disconnected.
     *
     * This is called in serverDisconnected() and lets the GUI know that the server
     * disconnected before the client closed so that it can prompt the user
     */
    void sendServerDisconnect();


private:

    /**
     * @brief sendCommand sends a message to the server
     *
     * The function to actually send the messsage in the form that is readable by the server.
     * It is called in
     *
     * @param msg the message that needs to be sent
     */
    void sendCommand(QString msg);

    /**
     * @brief getList gets the messages from the server
     *
     * This recieves the messages and the usernames that sent them according to the
     * defined protocol. This function decodes them and populates a QMap that is sent
     * to the GUI for display to the user.
     */
    void getList();



    QTcpSocket *tcpSocket;
    quint16 blockSize;
    QNetworkSession *networkSession;
};

#endif // CLIENT_H
