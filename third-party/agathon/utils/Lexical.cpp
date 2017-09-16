#include "Lexical.hpp"

/**
* This is a default empty constructor.
*/
Lexical::Lexical(){

    //Just for sanity
    setInput("");
    setPosition(0);
    setLength(0);
    state = new StateMachine();
}

/**
* Constructor.
* @param input The input string to be analised.
*/
Lexical::Lexical(string input){

    this->input = input;
    length = this->input.length();
    position = 0;
    state = new StateMachine;
}

/**
* This method is used on getToken() to pick the next character.
* The next character is the one at the index <i>position</i>.
* @return The next character of the input string. If there is no more characteres to analise in the input string, returns an empty string.
*/
string Lexical::getChar(){

    if(position < length)
    {
        string ch = input.substr(position, 1);
        position++;
        return ch;
    }
    position++;
    state->setCurrentState(StateMachine::ST_ERROR); // Reached the end of input string
    return "";
}

/**
* This method shoud be avoided !!!
* Returns the next Token, including the lexeme and the token type.
* @return The next Token. If there is no more tokens, returns a NULL pointer.
*/
Token* Lexical::getToken(){

    Token *token = new Token;
    state->setCurrentState(StateMachine::ST_BEGIN); // First, set state as BEGIN
    string ch = this->getChar(); // getChar() can change the state to ERROR!

    while(!state->end() && !state->error()){

        // Spaces: advance to the next character
        if(ch == " "){
            ch = this->getChar();
        }
        // Strings 'xyz' or "xyz": quotation marks
        else if(ch =="\'" || ch == "\""){
            this->resolveQuotes(ch, token);
        }
        // Identifier: [a-zA-Z][a-zA-z0-9$]*
        else if( (ch >= "A" && ch <= "Z") || (ch >= "a" && ch <= "z") || ch == "$"){
            this->resolveIdentifier(ch, token);
        }
        // Number: [-+]?[0-9]+("."[0-9]*)?([eE]"-"?[0-9]*)?
        else if( (ch >= "0" && ch <= "9") || ch == "+" || ch == "-"){
            this->resolveNumber(ch, token);
        }
        // Symbols ">" and ">="
        else if(ch == ">"){
            this->resolveGreaterSymbol(ch, token);
        }
        // Symbols "<", "<=" and "<>"
        else if(ch == "<"){
            this->resolveLessSymbol(ch, token);
        }
        // Symbol "!="
        else if(ch == "!"){
            this->resolveExclamationMark(ch, token);
        }
        // Symbols ( ) { } , ; . = * / %
        else if(ch == "(" || ch == ")" || ch == "{" || ch == "}" || ch == "," || ch == ";" || ch == "." || ch == "=" || ch == "*" || ch == "/" || ch == "%"){
            this->resolveSymbols(ch, token);
        }
    } // End while

    return (state->end()) ? token : 0;
}

/**
* This method is used on getToken() in order to recognize symbol \"!=\".
* @param ch The character (\"!\") read from the input string.
* @param token The token to be constructed.
*/
void Lexical::resolveExclamationMark(string ch, Token *token){

        token->setLexem(ch);
        ch = this->getChar();

        if(ch == "=") // Symbol !=
        {
                token->appendToLexem(ch);
                token->setTokenType(Token::TK_NE);
        }
        else
        {
                this->ungetChar(); // Just the symbol !. Put back the character read.
                token->setTokenType(Token::TK_LEX_ERROR);
        }

        state->setCurrentState(StateMachine::ST_END);
}

/**
* This method is used on getToken() in order to recognize symbols \"&gt;\" and \"&ge;\".
* @param ch The character (\"&gt;\") read from the input string.
* @param token The token to be constructed.
*/
void Lexical::resolveGreaterSymbol(string ch, Token *token){

        token->setLexem(ch);
        ch = this->getChar();

        if(ch == "=") // Symbol >=
        {
                token->appendToLexem(ch);
                token->setTokenType(Token::TK_GE);
        }
        else
        {
                this->ungetChar(); // Just the symbol >. Put back the character read.
                token->setTokenType(Token::TK_GQ);
        }

        // If the previous state == BEGIN, there are more characters to process.
        // If the previous state == ERROR, the input string finished.
        // In both cases, this method recognizes the symbol and finishes in a success state.
        state->setCurrentState(StateMachine::ST_END);
}

