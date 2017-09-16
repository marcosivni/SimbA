#include "SyntacticCreateIndexAnalyzer.hpp"

SyntacticCreateIndexAnalyzer::~SyntacticCreateIndexAnalyzer(){

    if (dic != NULL){
        delete (dic);
    }
    tableName.clear();
    indexName.clear();
    for (int x = 0; x < columnName.size(); x++){
        columnName[x].clear();
    }
    columnName.clear();
}

DataDictionary * SyntacticCreateIndexAnalyzer::getDictionary(){

    return dic;
}

string SyntacticCreateIndexAnalyzer::getTableName(){

    return tableName;
}

string SyntacticCreateIndexAnalyzer::getColumnName(int pos) throw (std::exception*){

    if (pos < columnName.size())
        return columnName[pos];
    else
        throw (new std::exception());
}

string SyntacticCreateIndexAnalyzer::getIndexName(){

    return indexName;
}

void SyntacticCreateIndexAnalyzer::index_name(){

    if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
        indexName = currentToken().getLexem();
        getParser()->match(getParser()->getCurrentToken());
    } else {
        addError("Identifier expected after CREATE INDEX. '" + currentToken().getLexem() + "' found instead.");
    }
}

void SyntacticCreateIndexAnalyzer::table_name(){

    if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
        if ((!(getDictionary()->isComplexAttributeReference(currentToken().getLexem(), getIndexName()))) &&
                ((getDictionary()->isComplexAttributeIndex(currentToken().getLexem(), getIndexName()) ||
                  (getDictionary()->isValidIndex(currentToken().getLexem(), getIndexName()))))){
            addError("Index '" + getIndexName() + "' over '" + currentToken().getLexem() + "' already exists!");
        } else {
            tableName = currentToken().getLexem();
            getParser()->match(getParser()->getCurrentToken());
        }
    } else {
        addError("Table name expected after ON. '" + currentToken().getLexem() + "' found instead.");
    }
}

void SyntacticCreateIndexAnalyzer::column_name(){

    bool first = true;
    while ((currentToken().getLexem() != ")") || (first)){
        first = false;
        if (currentToken().getTokenType() == Token::TK_COMMA){
            getParser()->match(getParser()->getCurrentToken());
        }
        if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
            if ((getDictionary()->isValidColumn(getTableName(), currentToken().getLexem())) ||
                    getDictionary()->isComplexAttributeReference(getTableName(), currentToken().getLexem())){
                columnName.push_back(currentToken().getLexem());
                getParser()->match(getParser()->getCurrentToken());
            } else {
                addError("Column " + currentToken().getLexem() + " does not exists in table " + getTableName() + ".");
                return;
            }
        } else {
            addError("Column name expected after '('. '" + currentToken().getLexem() + "' found instead.");
        }
    }
}

void SyntacticCreateIndexAnalyzer::metric_name(){

    if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
        if (getDictionary()->isMetric(currentToken().getLexem())){
            getParser()->match(getParser()->getCurrentToken());
        } else {
            addError(currentToken().getLexem() + " metric does not exists.");
        }
    } else {
        addError("Identifier expected after USING '" + currentToken().getLexem() + "' found instead.");
    }
}

void SyntacticCreateIndexAnalyzer::create_index_specification(){

    try{
        if (currentToken().toUpperLexem() == "CREATE"){
            getParser()->match(getParser()->getCurrentToken());
            if (currentToken().toUpperLexem() == "INDEX"){
                getParser()->match(getParser()->getCurrentToken());
                index_name();
                if (currentToken().toUpperLexem() == "ON"){
                    getParser()->match(getParser()->getCurrentToken());
                } else {
                    addError("ON clause expected, '" + currentToken().getLexem() + "' found instead.");
                }
                table_name();
                getParser()->match("(", Token::TK_OPEN_BRACE);
                column_name();
                getParser()->match(")", Token::TK_CLOSE_BRACE);

                for (int k = 0; ((k < columnName.size()) && (endOfStatement())); k++){
                    if(getDictionary()->isComplexAttributeReference(getTableName(), getColumnName(k))){
                        addError("'" + getColumnName(k) + "' is a complex attribute. Missing using <metric_name> clause.");
                    }
                }

                if ((!endOfStatement()) && (currentToken().toUpperLexem() == "USING")){
                    getParser()->match(getParser()->getCurrentToken());
                    if ((currentToken().toUpperLexem() == "BTREE") ||
                            (currentToken().toUpperLexem() == "HASH")){
                        getParser()->match(getParser()->getCurrentToken());
                    } else {
                        for (int k = 0; k < columnName.size(); k++){
                            if (!getDictionary()->isComplexAttributeReference(getTableName(), getColumnName(k))){
                                addError("'" + getColumnName(k) + "' is not a complex attribute.");
                            } else {
                                if (k > 0){
                                    addError("There are more than one column for complex index definition: " + getColumnName(k) + "'. Just one complex attribute at time allowed.");
                                }

                                if (getDictionary()->isValidMetric(currentToken().getLexem(), getTableName(), getColumnName(k))){
                                    addError("Complex indexes for metric " + currentToken().getLexem() + " already exists.");
                                } else {
                                    metric_name();
                                }
                            }
                        }
                    }
                }

                int count;
                for (int x = 0; x < columnName.size(); x++){
                    count = 0;
                    for (int y = 0; y < columnName.size(); y++){
                        if (getColumnName(x) == getColumnName(y))
                            count++;
                    }
                    if (count > 1){
                        addError("Duplicate column on index reference: '" + getColumnName(x) + "'.");
                        return;
                    }
                }

                if (!endOfStatement()){
                    addError("Extra tokens after the end of statement: '" + currentToken().getLexem() + "'.");
                }
            } else {
                addError("Missing INDEX after CREATE clause.");
            }
        } else {
            addError("Missing CREATE clause.");
        }
    } catch (...){
        addError("Syntax error when processing CREATE INDEX near '"+ getParser()->getToken(getParser()->countTokens()-1)->getLexem() +"'.");
    }
}

bool SyntacticCreateIndexAnalyzer::isValid(){

    clearErrors();

    create_index_specification();
    getParser()->resetParser();

    return (!hasErrors());
}
