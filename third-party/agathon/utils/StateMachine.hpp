/**
* @file This file defines a singular state machine, with no transition
* defined "a priori".
*
* @version 1.0
* @author Marcos Vinicius Naves Bedo (marcosivni@icmc.usp.br)
* @date 2013-30-05
*/

#ifndef STATEMACHINE_HPP
#define STATEMACHINE_HPP

/**
* This class defines a singular state machine, with no transition
* defined "a priori".
*
* @brief Defines a state machine.
* @author Marcos Vinicius Naves Bedo (marcosivni@icmc.usp.br)
* @version 1.0
* @see Lexical, Token
*/
class StateMachine{

    public:
        const static int ST_BEGIN = 0;
        const static int ST_END = 1;
        const static int ST_ERROR = 2;
        int currentState;

    public:
        StateMachine();
        ~StateMachine();

        void setCurrentState(int currentState);

        int getCurrentState();

        bool begin();
        bool end();
        bool error();
};

#endif // STATEMACHINE_HPP
