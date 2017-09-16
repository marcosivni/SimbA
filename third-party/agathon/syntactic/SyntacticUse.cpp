#include "SyntacticUse.hpp"

SyntacticUse::~SyntacticUse(){

    if (dic != NULL){
        delete (dic);
    }
}

DataDictionary * SyntacticUse::dictionary(){

    return dic;
}

bool SyntacticUse::isValid(){

    clearErrors();

    try{
        if (currentToken().toUpperLexem() == "USE"){
            getParser()->match(getParser()->getCurrentToken());
            if (dictionary()->isValidDatabase(currentToken().getLexem())){
                getParser()->match(getParser()->getCurrentToken());
            } else {
                addError("'" + currentToken().getLexem() + "' is not a valid database name.");
            }
        } else {
            addError("Use clause expected. '" + currentToken().getLexem() + "' found instead.");
        }
        if (!endOfStatement()){
            addError("Extra tokens at the end of the statement.");
        }
    } catch (...) {
        addError("Syntax error when processing USE near '"+ getParser()->getToken(getParser()->countTokens()-1)->getLexem() +"'.");
    }

    getParser()->resetParser();
    return (!hasErrors());
}
