#include "SyntacticDropTableAnalyzer.hpp"

SyntacticDropTableAnalyzer::~SyntacticDropTableAnalyzer(){

    if (dic != NULL){
        delete (dic);
    }
}

void SyntacticDropTableAnalyzer::table_name(){

    if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
        if (dic->isValidTable(currentToken().getLexem())){
            getParser()->match(getParser()->getCurrentToken());
        } else {
            addError("Table '" + currentToken().getLexem() + "' does not exists.");
        }
    } else {
        addError("Identifier expected '" + currentToken().getLexem() + "' found instead.");
    }
}


void SyntacticDropTableAnalyzer::drop_table_specification(){

    try{
        if (currentToken().toUpperLexem() == "DROP"){
            getParser()->match(getParser()->getCurrentToken());
            if (currentToken().toUpperLexem() == "TABLE"){
                getParser()->match(getParser()->getCurrentToken());
                table_name();
                if (!endOfStatement()){
                    addError("Extra tokens at the end of statement: " + currentToken().getLexem() + "'.");
                }
            } else {
                addError("TABLE is expected. '" + currentToken().getLexem() + "' found instead.");
            }
        } else {
            addError("DROP TABLE statement is expected. '" + currentToken().getLexem() + "' found instead.");
        }
    } catch (...) {
        addError("Syntax error when processing DROP TABLE near '"+ getParser()->getToken(getParser()->countTokens()-1)->getLexem() +"'.");
    }
}

DataDictionary* SyntacticDropTableAnalyzer::dictionary(){

    return dic;
}

bool SyntacticDropTableAnalyzer::isValid(){

    clearErrors();

    drop_table_specification();
    getParser()->resetParser();

    return (!hasErrors());
}

