#include "falsegate.h"

falseGate::falseGate(QObject *parent) :
    gate(parent)
{
}

falseGate::~falseGate(){
    emit destroyed(); //This is a QObject signal. In main, connect this to inputDestroyed();
}

void falseGate::init(const QString &gateName){
    setInitialized();
    setName(gateName);
    setGateOutput(logicFalse);
}

unsigned int falseGate::getNumInputs() const{
    return 0;
}

void falseGate::setInput(gatePtrType inGate){
    //empty function:this should never be called
    inGate = NULL;
}

QString falseGate::getInputName(unsigned int inputNum) const{
    qWarning() << "getInputName called with " << inputNum;
    return QString();
}

void falseGate::receiveInputChanged(){
    //empty: should not be called
}

void falseGate::inputDestroyed(){
    //empty: should not be called
}