/**
* This method is used on getToken() in order to recognize strings in format [a-zA-Z][a-zA-Z0-9$]*.
* @param ch The character read from the input string.
* @param token The token to be constructed.
*/
void Lexical::resolveIdentifier(string ch, Token *token){

        // Add letters to the lexeme
        while( (ch >= "A" && ch <= "Z") || (ch >= "a" && ch <= "z") || (ch >= "0" && ch <= "9") || ch == "_" || ch == "$")
        {
                token->appendToLexem(ch);
                ch = this->getChar();
        }

        this->ungetChar(); // Put back the character that broke the loop

        token->setTokenType(Token::TK_IDENTIFIER);

        //IN and BETWEEN are actually operators in
        //string format
        if (token->toUpperLexem() == "IN"){
            token->setTokenType(Token::TK_IN);
        }
        if (token->toUpperLexem() == "BETWEEN"){
            token->setTokenType(Token::TK_BETWEEN);
        }

        // If the previous state == BEGIN, there are more characters to process.
        // If the previous state == ERROR, the input string finished.
        // In both cases, this method recognizes the identifier and finishes in a success state.
        state->setCurrentState(StateMachine::ST_END);
}

/**
* This method is used on getToken() in order to recognize symbols \"&lt;\", \"&le;\" and \"&lt;&gt;\".
* @param ch The character (\"&lt;\") read from the input string.
* @param token The token to be constructed.
*/
void Lexical::resolveLessSymbol(string ch, Token *token){

        token->setLexem(ch);
        ch = this->getChar();

        if(ch == "=") // Symbol <=
        {
                token->appendToLexem(ch);
                token->setTokenType(Token::TK_LE);
        }
        else if(ch == ">") // Symbol <>
        {
                token->appendToLexem(ch);
                token->setTokenType(Token::TK_N2);
        }
        else
        {
                this->ungetChar(); // Just the symbol <. Put back the character read.
                token->setTokenType(Token::TK_LQ);
        }

        // If the previous state == BEGIN, there are more characters to process.
        // If the previous state == ERROR, the input string finished.
        // In both cases, this method recognizes the symbol and finishes in a success state.
        state->setCurrentState(StateMachine::ST_END);
}

/**
* This is a regex method.
* This method is used on getToken() in order to recognize
* numbers in format [-+]?[0-9]+("."[0-9]*)?([eE]"-"?[0-9]*)?.
*
* @param ch The character read from the input string.
* @param token The token to be constructed.
*/
void Lexical::resolveNumber(string ch, Token *token){

        // Recognize the signal: + or - or nothing
        this->resolveNumberSignal(ch, token);

        // Just continue if there are digits to process
        while(!state->end())
        {
                ch = this->getChar(); // Here, must be at least one digit

                // Get digit(s)
                this->resolveNumberIntegerPart(ch, token);

                // The next character can be "." (floating point), an "eE" (exponential) or anything (integer)
                ch = this->getChar();
                if(ch == ".") // Floating point number (for example, 12.345)
                {
                        // Add the "." to the lexeme
                        token->appendToLexem(ch);

                        // After the "." must be at least one digit
                        ch = this->getChar();
                        if(ch >= "0" && ch <= "9")
                        {
                                this->resolveNumberIntegerPart(ch, token);

                                // Checking exponential (Number in format 12.34e56)
                                ch = this->getChar();
                                if(ch == "e" || ch == "E")
                                {
                                        this->resolveNumberExponential(ch, token);
                                        state->setCurrentState(StateMachine::ST_END);
                                }
                                else
                                {
                                        this->ungetChar(); // Put back the character read.
                                        token->setTokenType(Token::TK_FLOATING_POINT);
                                        state->setCurrentState(StateMachine::ST_END);
                                }
                        }
                        else
                        {
                                token->setTokenType(Token::TK_FLOATINGPOINT_ERROR);
                                state->setCurrentState(StateMachine::ST_END);
                        }
                }
                else if(ch == "e" || ch == "E")
                {
                        this->resolveNumberExponential(ch, token);
                        state->setCurrentState(StateMachine::ST_END);
                }
                else // Integer
                {
                        this->ungetChar(); // Put back the character read.
                        token->setTokenType(Token::TK_INTEGER);
                        state->setCurrentState(StateMachine::ST_END); // Stop
                }
        }
}

