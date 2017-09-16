#include "UseExecutor.hpp"

UseExecutor::~UseExecutor(){

    if (dic != NULL){
        delete (dic);
    }
}

vector<string> UseExecutor::translate(){

    vector<string> answer;

    answer.push_back(dic->useStatement(getParser()->getToken(1)->getLexem()));

    return answer;
}

bool UseExecutor::isExecutable(){

    return (translate().size() > 0);
}
