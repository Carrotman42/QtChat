 #ifndef SERVER_H
 #define SERVER_H

 #include <QDialog>
#include <QtNetwork>
#include "threadedtcpserver.h"

 class QLabel;
 class QPushButton;
 class QTcpServer;
 class QNetworkSession;


 /**
  * @brief Server object.
  *
  * Interface for a QTcpServer (or more specifically, a ThreadedTCPServer).
  * This sets up the server session and the GUI.
  */
 class Server : public QDialog
 {
     Q_OBJECT

 public:
/**
 * @brief Constructor for the Server object.
 *
 * Creates a new Server Object with an optional parent.
 * This is where the GUI is created and the network manager instantiated.
 * A connection between the session and the sessionOpened() slot is created.
 *
 * @param parent Initializes the server with a qobject parent if desired
 * or can be instantiated without if desired.
 */
     Server(QWidget *parent = 0);

 private slots:
     /**
      * @brief Manages control when a session has been opened by the network manager.
      *
      * Creates and initializes the QTcpServer object that will be used to
      * interface with the socket connections with the clients.
      *
      */
     void sessionOpened();

     /**
      * @brief Attempts to notify sockets that this server is being disconnected.
      *
      * Calls a method that is part of the ThreadedTCPServer to notify that server's
      * sockets of the impending disconnect.
      *
      */
     void notifySockets();

 private:

     QLabel *statusLabel; /**< Label for a dialog box */
     QPushButton *quitButton; /**< A button that tells the server to quit */
     ThreadedTCPServer *tcpServer; /**< The QTcpServer that will handle socket communication */
     QNetworkSession *networkSession; /**< The QNetworkSession that sets up the QTcpServer */
 };


 #endif
