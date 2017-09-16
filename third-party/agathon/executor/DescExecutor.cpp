#include "DescExecutor.hpp"

DescExecutor::~DescExecutor(){

    if (dic != NULL){
        delete (dic);
    }
}

DataDictionary * DescExecutor::dictionary(){

    return dic;
}

vector<string> DescExecutor::translate(){

    vector<string> answer;

    try{
        getParser()->match(getParser()->getCurrentToken());
        if (currentToken().toUpperLexem() == "METRIC"){
            getParser()->match(getParser()->getCurrentToken());
            answer.push_back(dictionary()->descMetricCommand(currentToken().getLexem()));
        } else {
            getParser()->match(getParser()->getCurrentToken());
            answer.push_back(dictionary()->descTableCommand(currentToken().getLexem()));
        }
    } catch (...) {
    }

    getParser()->resetParser();

    return answer;
}

bool DescExecutor::isExecutable(){

    return (translate().size() > 0);
}

