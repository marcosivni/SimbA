#ifndef LEXICAL_HPP
#define LEXICAL_HPP

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <Token.hpp>
#include <StateMachine.hpp>
#include <algorithm>

using namespace std;

class Lexical{

    private:
        string input;
        int length;
        int position;
        StateMachine *state;

    public:
        // Defines the possible lexem types.
        const static int LK_UNDEFINED = -2;
        const static int LK_COLUMN_NAME = 200;
        const static int LK_TABLE_NAME = 201;
        const static int LK_TABLE_NAME_REF = 202;
        const static int LK_STILLIMAGE_DATATYPE = 203;
        const static int LK_PARTICULATE_DATATYPE = 204;
        const static int LK_AUDIO_DATATYPE = 205;
        const static int LK_DELETE_CLAUSE = 206;
        const static int LK_SUM_SG = 207;
        const static int LK_AVG_SG = 208;
        const static int LK_ALL_SG = 209;
        const static int LK_EVERY_SG = 210;
        const static int LK_ANY_SG = 211;
        const static int LK_MAX_SG = 212;
        const static int LK_TIMESERIES_DATATYPE = 213;

    private:
        string getChar();

        void resolveExclamationMark(string ch, Token *token);
        void resolveGreaterSymbol(string ch, Token *token);
        void resolveIdentifier(string ch, Token *token);
        void resolveLessSymbol(string ch, Token *token);
        void resolveNumber(string ch, Token *token);
        void resolveNumberExponential(string ch, Token *token);
        void resolveNumberIntegerPart(string ch, Token *token);
        void resolveNumberSignal(string ch, Token *token);
        void resolveQuotes(string quote, Token *token);
        void resolveSymbols(string ch, Token *token);
        void ungetChar();

        string base64Chars();
        bool isBase64(unsigned char c);

    public:
        Lexical();
        Lexical(string input);

        string toUpper(string input);
        string toHexaDecimal(string input);
        string toBase64(string input);
        string fromHexaDecimal(string input);
        string fromBase64(string input);

        void setLength(int length);
        void setPosition(int position);
        void setInput(string input);
        void setState(int state);

        int getLength();
        int getPosition();
        string getInput();
        int getState();

        Token * getToken();
};
#endif // LEXICAL_HPP
