#ifndef GATESERVER_H
#define GATESERVER_H

#include <QObject>
#include <QTcpServer>
#include <QMap>

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

protected:
    /**
     * Handles a new incomming connection by making a new session
     */
    virtual void incomingConnection(int socketDescriptor);

private:
	/**
	 * Maps users to their statuses.
	 *
	 * If a user is not in this map, they are considered not logged in. A user with a status that is
	 * of length 0 is still considered logged in.
	 */
	QMap<QString, QString> statuses;
};

#endif
