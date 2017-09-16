#include "SyntacticDescAnalyzer.hpp"

SyntacticDescAnalyzer::~SyntacticDescAnalyzer(){

    if (dic != NULL){
        delete (dic);
    }
}

DataDictionary* SyntacticDescAnalyzer::dictionary(){

    return dic;
}


void SyntacticDescAnalyzer::table_name(){

    if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
        getParser()->match(getParser()->getCurrentToken());
        if (dictionary()->isValidTable(currentToken().getLexem())){
            getParser()->match(getParser()->getCurrentToken());
        } else {
            addError("Table '" + currentToken().getLexem() + "' does not exists.");
        }
    } else {
        addError("Identifier expected. '" + currentToken().getLexem() + "' found instead.");
    }
}

void SyntacticDescAnalyzer::metric_name(){

    if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
        if (dictionary()->isMetric(currentToken().getLexem())){
            getParser()->match(getParser()->getCurrentToken());
        } else {
            addError("Metric '" + currentToken().getLexem() + "' does not exists.");
        }
    } else {
        addError("Identifier expected. '" + currentToken().getLexem() + "' found instead.");
    }
}

void SyntacticDescAnalyzer::desc_specification(){

    try{
        if (currentToken().toUpperLexem() == "DESC"){
            getParser()->match(getParser()->getCurrentToken());
            if (currentToken().toUpperLexem() == "METRIC"){
                getParser()->match(getParser()->getCurrentToken());
                metric_name();
            } else {
                table_name();
            }
            if (!endOfStatement()){
                addError("Extra tokens at the end of statement: '" + currentToken().getLexem() + "'.");
            }
        } else {
            addError("DESC expected. '" + currentToken().getLexem() + "' found instead.");
        }
    } catch (...) {
        addError("Syntax error when processing DESC near '"+ getParser()->getToken(getParser()->countTokens()-1)->getLexem() +"'.");
    }
}

bool SyntacticDescAnalyzer::isValid(){

    clearErrors();

    desc_specification();
    getParser()->resetParser();

    return (!hasErrors());
}

