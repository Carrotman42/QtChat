#include "orgate.h"
#include "gate.h"
#include <QDebug>


orGate::orGate(QObject *parent) :
    gate(parent)
{
}

orGate::~orGate(){
    emit destroyed();   //we emit ourselves so we know which object sent the signal.
}

void orGate::init(const QString &gateName){
    setInitialized();
    setName(gateName);
}

unsigned int orGate::getNumInputs() const{
    return inputPtrList.size();
}

void orGate::setInput(gatePtrType inGate){
    QObject::connect(inGate,SIGNAL(outputChanged()), this, SLOT(receiveInputChanged()));
    QObject::connect(inGate, SIGNAL(destroyed()), this, SLOT(inputDestroyed()));
    inputPtrList.append(inGate);
    if(inGate->getGateOutput() != getGateOutput()){
        LogicLevel result = eval();
        if(getGateOutput() != result){
            setGateOutput(result);
            emit outputChanged();
        }
    }
}

QString orGate::getInputName(unsigned int inputNum) const{
    return inputPtrList.at(inputNum)->getName();
}

void orGate::receiveInputChanged(){
    //A regular pointer is used here in place of gatePtrType because of the QT Documentation:
    //"Use a QPointer when you are storing a pointer over time"
    //This callingGate will only be used in this method and then will be discarded.
    gate* callingGate = qobject_cast<gate*>(QObject::sender());
    //if method called directly, just eval and set gate output
    LogicLevel previousOutput = getGateOutput();
    if(callingGate){
        LogicLevel newInput = callingGate->getGateOutput();
        if(newInput != previousOutput){
            if(newInput != logicUndefined){
                if(previousOutput == logicFalse && newInput == logicTrue)
                    setGateOutput(newInput);
                else{ //don't know what we'll end up with - must test
                    LogicLevel result = eval();
                    if(previousOutput != result)
                        setGateOutput(result);
                }
            }
            else
                setGateOutput(logicUndefined);
        }
    }
    else{
        LogicLevel result = eval();
        if(previousOutput != result)
            setGateOutput(result);
    }
    if(previousOutput != getGateOutput())
        emit outputChanged();
}

void orGate::inputDestroyed(){
    gate* callingGate = qobject_cast<gate*>(QObject::sender());
    if(callingGate){ //only perform from the destroyed signal we emit
        QObject::disconnect(callingGate, 0, this, 0);
        QString name = callingGate->getName();
        QMutableListIterator<gatePtrType> iter(inputPtrList);
        while (iter.hasNext()) {
            if (iter.next()->getName() == name){
                iter.remove();
                break;
            }
        }
        LogicLevel result = eval();
        if(getGateOutput() != result)
            setGateOutput(result);

        emit outputChanged();
    }
}

//Eval runs through all the inputs, evaluating.
//Eval is only called by the slot receiveInputChanged()
//and is only called when the output cannot be immediately determined by the new input
LogicLevel orGate::eval(){
    if(getNumInputs() == 0)
        return logicUndefined;
    LogicLevel result = logicFalse;
    //loop through every input, evaluating as we go
    foreach(gatePtrType g, inputPtrList){
        LogicLevel tempOutput = g->getGateOutput();
        if(tempOutput != logicFalse){
            if(tempOutput == logicUndefined)
                return tempOutput; //If any input is undefined, the entire circuit is undefined from that point forward.
            result = tempOutput; //only set if true.  if it's set once, it will forever be true unless undefined.
        }
    }
    return result;
}
