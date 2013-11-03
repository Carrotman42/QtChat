#ifndef TRUEGATE_H
#define TRUEGATE_H
#include "gate.h"

/**
 * @brief True gate class.
 *
 * The True gate class represents a boolean true value.
 * It is a gate purely for simplicity in connecting gates.
 *
 */
class trueGate : public gate
{
public:
    /**
     * @brief True gate constructor.
     *
     * Instantiates a new true gate object.
     *
     * @param parent Initializes the true gate with a qobject parent if desired
     * or can be instantiated without if desired.
     */
    explicit trueGate(QObject *parent = 0);

    /**
     * @brief Init function.
     *
     * Initializes the True gate to gateName.
     * Sets the LogicLevel to logicTrue.
     *
     * @param gateName The Name of the gate.
     */
    void init(const QString &gateName);

    /**
     * @brief Returns the number of inputs to the gate.
     *
     * This will always return 0 for trueGate.
     *
     * @return unsigned int The number of inputs.
     */
    unsigned int getNumInputs() const;

    /**
     * @brief Sets an input to inGate.
     *
     * In an andGate or an orGate, this would have a function.
     * In a falseGate or trueGate, this method does nothing.
     *
     * @param inGate The gate to connect as an input.
     */
    void setInput(gatePtrType inGate);

    /**
     * @brief Gets the name of an input described by inputNum.
     *
     * In an andGate or an orGate, this would have a function.
     * In a falseGate or trueGate, this method does nothing but returns a null QString.
     *
     * @param inputNum The number of the gate to communicate with.
     * @return QString The Name of the gate dictated by inputNum.
     */
    QString getInputName(unsigned int inputNum) const;

    /**
     * @brief Receives that one of this gate's inputs has changed. [Slot]
     *
     * This will never be called for falseGate nor for trueGate.
     * Their outputs will never change so we don't care if their inputs will
     * ever change (if there are potentially any)
     */
    void receiveInputChanged();

    /**
     * @brief Notified if one of its outputs is destroyed. [Slot]
     *
     * For a falseGate or TrueGate, this does nothing and will never
     * be called. These gates aren't supposed to have inputs and therefore
     * won't care that any have been destroyed.
     */
    void inputDestroyed();


    /**
     * @brief Destroys the gate.
     *
     * Destroys the gate - emits the destroyed() signal.
     */
    ~trueGate();

};

#endif // TRUEGATE_H
