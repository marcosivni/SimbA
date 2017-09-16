#include "DropDatabaseExecutor.hpp"

vector<string> DropDatabaseExecutor::translate(){

    vector<string> answer;

    try{
        string aux = currentToken().getLexem(); //DROP
        getParser()->match(getParser()->getCurrentToken());
        aux += " " + currentToken().getLexem(); //DATABASE
        getParser()->match(getParser()->getCurrentToken());
        aux += " " + currentToken().getLexem(); //db name
        answer.push_back(aux);
        aux.clear();
    } catch (...) {
    }

    getParser()->resetParser();
    return answer;
}

bool DropDatabaseExecutor::isExecutable(){

    return (translate().size() > 0);
}
