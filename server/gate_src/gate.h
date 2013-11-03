#ifndef GATE_H
#define GATE_H
#include <QDebug>
#include <QObject>
#include <QString>
#include <QPointer>

class gate; // need this "forward" definition
/**
 * @brief New pointer type for gates.
 *
 * For simplicity, we create the gatePtrType, a QPointer of our gate class.
 *
 */
typedef QPointer<gate> gatePtrType;

/**
 * @brief The possible states of the output of a gate.
 *
 * Holds the 3 states any given gate could possibly be.
 */
enum LogicLevel {logicTrue, logicFalse, logicUndefined};

/**
 * @brief Gate class. [Abstract]
 *
 * The Gate class is represents a physical any physical gate.
 * It is abstract so it must be derived - thus we have our 4 derivation classes.
 * This is set up to communicate partially through signals and slots and partially
 * through direct communication with inputs.
 */
class gate : public QObject
{
Q_OBJECT
public:
    //
    /**
     * @brief Init function.
     *
     * Initializes the gate to gateName.
     * Must be called prior to using an object of a class derived from gate
     * the argument is the name of the gate which the init() function should set.
     *
     * @param gateName The Name of the gate.
     */
    virtual void init(const QString &gateName) = 0;

    //
    /**
     * @brief Gets the output of this gate.
     *
     * Returns the logic value output by this gate given the state of its current inputs
     * Note: If any of the inputs to a gate are logicUndefined, then getGateOutput
     * must return logicUndefined.
     *
     * @return LogicLevel The LogicLevel of the output of this gate.
     */
    LogicLevel getGateOutput() const;

    /**
     * @brief Get the number of connected inputs.
     *
     * Returns the number of connected inputs to the gate.
     * If no inputs are connected, 0 is returned.
     *
     * @return unsigned int The number of connected inputs.
     */
    virtual unsigned int getNumInputs() const = 0;

    /**
     * @brief Add an input to this gate.
     *
     * inGate MUST point to a gate and may not be NULL.
     *
     * @param inGate A pointer to the input gate.
     */
    virtual void setInput(gatePtrType inGate) = 0;


    /**
     * @brief Gets the name of an input described by inputNum.
     *
     * Returns the name of one of the input gates, where the current inputs
     * are numbered 0 to getNumInputs()-1.
     *
     * @param inputNum The number of the gate to communicate with.
     * @return QString The Name of the gate dictated by inputNum.
     */
    virtual QString getInputName(unsigned int inputNum) const = 0;

    /**
     * @brief Destroys the gate.
     *
     * Destroys the gate - emits the destroyed() signal.
     */
    virtual ~gate();

    /**
     * @brief Returns the name of the gate.
     *
     * Returns a QString - the name of the gate.
     * Every gate MUST have a name, as set through init(QString).
     *
     * @return QString The name of the gate.
     */
    QString getName() const;

signals:

    /**
     * @brief Notifies connections that this output has changed. [Signal]
     *
     * Must be emitted whenever the output of the gate has changed (and only then)
     * This is set automatically through setInput(gatePtrType) but maybe be used elsewhere.
     *
     */
    void outputChanged();

protected slots:
    /**
     * @brief Receives that one of this gate's inputs has changed. [Slot]
     *
     * Should be called when an input to this gate has been changed.
     * Set up to communicate with other gates (specifically through outputChanged()).
     * This checks its own output by running through a list of inputs and grabbing each
     * of their LogicLevels.
     */
    virtual void receiveInputChanged() = 0;

    /**
     * @brief Notified if one of its outputs is destroyed. [Slot]
     *
     * Should be called when one of the gates that is an input to this gate has been destroyed.
     * Connected to the QObject destroyed() signal of gates that are inputs to this one.
     */
    virtual void inputDestroyed() = 0;

protected:
    /**
     * @brief Gate constructor.
     *
     * Instantiates a new gate object.
     *
     * @param parent Initializes the gate with a qobject parent if desired
     * or can be instantiated without if desired.
     */
    explicit gate(QObject *parent = 0);

    /**
     * @brief Sets the name of the gate.
     *
     * Sets the name of the gate to the provided QString.
     * This is meant to be called by the init() function of derived classes.
     *
     * @param name The name to set.
     */
    void setName(const QString &name);

    /**
     * @brief Set the logic level of the gate's output.
     *
     * This must only be called when the output has been changed.
     * Therefore, we force the program to crash if this is ever not the case.
     * If it were to continue running, execution would be undefined or simply
     * not working properly.
     *
     * @param output The logicLevel to set.
     */
    void setGateOutput(LogicLevel output);

    /**
     * @brief Ensures that the isInitialized boolean has been set by init().
     *
     * This is used to determine whether or not the init() method has been called.
     *
     * @return bool True if initialized, False otherwise.
     */
    bool getInitialized() const {return isInitialized;}
    /**
     * @brief Sets the initialized value to true.
     *
     * This happens when we call init so we know that whenever we access
     * a gate object, we know that it has been initialized.
     */
    void setInitialized() {isInitialized = true;}

private:
    QString m_name; /**< The name of the gate */

    bool isInitialized; /**< The initialization boolean - ensures init() has been called */

    LogicLevel m_gateOutput; /**< The output of the gate */
};

#endif // GATE_H
