#ifndef CLIENTGUI_H
#define CLIENTGUI_H

#include <QObject>
#include <QDialog>
#include <QtGui>

//#define testing


class QDialogButtonBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QTcpSocket;
class QNetworkSession;
class QTextEdit;
/**
 * @brief The clientGUI class Contains all the GUI elements for a client connection.
 *
 * This subclasses QDialog, similar to past homeworks and examples, to display a Qt GUI to
 * enable user functionality.  The GUI contains 2 major pages: a connection page and a chat room page.
 * One enables the connection to a specific server and the other actually sends  data to an active
 * socket connection.  This also receives relevent messages from the client regarding status of some commands.
 */
class clientGUI : public QDialog
{
    Q_OBJECT
public:
    /**
     * @brief clientGUI Build the initial connection page and sets up the signal-slot connections.
     *
     * This will not build the second page, as that will be done later in this GUI's lifetime.
     * No other signals will be set up other than connection page buttons to approproiate slots.
     * Client communication slots will be set up elsewhere (in main).
     * @param parent If there is a parent, this will set it.
     */
    explicit clientGUI(QWidget *parent = 0);

#ifdef testing
    /**
     * @brief setHostText sets the host text
     *
     * @param text The text to set
     * @return should be the same text as text, if set properly
     */
    QString setHostText(QString text);
    /**
     * @brief setPortNum Sets the port number to num
     * @param num the port number to set
     * @return the string of the set number
     */
    QString setPortNum(int num);
    /**
     * @brief setUserText Sets the user to text
     * @param text the text to the user name to
     * @return should be the same text as text, if set properly
     */
    QString setUserText(QString text);
    /**
     * @brief simulateConnectButtonPress calls the method connected to the button clicked signal
     * @return true if clicked properly, false if not
     */
    bool simulateConnectButtonPress();
    /**
     * @brief simulateQuitButtonPress calls the method connected to the button clicked signal
     * @return true if clicked properly, false if not
     */
    bool simulateQuitButtonPress();
    /**
     * @brief simulateSendButtonPress calls the method connected to the button clicked signal
     * @return true if clicked properly, false if not
     */
    bool simulateSendButtonPress();
    /**
     * @brief simulateTypingInNewMessage Sets the text in the new Message Window
     * @param message the text to set
     * @return the text that was set
     */
    QString simulateTypingInNewMessage(QString message);
    /**
     * @brief getMessageWindowText gets the text in the message window
     * @return The text in the message window
     */
    QString getMessageWindowText();
    /**
     * @brief getNewMessageWindowText gets the text in the new message window
     * @return the text in the new message window
     */
    QString getNewMessageWindowText();
    /**
     * @brief getUserWindowText Gets the text in the user window
     * @return the text in the user window
     */
    QString getUserWindowText();
    /**
     * @brief getStatusLabelText Gets the text in the status label
     * @return the text in the status label
     */
    QString getStatusLabelText();
    /**
     * @brief getIsConnectionsPage Returns the "isConnectionsPage" bool
     * @return the "isConnectionsPage" bool
     */
    bool getIsConnectionsPage();
#endif //testing

signals:
    /**
     * @brief sendConnectInfo Sends connect information to the client
     *
     * A client will be connected to the GUI through a slot connected to this signal.  This GUI
     * provides an ip address, port number, and username supplied by the user.
     *
     * @param ip The IP address to connect to.
     * @param port The port number to listen to and write to.
     * @param user THe user name that this client will be connecting with.
     */
    void sendConnectInfo(QString ip, int port, QString user);

    /**
     * @brief sendMsg Connect up to the client to pass along a message.
     *
     * This tells the client that the user has entered a message and hit send.  The message itself
     * is passed along as the msg param for the client to handle.
     * @param msg The message that the user entered.
     */
    void sendMsg(QString msg);

    /**
     * @brief quitClient Tells the connected slot that the quit has been issued.
     *
     * The user lets the client know that the GUI should be exited.
     */
    void quitClient();
    /**
     * @brief updateStatus Updates this user's status to status.
     *
     * This lets the client know that this user's status is changing to something else.
     * @param status the user's status
     */
    void updateStatus(QString status);


public slots:
    /**
     * @brief wasConnected If the connection was successful, set up the chat room page.
     *
     * This slot is connected to the client so if the connection between the client and the server
     * was established, the client lets the GUI know.  In doing so, the GUI destroys the old connection
     * page and builds a new chat room page to send messages to other users.
     */
    void wasConnected();
    /**
     * @brief gotMsg Handles the message provided by the connection.
     *
     * This will display the message supplied by the client connection.  The form will be:
     * username: msg
     * @param msg The message to be displayed
     * @param user The User sending the message
     */
    void gotMessage(QString user, QString msg);
    /**
     * @brief updateList The list of other usernames that the server is connected to.
     *
     * This is the list of users that we can communicate with as well as their status's, stored
     * in a QMap in the form where key=username, value=status.
     * @param map The list of users and their status's.
     */
    void updateList(QMap<QString, QString>& map);

