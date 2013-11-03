#ifndef ANDGATE_H
#define ANDGATE_H

#include "gate.h"
#include <QList>

/**
 * @brief And gate class.
 *
 * The And gate class is represents a physical And gate.
 * This is set up to communicate partially through signals and slots and partially
 * through direct communication with inputs.
 */
class andGate : public gate
{
public:
    /**
     * @brief And gate constructor.
     *
     * Instantiates a new and gate object.
     *
     * @param parent Initializes the and gate with a qobject parent if desired
     * or can be instantiated without if desired.
     */
    explicit andGate(QObject *parent = 0);

    /**
     * @brief Init function.
     *
     * Initializes the And gate to gateName.
     *
     * @param gateName The Name of the gate.
     */
    void init(const QString &gateName);

    /**
     * @brief Get the number of connected inputs.
     *
     * Returns the number of connected inputs to the gate.
     * If no inputs are connected, 0 is returned.
     *
     * @return unsigned int The number of connected inputs.
     */
    unsigned int getNumInputs() const;

    /**
     * @brief Sets an input to inGate.
     *
     * This gate takes an input, inGate, and sets in in a list.
     * It connects up the inGate's signals to this gates slot for when an
     * output changes and when the input is destroyed.
     * It then evaluates the output and notifies connections if necessary.
     *
     * @param inGate The gate to connect as an input.
     */
    void setInput(gatePtrType inGate);

    /**
     * @brief Gets the name of an input described by inputNum.
     *
     * The number passed in, inputNum, is assumed to be correct.
     * The gate will search through its inputs and return the name of that gate.
     *
     * @param inputNum The number of the gate to communicate with.
     * @return QString The Name of the gate dictated by inputNum.
     */
    QString getInputName(unsigned int inputNum) const;

    /**
     * @brief Receives that one of this gate's inputs has changed. [Slot]
     *
     * Should be called when an input to this gate has been changed.
     * Set up to communicate with other gates (specifically through outputChanged()).
     * This checks its own output by running through a list of inputs and grabbing each
     * of their LogicLevels.
     */
    void receiveInputChanged();

    /**
     * @brief Notified if one of its outputs is destroyed. [Slot]
     *
     * Should be called when one of the gates that is an input to this gate has been destroyed.
     * Connected to the QObject destroyed() signal of gates that are inputs to this one.
     */
    void inputDestroyed();

    /**
     * @brief Destroys the gate.
     *
     * Destroys the gate - emits the destroyed() signal.
     */
    ~andGate();
private:
    /**
     * @brief Evaluation method for the And gate
     *
     * This loops through connected inputs to test the output levels.
     * This functionality matches that of a physical and gate.
     * It will return logicFalse, logicTrue, or logicUndefined.
     *
     * @return LogicLevel The output LogicLevel of this gate.
     */
    LogicLevel eval(); //private use of eval
    int m_numInputs; /**< The number of inputs for this gate */
    QList<gatePtrType> inputPtrList; /**< A QList of pointers to gate objects. Used to communicate with inputs */
};

#endif // ANDGATE_H
