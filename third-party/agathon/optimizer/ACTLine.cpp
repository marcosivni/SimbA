#include "ACTLine.hpp"

ACTLine::ACTLine(){

    setAttribute("");
    setAttributeType(-1);
    setValue("");
    setValueType(-1);
}

ACTLine::~ACTLine(){

    parameters.clear();
}

void ACTLine::setAttribute(string attribute){

    this->attribute = attribute;
}

void ACTLine::setAttributeType(int dataType){

    this->attributeType = dataType;
}

void ACTLine::setOperator(string operattor){

    this->operattor = operattor;
}


void ACTLine::setValue(string value){

    this->value = value;
}

void ACTLine::setValueType(int valueType){

    this->valueType = valueType;
}

void ACTLine::setParameter(string key, string value){

    parameters.insert(pair<string, string> (key, value) );
}

string ACTLine::getAttribute(){

    return attribute;
}

int ACTLine::getAttributeType(){

    return attributeType;
}

string ACTLine::getOperator(){

    return operattor;
}

string ACTLine::getValue(){

    return value;
}

int ACTLine::getValueType(){

    return valueType;
}

//If .find(key) in maps makes no sense
//an empty string will be returned
string ACTLine::getParameter(string key){

    /*map < string, string > ::iterator it = parameters.find(key);
        if(it == map::end()) return "";
        else */
    return parameters.find(key)->second;
}

bool ACTLine::isEqual(ACTLine *actLine){

    return ( (getAttribute() == actLine->getAttribute()) &&
             (getAttributeType() == actLine->getAttributeType()) &&
             (getOperator() == actLine->getOperator()) &&
             (getValue() == actLine->getValue()) &&
             (getValueType() == actLine->getValueType()) );
}

void ACTLine::clearParameter(){

    parameters.clear();
}

string ACTLine::parameterToString(){

    string answer;

    if(parameters.size() > 0){
        for(std::map<string,string>::iterator it = parameters.begin(); it != parameters.end(); ++it){
            if (it != parameters.begin()){
                answer += ", ";
            }
            answer += it->first + "-" + it->second;
        }
    }

    return answer;
}
