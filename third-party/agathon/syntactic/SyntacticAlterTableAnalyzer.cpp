#include "SyntacticAlterTableAnalyzer.hpp"

SyntacticAlterTableAnalyzer::~SyntacticAlterTableAnalyzer(){

    if (dic != NULL){
        delete (dic);
    }
    tableName.clear();
}

void SyntacticAlterTableAnalyzer::table_name(){

    if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
        if (dictionary()->isValidTable(currentToken().getLexem())){
            tableName = currentToken().getLexem();
            getParser()->match(getParser()->getCurrentToken());
        } else {
            addError("Table '" + currentToken().getLexem() + "' does not exists.");
        }
    } else {
        addError("Identifier expected. '" + currentToken().getLexem() + "' found instead.");
    }
}

void SyntacticAlterTableAnalyzer::column_name(){

    if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
        if (dictionary()->isValidColumn(getTableName(), currentToken().getLexem())){
            if (dictionary()->isComplexAttributeReference(getTableName(), currentToken().getLexem())){
                addError("Add/modify a complex data-type column are not allowed.");
            } else {
                getParser()->match(getParser()->getCurrentToken());
            }
        } else {
            addError("Column '" + currentToken().getLexem() + "' does not exists in the table '" + getTableName() + "'.");
        }
    } else {
        addError("Identifier expected. '" + currentToken().getLexem() + "' found instead.");
    }
}

void SyntacticAlterTableAnalyzer::value_name(int op){

    if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
        if (op == 1){
            if (dictionary()->isValidDataType(currentToken().getLexem())){
                string dataType = currentToken().getLexem();
                getParser()->match(getParser()->getCurrentToken());

                if (dictionary()->isValidComplexDataType(dataType)){
                    addError("Add/modify a complex data-type column are not allowed.");
                }

                if ((!endOfStatement()) && (currentToken().getLexem() == "(")){
                    getParser()->match("(", Token::TK_OPEN_BRACE);
                    if (currentToken().getTokenType() == Token::TK_INTEGER){
                        getParser()->match(getParser()->getCurrentToken());
                    } else {
                        addError("Integer expected. '" + currentToken().getLexem() + "' found instead.");
                    }
                    getParser()->match(")", Token::TK_CLOSE_BRACE);
                }
            } else {
                addError("'" + currentToken().getLexem() + "' is not a valid data-type.");
            }
        } else {
            if (dictionary()->isValidColumn(getTableName(), currentToken().getLexem())){
                addError("Column '" + currentToken().getLexem() + "' already exists in the table '" + getTableName() + "'.");
            } else {
                getParser()->match(getParser()->getCurrentToken());
            }
        }
    } else {
        addError("Identifier expected. '" + currentToken().getLexem() + "' found instead.");
    }
}


void SyntacticAlterTableAnalyzer::command_name(){

    if (currentToken().toUpperLexem() == "ADD"){
        getParser()->match(getParser()->getCurrentToken());
        value_name(0);
        value_name(1);
    }
    if (currentToken().toUpperLexem() == "DROP"){
        getParser()->match(getParser()->getCurrentToken());
        if (currentToken().toUpperLexem() == "COLUMN"){
            getParser()->match(getParser()->getCurrentToken());
            column_name();
        } else {
            addError("COLUMN expected. '" + currentToken().getLexem() + "' found instead");
        }
    }
    if (currentToken().toUpperLexem() == "MODIFY"){
        getParser()->match(getParser()->getCurrentToken());
        if (currentToken().toUpperLexem() == "COLUMN"){
            getParser()->match(getParser()->getCurrentToken());
            column_name();
            value_name(1);
        } else {
            addError("COLUMN expected. '" + currentToken().getLexem() + "' found instead");
        }
    }
}

void SyntacticAlterTableAnalyzer::alter_table_specification(){

    try{
        if (currentToken().toUpperLexem() == "ALTER"){
            getParser()->match(getParser()->getCurrentToken());
            if (currentToken().toUpperLexem() == "TABLE"){
                getParser()->match(getParser()->getCurrentToken());
                table_name();
                command_name();
            } else {
                addError("TABLE expected. '" + currentToken().getLexem() + "' found instead.");
            }
        } else {
            addError("ALTER expected. '" + currentToken().getLexem() + "' found instead.");
        }

        if (!endOfStatement()){
            addError("Extra tokens at the end of statement: '" + currentToken().getLexem() + "'.");
        }
    } catch (...){
        addError("Syntax error when processing ALTER TABLE statement near '"+ getParser()->getToken(getParser()->countTokens()-1)->getLexem() +"'.");
    }
}

DataDictionary* SyntacticAlterTableAnalyzer::dictionary(){

    return dic;
}

string SyntacticAlterTableAnalyzer::getTableName(){

    return tableName;
}

bool SyntacticAlterTableAnalyzer::isValid(){

    clearErrors();

    alter_table_specification();
    getParser()->resetParser();

    return (!hasErrors());
}
