#include "AlterTableExecutor.hpp"

AlterTableExecutor::~AlterTableExecutor(){

    if (dic != NULL){
        delete (dic);
    }
    tableName.clear();
}

DataDictionary* AlterTableExecutor::dictionary(){

    return dic;
}


vector<string> AlterTableExecutor::translate(){

    vector<string> answer;

    try{
        getParser()->match(getParser()->getCurrentToken());
        getParser()->match(getParser()->getCurrentToken());
        tableName = currentToken().getLexem();
        getParser()->match(getParser()->getCurrentToken());
        //command_name();
        if (currentToken().toUpperLexem() == "ADD"){
            getParser()->match(getParser()->getCurrentToken());
            string column = currentToken().getLexem();
            getParser()->match(getParser()->getCurrentToken());
            string type = currentToken().getLexem();
            answer.push_back("ALTER TABLE " + tableName + " ADD " + column + " " + type);
        }
        if (currentToken().toUpperLexem() == "DROP"){
            getParser()->match(getParser()->getCurrentToken());
            getParser()->match(getParser()->getCurrentToken());
            string column = currentToken().getLexem();
            if (dictionary()->isComplexAttributeReference(tableName, column)){
                answer.push_back("DROP TABLE " + dictionary()->getTablePrefix(dictionary()->getComplexAttributeReference(tableName, column)) + "$" + tableName + "_" + column);
                answer.push_back("DELETE FROM CDD$ComplexAttribute WHERE TableName = '" + tableName + "' AND ComplexAttribName='" + column + "'");
                answer.push_back("DELETE FROM CDD$ComplexAttribMetric WHERE TableName = '" + tableName + "' AND ComplexAttribName='" + column + "'");
            }
            answer.push_back("ALTER TABLE " + tableName + " DROP COLUMN " + column);
        }
        if (currentToken().toUpperLexem() == "MODIFY"){
            getParser()->match(getParser()->getCurrentToken());
            getParser()->match(getParser()->getCurrentToken());
            string column = currentToken().getLexem();
            getParser()->match(getParser()->getCurrentToken());
            string type = currentToken().getLexem();
            answer.push_back("ALTER TABLE " + tableName + " MODIFY COLUMN " + column);
        }


    } catch (...){
    }

    tableName.clear();
    getParser()->resetParser();

    return answer;
}

bool AlterTableExecutor::isExecutable(){

    return (translate().size() > 0);
}
