#include "SyntacticDropDatabaseAnalyzer.hpp"

SyntacticDropDatabaseAnalyzer::~SyntacticDropDatabaseAnalyzer(){

    if (dic != NULL){
        delete (dic);
    }
}

DataDictionary * SyntacticDropDatabaseAnalyzer::dictionary(){

    return dic;
}

bool SyntacticDropDatabaseAnalyzer::isValid(){

    clearErrors();

    try{
        if (currentToken().toUpperLexem() == "DROP"){
            getParser()->match(getParser()->getCurrentToken());
            if (currentToken().toUpperLexem() == "DATABASE"){
                getParser()->match(getParser()->getCurrentToken());
                if (!dictionary()->isValidDatabase(currentToken().getLexem())){
                    addError("Database '" + currentToken().getLexem() + "' does not exists.");
                }
            } else {
                addError("DATABASE expected. '" + currentToken().getLexem() + "' found instead.");
            }
        } else {
            addError("DROP expected. '" + currentToken().getLexem() + "' found instead.");
        }
    } catch (...){
        addError("Syntax error when processing CREATE DATABASE near '"+ getParser()->getToken(getParser()->countTokens()-1)->getLexem() +"'.");
    }

    getParser()->resetParser();
    return (!hasErrors());
}
