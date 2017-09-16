#include "SyntacticCreateDatabaseAnalyzer.hpp"

SyntacticCreateDatabaseAnalyzer::~SyntacticCreateDatabaseAnalyzer(){

    if (dic != NULL){
        delete (dic);
    }
}

DataDictionary * SyntacticCreateDatabaseAnalyzer::dictionary(){

    return dic;
}

bool SyntacticCreateDatabaseAnalyzer::isValid(){

    clearErrors();

    try{
        if (currentToken().toUpperLexem() == "CREATE"){
            getParser()->match(getParser()->getCurrentToken());
            if (currentToken().toUpperLexem() == "DATABASE"){
                getParser()->match(getParser()->getCurrentToken());
                if (dictionary()->isValidDatabase(currentToken().getLexem())){
                    addError("Database '" + currentToken().getLexem() + "' already exists.");
                }
            } else {
                addError("DATABASE expected. '" + currentToken().getLexem() + "' found instead.");
            }
        } else {
            addError("CREATE expected. '" + currentToken().getLexem() + "' found instead.");
        }
    } catch (...){
        addError("Syntax error when processing CREATE DATABASE near '"+ getParser()->getToken(getParser()->countTokens()-1)->getLexem() +"'.");
    }

    getParser()->resetParser();
    return (!hasErrors());
}
