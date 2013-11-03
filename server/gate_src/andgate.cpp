#include "andgate.h"
#include "gate.h"
#include <QDebug>


andGate::andGate(QObject *parent) :
    gate(parent)
{
}

andGate::~andGate(){
    emit destroyed();   //This is a QObject signal. In main,
                        //connect this to inputDestroyed() of whatever this gate is an input to;
}

void andGate::init(const QString &gateName){
    setInitialized();
    setName(gateName);
}

unsigned int andGate::getNumInputs() const{
    return inputPtrList.size();
}

void andGate::setInput(gatePtrType inGate){
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

QString andGate::getInputName(unsigned int inputNum) const{
    return inputPtrList.at(inputNum)->getName();
}

void andGate::receiveInputChanged(){
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
                if(previousOutput == logicTrue && newInput == logicFalse)
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

void andGate::inputDestroyed(){
    gate* callingGate = qobject_cast<gate*>(QObject::sender());
    if(callingGate){ //only perform from the destroyed signal we emit
        QObject::disconnect(callingGate, 0, this, 0); //disconnect all connections between calling gate and this one
        QString name = callingGate->getName();
        QMutableListIterator<gatePtrType> iter(inputPtrList);
        while (iter.hasNext()) {
            if (iter.next()->getName() == name){
                iter.remove();
                break;
            }
        }
        LogicLevel result = eval();
        if(getGateOutput() != result){
            setGateOutput(result);
            emit outputChanged();
        }
    }
}

//Eval runs through all the inputs, evaluating.
//Eval is only called by the slot receiveInputChanged()
//and is only called when the output cannot be immediately determined by the new input
LogicLevel andGate::eval(){
    if(getNumInputs() == 0)
        return logicUndefined;
    LogicLevel result = logicTrue;
    //loop through every input, evaluating as we go
    foreach(gatePtrType g, inputPtrList){
        LogicLevel tempOutput = g->getGateOutput();
        if(tempOutput != logicTrue){
            if(tempOutput == logicUndefined)
                return tempOutput; //If any input is undefined, the entire circuit is undefined from that point forward.
            result = tempOutput; //only set if false.  if it's set once, it will forever be false unless undefined.
        }
    }
    return result;
}
