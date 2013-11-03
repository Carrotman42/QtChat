#ifndef THREADEDTCPSERVER_H
#define THREADEDTCPSERVER_H

#include <QtNetwork>

/**
 * @brief New QTcpServer object that handles threads and sockets.
 *
 * Performs all the same actions as a QTcpServer except this controls
 * incoming connections differently.  They never get queued in the
 * pending connections and are only handled here.
 *
 */
class ThreadedTCPServer:public QTcpServer
{
    Q_OBJECT

public:
/**
 * @brief Constructor for the ThreadedTCPServer.
 *
 * Instantiates a new object in the same manner as the QTcpServer.
 *
 * @param parent Initializes the QTcpServer with a qobject parent if desired
 * or can be instantiated without if desired.
 */
    ThreadedTCPServer(QObject * parent= 0);
protected:
    /**
     * @brief Overloaded function of the QTcpServer incomingConnection(); call
     *
     * This function differs because each new connection is placed into its own thread
     * to handle gates themselves. This also does not put the QTcpSocket into the
     * pending connections queue.  There is no need in this project to fill the queue.
     *
     * @param socketDescriptor The socket descriptor owned by the QTcpSocket. Identifies connections.
     */
    void incomingConnection(int socketDescriptor);
protected slots:
    /**
     * @brief Handler for when the thread is destroyed.
     *
     * Prints out a debug message, notifying the user.
     */
    void handleDestroyedThread();
    /**
     * @brief Handler for when the socket is destroyer.
     *
     * Prints out a debug message, notifying the user.
     */
    void handleDestroyedSocket();

public slots:
    /**
     * @brief Notifies sockets that this server is being disconnected.
     *
     * Emits the serverDisconnect(); signal to notify sockets
     *
     */
    void broadcastDisconnected();

signals:
    /**
     * @brief Tells connecitons that the server is disconnecting
     *
     * Emitted to notify sockets that the server disconnection is
     * taking place.
     *
     */
    void serverDisconnect();

};

#endif // THREADEDTCPSERVER_H
