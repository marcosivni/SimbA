#include "StateMachine.hpp"

/**
* Empty constructor.
* </br>Sets the state machine to an invalid stage.
*/
StateMachine::StateMachine() {

    currentState =-1;
}

/**
* Destructor.
*/
StateMachine::~StateMachine(){
}

/**
* Sets the current state.
*
* @param currentState The current state.
*/
void StateMachine::setCurrentState(int currentState) {

    this->currentState = currentState;
}

/**
* Gets the current state.
*
* @return The current state.
*/
int StateMachine::getCurrentState() {

    return currentState;
}

/**
* Checks if the state machine is in the start stage.
*
* @return True if the machine is in the start stage, false otherwise.
*/
bool StateMachine::begin(){

    return (getCurrentState() == 0);
}

/**
* Checks if the state machine is in the end stage.
*
* @return True if the machine is in the end stage, false otherwise.
*/
bool StateMachine::end(){

    return (getCurrentState() == 1);
}

/**
* Checks if the state machine is in the error stage.
*
* @return True if the machine is in the error stage, false otherwise.
*/
bool StateMachine::error(){

    return (currentState == 2);
}