    /**
     * @brief onServerDisconnect If the server was disconnected, the GUI needs to be sent back to initial page.
     *
     * The chat room page will be brought down on a server disconnect and the connection page will be
     * brought back up along with an error letting the user know that the server was disconnected.
     */
    void onServerDisconnect();

    /**
     * @brief catchError Prints out any error sent by the client.
     *
     * This will display the error in the GUI.
     *
     * @param error Error string to print out.
     */
    void catchError(QString error);

private slots:
    /**
     * @brief connectButtonPushed Read that the connect button was pushed.
     *
     * If this button was pressed, let the client know that this happened by emitting the sendConnectInfo();
     * signal.
     */
    void connectButtonPushed();
    /**
     * @brief sendButtonPushed Read that the send button was pushed.
     *
     * If this button was pushed read the command.  If the message starts with "\status .+" where .+ is
     * the regex of 1 or more characters, this user's status is updated to the server.  There can only be
     * a maximum of 20 characters for a status as well (not including the \status indicator.  If the
     * status doesn't meet these requirements, an error will be displayed.
     */
    void sendButtonPushed();
    /**
     * @brief quitButtonPushed Read that the quit button was pushed.
     *
     * If this button was pushed, emit quitClient();
     */
    void quitButtonPushed();

    /**
     * @brief enableConnectButton Enables the connect button if it should be enabled.
     *
     * Tests if the Connect button should be enabled by checking the contents of the
     * member line edits.  Then it enables it if necessary.
     */
    void enableConnectButton();

    /**
     * @brief enableSendButton Enables the send button if it should be enabled.
     *
     * Tests if the new message window has information typed in.  If it does, it enables
     * the button.
     */
    void enableSendButton();


private:
    /**
     * @brief buildConnectBox Builds page 1, the connection page
     *
     * This GUI screen will consist of the ip address, port number, and username fields as well as 2
     * buttons: connect and quit.  The page will be built from simple 1 line edit boxes and push buttons,
     * the same format as previous homeworks and the client example.  There will also be an error
     * line to display any closed connections from the server so reconnections can be attempted.
     */
    void buildConnectPage();
    /**
     * @brief buildChatRoom Builds page 2, the chat room page.
     *
     * This GUI screen will consist of 1 large chat window screen, displaying all message from all users.
     * This screen will have a vertical scrollbar on the right to view all previous messages.
     * Below this will be the editable text box where the user can type in his message. There will be a
     * maximum length of 100 (for now) characters to avoid any discrepencies with vertical or horizontal
     * scroll.  The the right of all this will be the users list.  This will contain a horizontal scroll bar
     * at the bottom of the screen to view the entire user's status.
     */
    void buildChatPage();

    /**
     * @brief tearDownConnectPage Destroys the whole connection page to make room for the chat page.
     *
     * Deletes all of the object related to the connections page.  If needed, they can be rebuilt later.
     */
    void tearDownConnectPage();
    /**
     * @brief tearDownChatPage Destroys the whole Chat Room page to make room for the connections page.
     *
     * Deletes all of the object related to the chat room page.  If needed, they can be rebuilt later.
     */
    void tearDownChatPage();

    /**
     * @brief postMessageToWindow Posts the sent the message sent by user to window
     *
     * If sent by this user, user is displayed in red.  Everyone else is in blue.
     * @param window the window to post to
     * @param user the user sending the message
     * @param message the message that was sent
     */
    void postMessageToWindow(QTextEdit* window, QString user, QString message);

    /**
     * @brief postError Posts the error to the GUI.
     *
     * If the GUI is in the connections page, errors are posted to the status label.
     * If it's in the chat room, errors are posted in grey to the message window
     * @param errorMessage The message to post
     */
    void postError(QString errorMessage);

    QLabel *hostLabel; /**< The label "Server Name:" */
    QLabel *portLabel; /**< The label "Server Port:" */
    QLabel *userNameLabel; /**< The label "Command:" */
    QLineEdit *hostLineEdit; /**< The editable box for the server name */
    QLineEdit *portLineEdit; /**< The editable box for the port number */
    QLineEdit *userNameLineEdit; /**< The editable box for the command string */
    QLabel *statusLabel; /**< The label displaying the result returned from the server */
    QPushButton *sendButton; /**< The button to send a command to the server */
    QPushButton *quitButton; /**< The button to make the client quit and disconnect */
    QPushButton *connectButton; /**< The button to make the client connect to the server */
    QDialogButtonBox *buttonBox; /**< Contains the two buttons */

    QTextEdit *messageWindow; /**< Contains all the messages from all users*/
    QTextEdit *userWindow; /**< Contains all the usernames and statuses */
    QTextEdit *newMessageWindow; /**< Contains the message the will be sent by this user */

    bool isConnectionsPage; /**< true if the connection page is active.  false if the chat page is active*/
    QString user; /**< The user name chosen - used to display different colors in the windows */


};

#endif // CLIENTGUI_H



