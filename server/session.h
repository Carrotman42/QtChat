#ifndef SESSION_H
#define SESSION_H

#include <QObject>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QStringList>

/**
 * Takes care of a single client session. Holds on to a shared instance of GateDB and
 * does all of its actions through there.
 */
class session : public QObject
{
Q_OBJECT

public:
    /**
     * Creates a new session based on the given descriptor. This takes care of
     * making a new thread and assigning the new session to the thread as well
     * as mapping the correct signals/slots so that when the connection ends
     * the thread will end.
     *
     * \param socketDescriptor The file descriptor for the client connection.
     * \param db The database object to use.
     * \returns The new session object.
     */
    static session* createSession(int socketDescriptor);

    /**
     * Destructor for the session class. Quits the thread to which the
     * object belongs.
     */
    ~session();

private slots:
    /**
     * Connected to the disconnected signal of the session's socket
     */
    void sockDisconnected();
    /**
     * Connected to the error signal of the session's socket
     *
     * \param socketError The error code associated with the socket error status
     */
    void sockError(QAbstractSocket::SocketError socketError);

    /**
     * Connected to the readyRead signal of the socket.
     */
    void gotClientMessage();

private:
    /**
     * Creates a new session object.
     *
     * \param sockDescr The file descriptor for the connection.
     */
    session(int sockDescr);

    /**
     * Sends a message to the connected client. Automatically appends a newline.
     *
     * Will emit the clientError signal if there was an issue sending the message.
     *
     * \param msg Message to send.
     */
    void sendResponse(QString msg);

signals:
    /**
     * Emitted when this session receives a logon attempt.
     *
     * \param name The name of the user who wants to log in
     */
    void recvLogin(QString name);
    
    /**
     * Emitted when this user sends a message.
     *
     * \param name Name of the user
     * \param msg Message they sent
     */
    void recvMsg(QString name, QString msg);
    
    /**
     * Emitted when this user changes their status
     *
     * \param name Name of the user
     * \param status Their updated status
     */
    void recvStatus(QString name, QString status);
    
public slots:
    /**
     * Called to send a response command back to the client.
     *
     * \param response A pre-formatted command to send to this client.
     */
    void sendResponse(QString response);

private:
    /**
     * The socket that is connected to the actual client.
     */
    QTcpSocket sock;
    
    /**
     * The name the user has logged in with
     */
    QString name;
};

#endif
