
 #ifndef CLIENT_H
 #define CLIENT_H

 #include <QDialog>
 #include <QTcpSocket>

 class QDialogButtonBox;
 class QLabel;
 class QLineEdit;
 class QPushButton;
 class QTcpSocket;
 class QNetworkSession;

 /**
  * @brief Client object.
  *
  * This client object is meant to provide a GUI in order to communicate with a Server.
  * This GUI is lightweight in comparison, providing only socket communication and
  * through an interface.  The server that this connects to performs all major operations.
  *
  */
 class Client : public QDialog
 {
     Q_OBJECT

 public:
/**
 * @brief The constructor for a client object.
 *
 * This constructor builds the client object by setting up the entire GUI here and starting
 * a Network Manager.
 *
 * @param parent Initializes the server with a qobject parent if desired
 * or can be instantiated without if desired.
 */
     Client(QWidget *parent = 0);

 private slots:
     /**
      * @brief Controls sending a command to the server.
      *
      * First, this disables the push button and then sets up the communication with the server.
      * The communication is only set up if it currently isn't open. The communication is closed
      * every time this client quits or the server quits.  Last, this writes data to the socket.
      *
      */
     void sendCmd();
     /**
      * @brief Reads the results sent back by the server through the socket.
      *
      * This is set up through the readyRead() signal to run and continutously read back data.
      * The amount of data read back is identified by the first 16 bits read through the socket
      * on each new transfer
      *
      */
     void readResults();
     /**
      * @brief Displays any error in reading from the socket.
      *
      * Displays relevant error information in a popup message box.
      *
      * @param socketError the error seen on the socket.
      */
     void displayError(QAbstractSocket::SocketError socketError);
     /**
      * @brief Determines whether or not to enable the sendCmdButton.
      *
      * This looks at connection information as well as text that's been typed in to make
      * sure relevant information has been provided and that the action can actually do something
      * if clicked.
      *
      */
     void enableSendCmdButton();
     /**
      * @brief Sets up settings and enables sending commands.
      *
      * This only runs if the network manager was able to open up the session.  Enables
      * the sendCmdButton if the session was opened (and if the other requirements are met).
      *
      */
     void sessionOpened();
     /**
      * @brief Disconnects this socket from the server.
      *
      * This will emit a disconnected signal to whoever it's connected to.  In the case that even
      * a single command has been sent to the server, the server will catch the signal and
      * handle it accordingly.
      *
      */
     void quitFromServer();
     /**
      * @brief Reads that the server this is connected to has been disconnected.
      *
      * If the server has been disconnected, this tcp socket has to be closed and the user
      * should be notified that the server is no longer operation.  A new connection to a new
      * server must be made (or the same server if the server is returned to life).
      *
      */
     void serverDisconnected();

 private:
     /**
      * @brief Writes a command to the socket.
      *
      * This will first write data to a socket indicating how long this data will be. Then it writes
      * the data itself along the communication and waits for the bytes to be written before returning.
      *
      * @param QString The command string that we wish to pass to the server.
      */
     void writeCommand(QString);

     QLabel *hostLabel; /**< The label "Server Name:" */
     QLabel *portLabel; /**< The label "Server Port:" */
     QLabel *commandLabel; /**< The label "Command:" */
     QLineEdit *hostLineEdit; /**< The editable box for the server name */
     QLineEdit *portLineEdit; /**< The editable box for the port number */
     QLineEdit *commandLineEdit; /**< The editable box for the command string */
     QLabel *statusLabel; /**< The label displaying the result returned from the server */
     QPushButton *sendCmdButton; /**< The button to send a command to the server */
     QPushButton *quitButton; /**< The button to make the client quit and disconnect */
     QDialogButtonBox *buttonBox; /**< Contains the two buttons */

     QTcpSocket *tcpSocket; /**< The tcp socket that this client communicates with */
     quint16 blockSize; /**< The block size that will be read from the socket.  Resets on every new read */

     QNetworkSession *networkSession; /**< The session with the server */
 };

 #endif
