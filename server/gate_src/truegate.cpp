#include "truegate.h"
#include "QDebug"

trueGate::trueGate(QObject *parent) :
    gate(parent)
{
}

trueGate::~trueGate(){
    emit destroyed(); //This is a QObject signal. In main, connect this to inputDestroyed();
}

void trueGate::init(const QString &gateName){
    setInitialized();
    setName(gateName);
    setGateOutput(logicTrue);
}

unsigned int trueGate::getNumInputs() const{
    return 0;
}

void trueGate::setInput(gatePtrType inGate){
    //empty function:this should never be called
    inGate = NULL;
}

QString trueGate::getInputName(unsigned int inputNum) const{
    qWarning() << "getInputName called with " << inputNum;
    return QString();
}

void trueGate::receiveInputChanged(){
    //empty: should not be called
}

void trueGate::inputDestroyed(){
    //empty: should not be called
}
