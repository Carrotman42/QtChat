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
     * Creates a new session object.
     *
     * \param sockDescr The file descriptor for the connection.
     * \param desc A number to uniquely identify this session
     */
    session(int sockDescr, int desc);

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
     * \param desc The unique identifier for this session
     * \param name The name of the user who wants to log in
     */
    void recvLogin(int desc, QString name);
    
    /**
     * Emitted when this user sends a message.
     *
     * \param desc The unique identifier for this session
     * \param msg Message they sent
     */
    void recvMsg(int desc, QString msg);
    
    /**
     * Emitted when this user changes their status
     *
     * \param desc The unique identifier for this session
     * \param status Their updated status
     */
    void recvStatus(int desc, QString status);
    
    /**
     * Emit when the client disconnects (or leaves) the chatroom.
     *
     * \param desc The unique identifier for this session
     */
    void clientDisconnected(int desc);
    
public slots:
    /**
     * Called to send a response command back to the client.
     *
     * \param response A pre-formatted command to send to this client.
     */
    void sendResponse(QByteArray response);

private:
    /**
     * The socket that is connected to the actual client.
     */
    QTcpSocket sock;
    
    /**
     * The unique identifier given at construction
     */
    int desc;
};

#endif
