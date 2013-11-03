#include <QDebug>
#include "gate.h"

gate::gate(QObject *parent) :
    QObject(parent), isInitialized(false), m_gateOutput(logicUndefined)
{
}

gate::~gate()
{
    Q_ASSERT(getInitialized());
    qDebug() << "Gate " << getName() << " was destroyed.";
}

LogicLevel gate::getGateOutput() const {
    Q_ASSERT(getInitialized());
    return m_gateOutput;
}

void gate::setName(const QString &name) {
    Q_ASSERT(getInitialized());
    m_name = name;
}

QString gate::getName() const {
    Q_ASSERT(getInitialized());
    return(m_name);
}

QString convLogicToString(LogicLevel inVal) {
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

// This is the only place where the gate output value will be printed
//   qDebug() is used because this is really just to check that the code is doing what it should be doing
void gate::setGateOutput(LogicLevel output) {
    Q_ASSERT(getInitialized());
    Q_ASSERT(output != m_gateOutput);
    m_gateOutput = output;
}
