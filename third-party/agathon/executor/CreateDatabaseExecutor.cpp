#include "CreateDatabaseExecutor.hpp"

CreateDatabaseExecutor::~CreateDatabaseExecutor(){

    if (dic != NULL){
        delete (dic);
    }
}

vector<string> CreateDatabaseExecutor::translate(){

    vector<string> answer;

    try{
        string old = dic->getDatabaseManager()->getDataBaseName();

        answer.push_back("CREATE DATABASE " + getParser()->getToken(2)->getLexem());
        answer.push_back(dic->useStatement(getParser()->getToken(2)->getLexem()));

        vector<string> ddl = dic->dataDictionaryInDDLDefinition();

        for (int x = 0; x < ddl.size(); x++){
            answer.push_back(ddl[x]);
            ddl[x].clear();
        }
        ddl.clear();

        answer.push_back(dic->useStatement(old));

    } catch (...) {
    }

    getParser()->resetParser();
    return answer;
}

bool CreateDatabaseExecutor::isExecutable(){

    return (translate().size() > 0);
}
