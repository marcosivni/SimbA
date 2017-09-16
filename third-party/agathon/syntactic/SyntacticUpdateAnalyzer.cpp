#include "SyntacticUpdateAnalyzer.hpp"

void SyntacticUpdateAnalyzer::column_name(){

    if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
        if (dictionary()->isValidColumn(getTableName(), currentToken().getLexem())){
            if (dictionary()->isComplexAttributeReference(getTableName(), currentToken().getLexem())){
                addError("Update on complex data-type are not allowed.");
            } else {
                getParser()->match(getParser()->getCurrentToken());
            }
        } else {
            addError("Column '" + currentToken().getLexem() + "' does not exists in table '" + getTableName() + "'.");
        }
    } else {
        addError("Identifier expected '" + currentToken().getLexem() + "' found instead.");
    }
}

void SyntacticUpdateAnalyzer::value_name(){

    if ((currentToken().getTokenType() == Token::TK_STRING) ||
            (currentToken().getTokenType() == Token::TK_FLOATING_POINT) ||
            (currentToken().getTokenType() == Token::TK_INTEGER)){
        getParser()->match(getParser()->getCurrentToken());
    } else {
        addError("Invalid value for column '" + currentToken().getLexem() + "' found instead.");
    }
}

void SyntacticUpdateAnalyzer::update_reference(){

    column_name();
    getParser()->match("=", Token::TK_EQUAL);
    value_name();
}

void SyntacticUpdateAnalyzer::update_list(){

    update_reference();
    if (currentToken().getTokenType() == Token::TK_COMMA){
        getParser()->match(getParser()->getCurrentToken());
        update_list();
    }
}

void SyntacticUpdateAnalyzer::table_name(){

    if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
        if (dictionary()->isValidTable(currentToken().getLexem())){
            tableName = currentToken().getLexem();
            getParser()->match(getParser()->getCurrentToken());
        } else {
            addError("Table '" + currentToken().getLexem() + "' does not exists.");
        }
    } else {
        addError("Identifier expected '" + currentToken().getLexem() + "' found instead.");
    }
}

void SyntacticUpdateAnalyzer::where_clause(){

    if (currentToken().toUpperLexem() == "WHERE"){
        getParser()->match(getParser()->getCurrentToken());
        column_where_list();
    } else {
        addError("WHERE clause expected. '" + currentToken().getLexem() + "' found instead.");
    }
}

void SyntacticUpdateAnalyzer::column_where_list(){

    column_where();
    if ((!endOfStatement()) && ((currentToken().toUpperLexem() == "AND") || (currentToken().toUpperLexem() == "OR"))){
        getParser()->match(getParser()->getCurrentToken());
        column_where_list();
    }
}

void SyntacticUpdateAnalyzer::column_where(){

    column_name();

    if ((currentToken().getTokenType() == Token::TK_EQUAL) ||
            (currentToken().getTokenType() == Token::TK_NE) ||
            (currentToken().toUpperLexem() == "LIKE") ||
            (currentToken().getTokenType() == Token::TK_N2) ||
            (currentToken().getTokenType() == Token::TK_LQ) ||
            (currentToken().getTokenType() == Token::TK_LE) ||
            (currentToken().getTokenType() == Token::TK_GQ) ||
            (currentToken().getTokenType() == Token::TK_GE)){
        getParser()->match(getParser()->getCurrentToken());
        value_name();
    } else {
        if (currentToken().toUpperLexem() == "NOT"){
            getParser()->match(getParser()->getCurrentToken());
            if (currentToken().toUpperLexem() == "IN"){
                getParser()->match(getParser()->getCurrentToken());
                getParser()->match("(", Token::TK_OPEN_BRACE);
                value_name();
                while (currentToken().getTokenType() == Token::TK_COMMA){
                    getParser()->match(getParser()->getCurrentToken());
                    value_name();
                }
                getParser()->match(")", Token::TK_CLOSE_BRACE);
            } else {
                addError("IN expected. '" + currentToken().getLexem() + "' found instead.");
            }
        }
        if (currentToken().toUpperLexem() == "IN"){
            getParser()->match(getParser()->getCurrentToken());
            getParser()->match("(", Token::TK_OPEN_BRACE);
            value_name();
            while (currentToken().getTokenType() == Token::TK_COMMA){
                getParser()->match(getParser()->getCurrentToken());
                value_name();
            }
            getParser()->match(")", Token::TK_CLOSE_BRACE);
        } else {
            if (currentToken().toUpperLexem() == "BETWEEN"){
                value_name();
                if (currentToken().toUpperLexem() == "AND"){
                    getParser()->match(getParser()->getCurrentToken());
                    value_name();
                } else {
                    addError("AND expected. '" + currentToken().getLexem() + "' found instead.");
                }
            } else {
                addError("Predicate '" + currentToken().getLexem() + "' is not valid for UPDATE statement.");
            }
        }
    }
}

void SyntacticUpdateAnalyzer::update_specification(){

    try{
        if (currentToken().toUpperLexem() == "UPDATE"){
            getParser()->match(getParser()->getCurrentToken());
            table_name();
            if (currentToken().toUpperLexem() == "SET"){
                getParser()->match(getParser()->getCurrentToken());
                update_list();
                if ((!endOfStatement()) && (currentToken().toUpperLexem() == "WHERE")){
                    where_clause();
                } else {
                    addError("WHERE expected. '" + currentToken().getLexem() + "' found instead.");
                }
            } else {
                addError("SET expected. '" + currentToken().getLexem() + "' found instead.");
            }
        } else {
            addError("UPDATE expected. '" + currentToken().getLexem() + "' found instead.");
        }

        if (!endOfStatement()){
            addError("Extra tokens at the end of statement: '" + currentToken().getLexem() + "'.");
        }
    } catch (...){
        addError("Syntax error in UPDATE statement near '"+ getParser()->getToken(getParser()->countTokens()-1)->getLexem() +"'.");
    }
}

SyntacticUpdateAnalyzer::~SyntacticUpdateAnalyzer(){

    if (dic != NULL){
        delete (dic);
    }
    tableName.clear();
}

bool SyntacticUpdateAnalyzer::isValid(){

    clearErrors();

    update_specification();
    getParser()->resetParser();

    return (!hasErrors());
}

DataDictionary * SyntacticUpdateAnalyzer::dictionary(){

    return dic;
}

string SyntacticUpdateAnalyzer::getTableName(){

    return tableName;
}
