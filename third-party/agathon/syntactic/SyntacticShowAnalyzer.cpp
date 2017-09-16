#include "SyntacticShowAnalyzer.hpp"

SyntacticShowAnalyzer::~SyntacticShowAnalyzer(){

    if (dic != NULL){
        delete (dic);
    }
}

DataDictionary * SyntacticShowAnalyzer::dictionary(){

    return dic;
}

void SyntacticShowAnalyzer::show_specification(){

    try{
        if (currentToken().toUpperLexem() == "SHOW"){
            getParser()->match(getParser()->getCurrentToken());
            if ((currentToken().toUpperLexem() == "TABLES") ||
                (currentToken().toUpperLexem() == "EXTRACTORS") ||
                (currentToken().toUpperLexem() == "METRICS") ||
                (currentToken().toUpperLexem() == "DISTANCE_FUNCTIONS") ||
                (currentToken().toUpperLexem() == "DATABASES") ||
                (currentToken().toUpperLexem() == "INDEXES") ){
                getParser()->match(getParser()->getCurrentToken());
                if (!endOfStatement()){
                    addError("Extra tokens at the end of statement: '" + currentToken().getLexem() + "'.");
                }
            } else {
                addError("TABLES | DATABASES | EXTRACTORS | METRICS | DISTANCE_FUNCTIONS | INDEXES expected '" + currentToken().getLexem() + " found instead.");
            }
        } else {
            addError("SHOW expected '" + currentToken().getLexem() + " found instead.");
        }
    } catch (...) {
        addError("Syntax error when processing SHOW near '"+ getParser()->getToken(getParser()->countTokens()-1)->getLexem() +"'.");
    }
}

bool SyntacticShowAnalyzer::isValid(){

    clearErrors();

    show_specification();
    getParser()->resetParser();

    return (!hasErrors());
}

