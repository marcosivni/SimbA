#include "CreateMetricSemanticStructs.hpp"

CheckExtractor::CheckExtractor(){
}

CheckExtractor::~CheckExtractor(){

    for (int x = 0; x < getSize(); x++){
        extractorList[x].first.clear();
        extractorList[x].second.clear();
    }
    extractorList.clear();

    for (int x = 0; x < parametersList.size(); x++){
        parametersList[x].first.clear();
        parametersList[x].second.clear();
    }
    parametersList.clear();

    for (int x = 0; x < weigth.size(); x++){
        weigth[x].clear();
    }
    weigth.clear();

    error.clear();
    dataType.clear();
}

void CheckExtractor::addParameter(string extractorName, string parameterName){

    pair<string, string> p(extractorName, parameterName);
    parametersList.push_back(p);
}

void CheckExtractor::addWeigth(string w){

    weigth.push_back(w);
}

void CheckExtractor::addExtractor(string extractorName, string extractorAlias){

    pair<string, string> aux(extractorName, extractorAlias);
    extractorList.push_back(aux);
}

void CheckExtractor::addExtractorAlias(string extractorAlias){

    if (getSize() > 0){
        extractorList[getSize()-1].second = extractorAlias;
    }
}

bool CheckExtractor::isValid(){

    int count = 0;
    for (int x = 0; x < extractorList.size(); x++){
        count = 0;
        for (int y = 0; y < extractorList.size(); y++){
            if (extractorList[x].first == extractorList[y].first){
                count++;
            }
        }
        if (count > 1){
            addError("Duplicate extractor: '" + extractorList[x].first + "'.");
            return false;
        }
    }
    for (int x = 0; x < extractorList.size(); x++){
        count = 0;
        for (int y = 0; y < extractorList.size(); y++){
            if ((!extractorList[x].second.empty()) && (extractorList[x].second == extractorList[y].second)){
                count++;
            }
        }
        if (count > 1){
            addError("Duplicate alias: '" + extractorList[x].second + "'.");
            return false;
        }
    }

    return true;
}

pair <string, string> CheckExtractor::getExtractor(int pos) throw (std::exception*){

    if (pos < getSize()){
        return extractorList[pos];
    } else {
        throw (new std::exception*);
    }
}

pair <string, string> CheckExtractor::getParameter(int pos) throw (std::exception*){

    if (pos < parametersList.size()){
        return parametersList[pos];
    } else {
        throw (new std::exception*);
    }
}

string CheckExtractor::getWeigth(int pos) throw (std::exception*){

    if (pos < weigth.size()){
        return weigth[pos];
    } else {
        throw (new std::exception*);
    }
}

int CheckExtractor::getSize(){

    return extractorList.size();
}

int CheckExtractor::getParametersSize(){

    return parametersList.size();
}

void CheckExtractor::addError(string error){

    if (this->error.size() > 0){
        this->error += "\n";
    }
    this->error += error;
}

string CheckExtractor::getError(){

    return error;
}

void CheckExtractor::setDataType(string dataType){

    this->dataType = dataType;
}

string CheckExtractor::getDataType(){

    return dataType;
}

string CheckExtractor::toString(){

    string answer = "EXTRACTOR || ALIAS \n";

    for (int x = 0; x < extractorList.size(); x++){
        answer += extractorList[x].first + " || ";
        answer += extractorList[x].second + "\n";
    }

    return answer;
}
