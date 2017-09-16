#include "SyntacticInsertIntoAnalyzer.hpp"

SyntacticInsertIntoAnalyzer::~SyntacticInsertIntoAnalyzer(){

    if (dic != NULL){
        delete (dic);
    }
}

void SyntacticInsertIntoAnalyzer::insert_column(){

    if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
        if (!dic->isValidColumn(getTableName(), currentToken().getLexem())){
            addError("Column '" + currentToken().getLexem() + "' does not exists in the table " + getTableName() + ".");
        } else {
            columns.push_back(currentToken().getLexem());
            getParser()->match(getParser()->getCurrentToken());
        }
    } else {
        addError("Identifier expected '" + currentToken().getLexem() + "' found instead.");
    }
}

void SyntacticInsertIntoAnalyzer::insert_column_list(){

    insert_column();
    if (currentToken().getTokenType() == Token::TK_COMMA) {
        getParser()->match(",", Token::TK_COMMA);
        insert_column_list();
    }
}

void SyntacticInsertIntoAnalyzer::table_name(){

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

void SyntacticInsertIntoAnalyzer::insert_value(){


    if ((currentToken().getTokenType() == Token::TK_INTEGER)  ||
        (currentToken().getTokenType() == Token::TK_FLOATING_POINT) ||
        (currentToken().getTokenType() == Token::TK_STRING)){
        values.push_back(currentToken().getLexem());
        getParser()->match(getParser()->getCurrentToken());
    } else {
        addError("Integer, floating point or string expected. '" + currentToken().getLexem() + "' found instead.");
    }
}

void SyntacticInsertIntoAnalyzer::insert_value_list(){

    insert_value();
    if (currentToken().getTokenType() == Token::TK_COMMA) {
        getParser()->match(",", Token::TK_COMMA);
        insert_value_list();
    }
}

void SyntacticInsertIntoAnalyzer::load_all_columns(){

    vector<string> cols =  dictionary()->getAllColumnsFromTable(getTableName());

    for (int x = 0; x < cols.size(); x++){
        columns.push_back(cols[x]);
        cols[x].clear();
    }
    cols.clear();
}

void SyntacticInsertIntoAnalyzer::insert_specification(){

    try{
        if (currentToken().toUpperLexem() == "INSERT"){
            getParser()->match(getParser()->getCurrentToken());
            if (currentToken().toUpperLexem() == "INTO"){
                getParser()->match(getParser()->getCurrentToken());
                table_name();
                if (currentToken().getLexem() == "("){
                    getParser()->match(getParser()->getCurrentToken());
                    insert_column_list();
                    getParser()->match(")", Token::TK_CLOSE_BRACE);
                } else {
                    load_all_columns();
                }
                if (currentToken().toUpperLexem() == "VALUES"){
                    getParser()->match(getParser()->getCurrentToken());
                    getParser()->match("(", Token::TK_OPEN_BRACE);
                    insert_value_list();
                    getParser()->match(")", Token::TK_CLOSE_BRACE);
                    if (!endOfStatement()){
                        addError("Extra tokens after the end of statement: '" + currentToken().getLexem() + "'.");
                    }
                } else {
                    addError("VALUES expected '" + currentToken().getLexem() + "' found instead");
                }
            } else {
                addError("INTO expected '" + currentToken().getLexem() + "' found instead.");
            }
        } else {
            addError("INSERT INTO statement expected '" + currentToken().getLexem() + "' found instead.");
        }
    } catch (...){
        addError("Syntax error when processing INSERT INTO near '"+ getParser()->getToken(getParser()->countTokens()-1)->getLexem() +"'.");
    }
}

string SyntacticInsertIntoAnalyzer::getTableName(){

    return tableName;
}

void SyntacticInsertIntoAnalyzer::checkColumnsAndValues(){

    if (columns.size() != values.size()){
        addError("Missing or extra values to insert.");
        return;
    }

    bool found = false;
    bool pkCheck = false;
    for (int x = 0; x < columns.size(); x++){
        if (dictionary()->isPrimaryKey(getTableName(), columns[x])){
            found = true;
            if (!dictionary()->isValidPrimaryKey(values[x], getTableName(), columns[x])){
                pkCheck = (pkCheck || false);
            } else {
                pkCheck = true;
            }
        }
    }

    if (!found){
        addError("Missing primary key value to insert.");
        return;
    }

    if (!pkCheck){
        addError("Duplicate entry for primary-key specification.");
        return;
    }

    vector < pair <string, bool> > nullable = dictionary()->getColumnsAndNullableFromTable(getTableName());

    for (int x = 0; x < nullable.size(); x++){
        if (!nullable[x].second){
            bool found = false;
            for (int y = 0; y < columns.size(); y++){
                if (columns[y] == nullable[x].first){
                    found = true;
                }
            }
            if (!found){
                addError("Column '" + nullable[x].first + "' is defined as not null. Please, insert a valid value.");
            }
        }
    }

    for (int x = 0; x < nullable.size(); x++){
        nullable[x].first.clear();
    }
    nullable.clear();

    for (int x = 0; x < columns.size(); x++){
        if (dictionary()->isComplexAttributeReference(getTableName(), columns[x])){
            try{
                string fileName, ax;
                ax = values[x];
                for (int y = 1; y < ax.size()-1; y++){
                    fileName += ax[y];
                }
                ax.clear();
                ComplexObject *cObject = new ComplexObject(fileName);
                delete (cObject);
            } catch (...) {
                addError("File: " + values[x] + " is not a valid SIREN complex type.");
            }
        }
    }
}

DataDictionary * SyntacticInsertIntoAnalyzer::dictionary(){

    return dic;
}

bool SyntacticInsertIntoAnalyzer::isValid(){

    clearErrors();

    insert_specification();

    checkColumnsAndValues();

    for (int x = 0; x < columns.size(); x++){
        columns[x].clear();
    }
    columns.clear();

    for (int x = 0; x < values.size(); x++){
        values[x].clear();
    }
    values.clear();
    tableName.clear();

    getParser()->resetParser();

    return (!hasErrors());
}
