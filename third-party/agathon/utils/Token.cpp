#include "Token.hpp"

/**
* Empty constructor.
* </br>Usefull for token manipulations (inserts and removes).
* </br>This method generates a empty token (no lexem and
* undefined type).
*/
Token::Token() {

    lexem = "";
    tokenType = TK_UNDEFINED;
    lexemType = -2;
    scopePosition = -1;
}

/**
* Full constructor.
* </br>Sets all the abributtes of the token, unless the scope.
* </br> The default scope is -1 (undefined).
*
* @param lexem The token true text string.
* @param tokenType The type of the token (e.g. Identifier, operator, etc.).
* @param lexemType The type of lexem (e.g. if describes a column or a atributte).
*/
Token::Token(string lexem, int tokenType, int lexemType) {

    this->lexem = lexem;
    this->tokenType = tokenType;
    this->lexemType = lexemType;
    this->scopePosition = -1;
}

/**
* Full constructor.
* </br>Sets all the abributtes of the token.
*
* @param lexem The token true text string.
* @param tokenType The type of the token (e.g. Identifier, operator, etc.).
* @param lexemType The type of lexem (e.g. if describes a column or a atributte).
* @param scopePosition Defines the hierachic position of the token.
*/
Token::Token(string lexem, int tokenType, int lexemType, int scopePosition) {

    this->lexem = lexem;
    this->tokenType = tokenType;
    this->lexemType = lexemType;
    this->scopePosition = scopePosition;
}

/**
* Destructor.
*/
Token::~Token(){
}

/**
* Gets the token text.
*
* @return The token text string.
*/
string Token::getLexem() {

    return lexem;
}

/**
* Sets the token text.
*
* @param lexem The token text string.
*/
void Token::setLexem(string lexem) {

    this->lexem = lexem;
}

/**
* Gets the token lexem type.
* </br>Must be compared using Lexical static const public attributes.
* </br>e.g.: getLexemType() == Lexical::LK_TABLE_NAME
*
* @return The lexem token type.
*/
int Token::getLexemType() {

    return lexemType;
}

/**
* Sets the token lexem type.
*
* @param lexemType The token lexem type.
*/
void Token::setLexemType(int lexemType) {

    this->lexemType = lexemType;
}

/**
* Gets the token scope position.
*
* @return The token's scope position.
*/
int Token::getScopePosition() {

    return scopePosition;
}

/**
* Sets the token scope position.
*
* @param scopePosition The token's scope position.
*/
void Token::setScopePosition(int scopePosition) {

    this->scopePosition = scopePosition;
}

/**
* Gets the token type.
* </br>Must be compared using Token static const public attributes.
* </br>e.g.: getTokenType() == Token::TK_OPEN_BRACE
*
* @return The token type.
*/
int Token::getTokenType() {

    return tokenType;
}

/**
* Sets the token type.
*
* @param tokenType The token type.
*/
void Token::setTokenType(int tokenType) {

    this->tokenType = tokenType;
}

/**
* Returns a token text in capitals.
* </br>IMPORTANT: Does not modifies the original token text (lexem).
*
* @return The token text in capitals.
*/
string Token::toUpperLexem(){

    string aux = getLexem();
    char empty = ' ';
    string aux2;

    std::transform(aux.begin(), aux.end(), aux.begin(), ::toupper);

    if ((aux.size() != getLexem().size()) || (aux[aux.size()-1] == empty)){
        aux2 = aux;
        aux = "";
        for (int x = 0; x < aux2.size()-1; x++)
            aux += aux2[x];
    }

    return aux;
}

/**
* Appends the str parameter to the end of the lexem.
* This method is useful for Lex.
* @param str The string to be appended to the lexem.
*/
void Token::appendToLexem(string str){

	lexem = lexem.append(str);
}
