#include "UpdateExecutor.hpp"

vector<string> UpdateExecutor::translate(){

    vector<string> answer;
    string aux;

    //Automatic
    while (!endOfStatement()){
        if (aux.size() > 0)
            aux += " ";
        aux += currentToken().getLexem();
        getParser()->match(getParser()->getCurrentToken());
    }

    answer.push_back(aux);
    aux.clear();

    getParser()->resetParser();

    return answer;
}

bool UpdateExecutor::isExecutable(){

    return (translate().size() > 0);
}
