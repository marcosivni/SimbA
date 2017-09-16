/**
* @file This file defines a singular token, that composes the SQL statement.
*
* @version 2.0
* @author Marcos Vinicius Naves Bedo (marcosivni@icmc.usp.br)
* @author Humberto Luiz Razente v1.0 (hlr@icmc.usp.br)
* @author Maria Camila Nardini Barioni v1.0 (mcamila@icmc.usp.br)
* @date 2013-30-05
*/

#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <vector>

using namespace std;

/**
* This is a base class to SQL statements.</br>
* A SQL statement is saw as a <b>sequence</b> of valid tokens. </br>
* <b>IMPORTANT</b>: A token object is singular, <b>NOT</b> a complete
* double linked list as previous version.<br>
* In the essency each token stores three importante informations: </br>
* 1. Lexem: The true content typed by the user.
* 2. TokenType: That defines what kind token we are dealing with (e.g. number, operator, etc.).</br>
* 3. Scope: This information is a strategy to sort the tokens according to the preference on
* a  regular SQL select statement.</br>
* e.g.: </br>
* <i> SELECT1  ... WHERE ... IN ( SELECT2 ... WHERE ... IN (SELECT3 ...) ) )</i></br>
* The equivalent validated token list will be: </br>
*  - SELECT1 - scope 0 </br>
*  - ...     - scope 0 </br>
*  - WHERE   - scope 0 </br>
*  - ...     - scope 0 </br>
*  - (       - scope 0 </br>
*  - SELECT2 - scope 1 </br>
*  - ...     - scope 1 </br>
*  - and so on ... </br>
*  - )       - scope 0 </br></br>
*
* The scope information allow us to sort the token hierarchically, in order to
* process SELECT's statements. </br>
*
* @brief Full class of token (definitions).
* @author Marcos Vinicius Naves Bedo (marcosivni@icmc.usp.br)
* @author Humberto Luiz Razente v1.0 (hlr@icmc.usp.br)
* @version 2.0
* @see Lexical, StateMachine
*/
class Token{

    public:
        static const int TK_LEX_ERROR = -10;
        static const int TK_UNTERMINATED_STRING = -11;
        static const int TK_FLOATINGPOINT_ERROR = -12;

        static const int TK_UNDEFINED = -1;
        static const int TK_IDENTIFIER = 100;
        static const int TK_INTEGER = 101;
        static const int TK_FLOATING_POINT = 102;
        static const int TK_STRING = 103;
        static const int TK_GQ = 110;             // >
        static const int TK_GE = 111;             // >=
        static const int TK_LQ = 112;             // <
        static const int TK_NE = 113;             // !=
        static const int TK_N2 = 114;             // <>
        static const int TK_LE = 115;             // <=
        static const int TK_OPEN_BRACE = 120;     // (
        static const int TK_CLOSE_BRACE = 121;    // )
        static const int TK_OPEN_BLOCK = 122;     // {
        static const int TK_CLOSE_BLOCK = 123;    // }
        static const int TK_COMMA = 124;          // ,
        static const int TK_SEMICOLON = 125;      // ;
        static const int TK_PERIOD = 126;         // .
        static const int TK_EQUAL = 127;          // =
        static const int TK_SUM = 128;            // +
        static const int TK_SUB = 129;            // -
        static const int TK_MUL = 130;            // *
        static const int TK_DIV = 131;            // /
        static const int TK_PER = 132;            // %
        static const int TK_IN = 133;             //IN (...)
        static const int TK_BETWEEN = 134;        //BETWEEN ... AND ...

    private:
        string lexem;
        int tokenType;
        int lexemType;
        int scopePosition;

    public:
        Token();
        Token(string lexem, int tokenType, int lexemType);
        Token(string lexem, int tokenType, int lexemType, int scopePosition);
        ~Token();
        string getLexem();

        void setLexem(string lexem);
        void setLexemType(int lexemType);
        void setScopePosition(int scopePosition);
        void setTokenType(int tokenType);

        int getLexemType();
        int getScopePosition();
        int getTokenType();

        string toUpperLexem();
        void appendToLexem(string str);
};

#endif // TOKEN_HPP