/**
* This method is used on resolveNumber() in order to recognize exponential part of a number.
* @param ch The character (\"e\" or \"E\") read from the input string.
* @param token The token to be constructed.
*/
void Lexical::resolveNumberExponential(string ch, Token *token){

        // Add the "eE" to the lexeme
        token->appendToLexem(ch);

        // Check the signal
        ch = this->getChar();
        this->resolveNumberSignal(ch, token);

        // Check if there are digits to process
        if(!state->end())
        {
                ch = this->getChar(); // Here, must be at least one digit

                // Get digit(s)
                this->resolveNumberIntegerPart(ch, token);
                token->setTokenType(Token::TK_FLOATING_POINT);
        }
        else // Error: just a signal after "eE".
        {
                token->setTokenType(Token::TK_FLOATINGPOINT_ERROR);
        }
}

/**
* This method is used on resolveNumber() in order to recognize integer part of a number.
* @param ch The character read from the input string.
* @param token The token to be constructed.
*/
void Lexical::resolveNumberIntegerPart(string ch, Token *token){
        // Add digits to the lexeme

        while(ch >= "0" && ch <= "9"){
                token->appendToLexem(ch);
                ch = this->getChar();
        }

        this->ungetChar(); // Put back the character that broke the loop
}

/**
* This method is used on resolveNumber() in order to recognize the signals \"+\" and \"-\".
* @param ch The character read from the input string.
* @param token The token to be constructed.
*/
void Lexical::resolveNumberSignal(string ch, Token *token){

        if(ch == "+" || ch == "-"){

                if(ch == "+") token->setTokenType(Token::TK_SUM);
                else token->setTokenType(Token::TK_SUB);
                // Here, just append.
                // Do not set because exponential can be signalized too.
                token->appendToLexem(ch);


                // Checks if the signal is followed by a number.
                // If it is not, we have a + or - operator; otherwise, is a signed number.
                ch = this->getChar();

                // Read a character more to perform this checking.
                // Outside this IF, it will be put back.
                if(ch < "0" || ch > "9"){
                        // Finish. It is just a signal.
                        state->setCurrentState(StateMachine::ST_END);
                }
        }

        // If ch is a signal, inside the IF an extra character was read.
        // So, unget that character.
        // If ch is not a signal, put it back.
        this->ungetChar();
}

/**
* This method is used on getToken() in order to recognize strings in format \'xyz\' or \"xyz\".
* @param quote The character (a quote) read from the input string.
* @param token The token to be constructed.
*/
void Lexical::resolveQuotes(string quote, Token *token){

        string ch;
        token->setLexem(quote); // Initialize the token lexeme

        do{
                ch = this->getChar();
                token->appendToLexem(ch);
        } while(ch != quote && !state->error());

        // Set the token type
        if(!state->error()){
                token->setTokenType(Token::TK_STRING);
        } else {
                token->setTokenType(Token::TK_UNTERMINATED_STRING);
        }

        // If the previous state == BEGIN, there are more characters to process.
        // If the previous state == ERROR, the input string finished and string is unterminated.
        // In both cases, this method recognizes the string (or unterninated string) and
        // finishes in a success state.
        state->setCurrentState(StateMachine::ST_END);;
}

/**
* This method is used on getToken() in order to recognize symbols
* \"(\", \")\", \"{\", \"}\", \",\", \";\", \".\", \"=\", \"*\", \"/\" and \"%\".
* @param ch The character read from the input string.
* @param token The token to be constructed.
*/
void Lexical::resolveSymbols(string ch, Token *token){

    token->setLexem(ch);

    char c = ch.at(0);

    switch(c){
        case '(': token->setTokenType(Token::TK_OPEN_BRACE); break;
        case ')': token->setTokenType(Token::TK_CLOSE_BRACE); break;
        case '{': token->setTokenType(Token::TK_OPEN_BRACE); break;
        case '}': token->setTokenType(Token::TK_OPEN_BLOCK); break;
        case ',': token->setTokenType(Token::TK_COMMA); break;
        case ';': token->setTokenType(Token::TK_SEMICOLON); break;
        case '.': token->setTokenType(Token::TK_PERIOD); break;
        case '=': token->setTokenType(Token::TK_EQUAL); break;
        case '*': token->setTokenType(Token::TK_MUL); break;
        case '/': token->setTokenType(Token::TK_DIV); break;
        case '%': token->setTokenType(Token::TK_PER); break;
    }

    state->setCurrentState(StateMachine::ST_END);
}

