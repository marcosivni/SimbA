#include "SyntacticDropIndexAnalyzer.hpp"

DataDictionary * SyntacticDropIndexAnalyzer::getDictionary(){

    return dic;
}

string SyntacticDropIndexAnalyzer::getTableName(){

    return tableName;
}

void SyntacticDropIndexAnalyzer::index_name(){

    if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
        if ((!(getDictionary()->isComplexAttributeReference(getTableName(), currentToken().getLexem()))) &&
            ((getDictionary()->isComplexAttributeIndex(getTableName(), currentToken().getLexem()) ||
            (getDictionary()->isValidIndex(getTableName(), currentToken().getLexem()))))){
            getParser()->match(getParser()->getCurrentToken());
        } else {
            addError("Index '" + currentToken().getLexem() + "' does not exists.");
        }
    } else {
        addError("Identifier expected. '" + currentToken().getLexem() + "' found instead.");
    }
}

void SyntacticDropIndexAnalyzer::table_name(){

    if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
        if (getDictionary()->isValidTable(currentToken().getLexem())){
            tableName = currentToken().getLexem();
            getParser()->match(getParser()->getCurrentToken());
        } else {
            addError("Table '" + currentToken().getLexem() + "' does not exists!");
        }
    } else {
        addError("Identifier expected. '" + currentToken().getLexem() + "' found instead.");
    }
}

void SyntacticDropIndexAnalyzer::drop_index_specification(){

    try{
        if (currentToken().toUpperLexem() == "DROP"){
            getParser()->match(getParser()->getCurrentToken());
            if (currentToken().toUpperLexem() == "INDEX"){
                getParser()->match(getParser()->getCurrentToken());
                table_name();
                if ((!endOfStatement()) && (currentToken().getLexem() == ".")){
                    getParser()->match(getParser()->getCurrentToken());
                    index_name();
                } else {
                    addError("Syntax error. You should provide DROP INDEX 'table_name'.'index_name'.");
                }


                if (!endOfStatement()){
                    addError("Extra tokens after the end of statement: " + currentToken().getLexem());
                }

            } else {
                addError("INDEX clause missing for DROP INDEX statement.");
            }
        } else {
            addError("DROP clause missing for DROP INDEX statement.");
        }
    } catch (...){
        addError("Syntax error when processing CREATE INDEX statement near '"+ getParser()->getToken(getParser()->countTokens()-1)->getLexem() +"'.");
    }
}

bool SyntacticDropIndexAnalyzer::isValid(){

    clearErrors();

    drop_index_specification();
    getParser()->resetParser();

    return (!hasErrors());
}
