#ifndef GATESERVER_H
#define GATESERVER_H

#include <QObject>
#include <QTcpServer>
#include <QMap>

struct NameAndStatus {
    QString name;
    QString status;
};

/**
 * A class which takes care of listening and assigning sessions to incoming TCP connections.
 */
class chatserver : public QTcpServer
{
Q_OBJECT

public:
    /**
     * Creates a chat server
     */
    chatserver();

public slots:
    /**
     * Logs a user with the given name in.
     *
     * This method will cause a notification to be sent to all users
     * that the list of people in the chatroom has been updated.
     *
     * \param desc The number given to sessions to uniquely identify them
     * \param name The name of the user who wants to log in
     */
    void userLogin(int desc, QString name);
    
    /**
     * Broadcasts a message from the given user.
     *
     * \param desc The number given to sessions to uniquely identify them
     * \param msg Message they sent
     */
    void userMsg(int desc, QString msg);
    
    /**
     * Alters the status of the given user.
     *
     * This method will cause a notification to be sent to all users that
     * the list of people in the chatroom (and their statuses) has been updated.
     *
     * \param desc The number given to sessions to uniquely identify them
     * \param status Their updated status
     */
    void userStatus(int desc, QString status);
    
    /**
     * Logs a user out of the chatroom
     *
     * This method will cause a notification to be sent to all users
     * that the list of people in the chatroom has been updated.
     * 
     * \param desc The number given to sessions to uniquely identify them
     */
    void userLogout(int desc);
    
signals:
    /**
     * Will be emitted when the server sees that data needs to be broadcasted to
     * all users in the chat server
     *
     * \param data The preformated command to send to all clients.
     */
    void broadcastResponse(QByteArray data);
    
protected:
    /**
     * Handles a new incomming connection by making a new session
     *
     * \param socketDescriptor The file descriptor for the new socket
     */
    virtual void incomingConnection(int socketDescriptor);

private:
	/**
	 * Maps unique identifiers of sessions to their users and statuses.
	 *
	 * If a session is not in this map, they are considered not logged in. A user with a status
	 * of length 0 is still considered logged in.
	 */
	QMap<int, NameAndStatus> statuses;
};

#endif
