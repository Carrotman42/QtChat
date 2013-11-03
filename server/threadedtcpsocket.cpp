#include "threadedtcpsocket.h"
#include "gate_src/andgate.h"
#include "gate_src/falsegate.h"
#include "gate_src/truegate.h"
#include "gate_src/orgate.h"

ThreadedTCPSocket::ThreadedTCPSocket(QObject* parent) : QTcpSocket(parent), m_blockSize(0)
{
}

void ThreadedTCPSocket::handleStart(){
    connect(this, SIGNAL(readyRead()), this, SLOT(readFromSocket()));
    gatePtrType trueG = new trueGate;
    gatePtrType falseG = new falseGate;
    trueG->init("True");
    falseG->init("False");
    m_gates["True"] = trueG;
    m_gates["False"] = falseG;
}

void ThreadedTCPSocket::readFromSocket(){

    QDataStream in(this);
    in.setVersion(QDataStream::Qt_4_0);

    if (m_blockSize == 0) {
        if (bytesAvailable() < (int)sizeof(quint16))
            return;

        in >> m_blockSize;
    }

    if (bytesAvailable() < m_blockSize)
        return;

    QString readInData;
    in >> readInData;
    QStringList cmd = readInData.split(QRegExp("\\s+"));
    QString results = QString("");
    if(QString::compare(cmd.first(), QString("Create"), Qt::CaseInsensitive) == 0){
        if(cmd.size() == 2)
            results = createGate(cmd.at(1));
        else results = QString("Create failed. Number of arguments was %1").arg(cmd.size()-1);
    }
    else if(QString::compare(cmd.first(), QString("Connect"), Qt::CaseInsensitive) == 0){
        if(cmd.size() == 3)
            results = connectGate(cmd.at(1), cmd.at(2));
        else results = QString("Connect failed. Number of arguments was %1").arg(cmd.size()-1);
    }
    else if(QString::compare(cmd.first(), QString("Destroy"), Qt::CaseInsensitive) == 0){
        if(cmd.size() == 2)
            results = destroyGate(cmd.at(1));
        else results = QString("Destroy failed. Number of arguments was %1").arg(cmd.size()-1);
    }
    else if(QString::compare(cmd.first(), QString("Gateoutput"), Qt::CaseInsensitive) == 0){
        if(cmd.size() == 2)
            results = getOutput(cmd.at(1));
        else results = QString("Gateoutput failed. Number of arguments was %1").arg(cmd.size()-1);
    }
    else
        results = QString("Error: Could not find command " + cmd.first());

    //write out back to socket
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << results;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    write(block);
    m_blockSize = 0;
}

QString ThreadedTCPSocket::createGate(QString gate){
    if(m_gates.contains(gate))
        return QString(gate + " is already in circuit.");
    gatePtrType currentGate;
    if(gate.startsWith("and",Qt::CaseInsensitive)){
        currentGate = new andGate;
    }
    else if(gate.startsWith("or",Qt::CaseInsensitive)){
        currentGate = new orGate;
    }
    else return QString(gate + " not created.  Invalid gate name.");
    currentGate->init(gate);
    m_gates.insert(gate, currentGate);
    return QString(gate + " created");
}

QString ThreadedTCPSocket::connectGate(QString gate1, QString gate2){
    if(!m_gates.contains(gate1))
        return "Could not connect: " + gate1 + " not found";
    if(QString::compare(gate2, "false", Qt::CaseInsensitive) != 0 &&
            QString::compare(gate2, "true", Qt::CaseInsensitive) != 0 &&
            !m_gates.contains(gate2))
        return "Could not connect: " + gate2 + " not found";

    gatePtrType gateUnderChange = m_gates[gate1];
    gatePtrType gateToConnect;
    if(QString::compare(gate2, "true",Qt::CaseInsensitive) == 0)
        gateToConnect = m_gates["True"];
    else if(QString::compare(gate2, "false", Qt::CaseInsensitive) == 0)
        gateToConnect = m_gates["False"];
    else
        gateToConnect = m_gates[gate2];
    gateUnderChange->setInput(gateToConnect);
    gateUnderChange->getGateOutput();
    return QString(gate1 +  " " + convLogicToString(gateUnderChange->getGateOutput()));
}

QString ThreadedTCPSocket::destroyGate(QString gate){
    if(!m_gates.contains(gate))
        return "Could not destroy: " + gate + " not found";
    gatePtrType gateToDelete = m_gates[gate];
    delete gateToDelete;
    m_gates.remove(gate);
    return QString(gate + " destroyed");
}

QString ThreadedTCPSocket::getOutput(QString gate){
    if(!m_gates.contains(gate))
        return "Could not get gate output: " + gate + " not found";
    return QString(gate +  " " + convLogicToString(m_gates[gate]->getGateOutput()));
}

QString ThreadedTCPSocket::convLogicToString(LogicLevel inVal) {
    switch (inVal) {
    case logicFalse: {
            return("False");
        }
    case logicTrue: {
            return("True");
        }
    case logicUndefined: {
            return("Undefined");
        }
    default: {
            return("Error");
        }
    }
}

void ThreadedTCPSocket::deleteAllGates(){
    QMapIterator<QString, gatePtrType> iter(m_gates);
    while (iter.hasNext()) {
        iter.next();
        delete iter.value();
    }
    m_gates.clear();
    thread()->quit();
}

void ThreadedTCPSocket::disconnectSocket(){
    disconnectFromHost();
}