/**
* Decreases the current position in the input string by one character.
* This method makes the last character extracted from the input once again available.
* It is useful to eliminate characters such as spaces when getting the lexemes.
* Its not possible to \"unget\" character if the end of the input string was reached.
*/
void Lexical::ungetChar(){

    // If state == ERROR, the end of the input string was reached.
    // So, it is not possible to unget.
    if(!state->error()){
        position--;
    }
}


string Lexical::base64Chars(){

    return "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
}

bool Lexical::isBase64(unsigned char c){

    return (isalnum(c) || (c == '+') || (c == '/'));
}

string Lexical::toUpper(string input){

    string aux = input;
    char empty = ' ';
    string aux2;

    std::transform(aux.begin(), aux.end(), aux.begin(), ::toupper);

    if ((aux.size() != input.size()) || (aux[aux.size()-1] == empty)){
        aux2 = aux;
        aux = "";
        for (int x = 0; x < aux2.size()-1; x++)
            aux += aux2[x];
    }

    return aux;
}


string Lexical::toHexaDecimal(string input){

    static const char* const lut = "0123456789ABCDEF";
    size_t len = input.length();

    string output;
    output.reserve(2 * len);
    for (size_t i = 0; i < len; ++i)
    {
        const unsigned char c = (unsigned char) input[i];
        output.push_back(lut[c >> 4]);
        output.push_back(lut[c & 15]);
    }
    return output;
}

string Lexical::toBase64(string input){

    unsigned char * bytes_to_encode = (unsigned char *) input.c_str();
    int in_len = input.size();

    string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    while (in_len--) {
        char_array_3[i++] = *(bytes_to_encode++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for(i = 0; (i <4) ; i++)
                ret += base64Chars()[char_array_4[i]];
            i = 0;
        }
    }

    if (i)
    {
        for(j = i; j < 3; j++)
            char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (j = 0; (j < i + 1); j++)
            ret += base64Chars()[char_array_4[j]];

        while((i++ < 3))
            ret += '=';

    }

    return ret;
}

string Lexical::fromHexaDecimal(string input){

    static const char* const lut = "0123456789ABCDEF";
    size_t len = input.length();
    //if (len & 1) throw std::invalid_argument("odd length");

    std::string output;
    output.reserve(len / 2);
    for (size_t i = 0; i < len; i += 2)
    {
        char a = (char) input[i];
        const char* p = std::lower_bound(lut, lut + 16, a);
        //if (*p != a) throw std::invalid_argument("not a hex digit");

        char b = input[i + 1];
        const char* q = std::lower_bound(lut, lut + 16, b);
        //if (*q != b) throw std::invalid_argument("not a hex digit");

        output.push_back(((p - lut) << 4) | (q - lut));
    }

    return output;
}

string Lexical::fromBase64(string input){

    size_t in_len = input.size();
    size_t i = 0;
    size_t j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::string ret;

    while (in_len-- && ( input[in_] != '=') && isBase64(input[in_])) {
        char_array_4[i++] = input[in_]; in_++;
        if (i ==4) {
            for (i = 0; i <4; i++)
                char_array_4[i] = static_cast<unsigned char>(base64Chars().find(char_array_4[i]));

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; (i < 3); i++)
                ret += char_array_3[i];
            i = 0;
        }
    }

    if (i) {
        for (j = i; j <4; j++)
            char_array_4[j] = 0;

        for (j = 0; j <4; j++)
            char_array_4[j] = static_cast<unsigned char>(base64Chars().find(char_array_4[j]));

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
    }

    return ret;
}

void Lexical::setLength(int length) {

    this->length = length;
}

void Lexical::setPosition(int position) {

    this->position = position;
}

void Lexical::setInput(string input) {

    this->input = input;
}

void Lexical::setState(int state) {

    this->state->setCurrentState(state);
}

int Lexical::getLength() {

    return length;
}


int Lexical::getPosition() {

    return position;
}

string Lexical::getInput() {

    return input;
}

int Lexical::getState() {

    return state->getCurrentState();
}
