#ifndef THREADEDTCPSOCKET_H
#define THREADEDTCPSOCKET_H

#include <QtNetwork>
#include "gate_src/gate.h"

/**
 * @brief New Threaded QTcpSocket that lives in its own thread and handles communication with clients.
 *
 * This socket controls a set of gates specific to this thread and specific to this socket.
 */
class ThreadedTCPSocket : public QTcpSocket
{
    Q_OBJECT

public:
/**
 * @brief Constructor for the ThreadedTCPSocket.
 *
 * Instantiates a new object in the same manner as the QTcpSocket.
 *
 * @param parent Initializes the QTcpSocket with a qobject parent if desired
 * or can be instantiated without if desired.
 */
    ThreadedTCPSocket(QObject * parent = 0);

private slots:

    /**
     * @brief Initializes the socket to run. [Slot]
     *
     * Sets up connections for reading from the socket and initializes
     * the true and false gates, loading them into the qmap.
     *
     */
    void handleStart();

    /**
     * @brief Handles reading from the socket any time reading is available. [Slot]
     *
     * When connected to a socket's readyRead() signal, this will read in commands
     * and run through them, executing them in regards to this socket's map of gates.
     */
    void readFromSocket();

    /**
     * @brief Deletes all the gates from the circuit.
     *
     * This is used to clear the map of gates.  After this is performed, the thread
     * is quit.
     *
     */
    void deleteAllGates();

    /**
     * @brief Disconnects the socket from the host.
     *
     * Calls the disconnectFromHost() function to disconnect the socket.
     *
     */
    void disconnectSocket();

private:
    /**
     * @brief Creates a gate object.
     *
     * Creates a new gate object depending on the gate name and loads it into
     * the Qmap.
     *
     * @param name The name of the gate to create.
     * @return QString Returns "gateName created" if successful, otherwise an error message is returned.
     */
    QString createGate(QString name);

    /**
     * @brief Connects two gates, one as an input to another.
     *
     * The gate specified by name2 becomes the input to the gate specified by gate1.
     *
     * @param name1 The name of the gate to be changed.
     * @param name2 The name of the gate to be an input.
     * @return QString Returns "gateName LogicLevel" if successful, otherwise an error message is returned.
     */
    QString connectGate(QString name1, QString name2);

    /**
     * @brief Destroys the gate.
     *
     * Destroys the gate by calling delete on the gate and removing it from the list.
     * ALl connections are then handled by the gate objects through signals and slots.
     *
     * @param name The name of the gate to be destroyed.
     * @return QString Returns "gateName destroyed" if successful, otherwise an error message is returned.
     */
    QString destroyGate(QString name);

    /**
     * @brief Gets the output LogicLevel of the gate.
     *
     * Calls the gate's getGateOutput() function to retrieve the output and then return it to the client.
     *
     * @param name The name of the gate to get the LogicLevel of.
     * @return QString Returns "gateName LogicLevel" if successful, otherwise an error message is returned.
     */
    QString getOutput(QString name);

    /**
     * @brief Converts the LogicLevel to a QString.
     *
     * Equivalent to the method of the same name in gate.cpp
     *
     * @param level The LogicLevel to be converted.
     * @return QString The equivalent QString of the LogicLevel provided.
     */
    QString convLogicToString(LogicLevel level);

    //members
    quint16 m_blockSize; /**< The block size of the incoming message */
    QMap<QString, gatePtrType> m_gates; /**< The map of gates and their names */
};

#endif // THREADEDTCPSOCKET_H
