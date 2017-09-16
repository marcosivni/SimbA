#include "CreateTableSemanticStructs.hpp"

CheckValidColumnsAndTypes::CheckValidColumnsAndTypes(DataDictionary *dic){

    this->dic = dic;
}

CheckValidColumnsAndTypes::~CheckValidColumnsAndTypes(){


    for (int x = 0; x < listColumns.size(); x++){
        listColumns[x].first.clear();
        listColumns[x].second.clear();
    }
    listColumns.clear();
    error.clear();
}

void CheckValidColumnsAndTypes::setDataType(int pos, string dataType) throw (std::exception*){

    if (pos < getSize()){
        listColumns[pos].second = dataType;
    } else {
        throw (new std::exception());
    }
}

void CheckValidColumnsAndTypes::addNewColumn(string columnName, string dataType){

    pair<string, string> aux(columnName, dataType);
    listColumns.push_back(aux);
}

void CheckValidColumnsAndTypes::addNewColumn(string columnName){

    pair<string, string> aux(columnName, "");
    listColumns.push_back(aux);
}

void CheckValidColumnsAndTypes::addNewDataType(string dataType){

    if (listColumns.size() > 0){
        listColumns[listColumns.size()-1].second = dataType;
    }
}

void CheckValidColumnsAndTypes::addError(string error){

    if (this->error.size() > 0){
        this->error += "\n";
    }

    this->error += error;
}

bool CheckValidColumnsAndTypes::isComplexColumn(string columnName){

    for (int x = 0; x < listColumns.size(); x++){
        if (listColumns[x].first == columnName){
            return dictionary()->isValidComplexDataType(listColumns[x].second);
        }
    }
    return false;
}

int CheckValidColumnsAndTypes::countColumns(string columnName){

    int count = 0;
    for (int x = 0; x < listColumns.size(); x++){
        if (listColumns[x].first == columnName)
            count++;
    }

    return count;
}

bool CheckValidColumnsAndTypes::isValid(){

    if (!isUniqueColumnNames())
        return false;

    for (int x = 0; x < listColumns.size(); x++){
        if (!dictionary()->isValidDataType(listColumns[x].second)){
            addError("Invalid data-type: '" + listColumns[x].second + "'.");
            return false;
        }
    }

    return true;
}

bool CheckValidColumnsAndTypes::isUniqueColumnNames(){

    for (int x = 0; x < listColumns.size(); x++){
        if (countColumns(listColumns[x].first) != 1){
            addError("Duplicate column name: '" + listColumns[x].first + "'.");
            return false;
        }
    }

    return true;
}

string CheckValidColumnsAndTypes::dataType(string columnName){

    string answer = "NULL";
    for (int x = 0; x < listColumns.size(); x++){
        if (listColumns[x].first == columnName)
            return listColumns[x].second;
    }

    return answer;
}

DataDictionary* CheckValidColumnsAndTypes::dictionary(){

    return dic;
}

int CheckValidColumnsAndTypes::getSize(){

    return listColumns.size();
}

pair<string, string> CheckValidColumnsAndTypes::getColumnNameAndDataType(int pos) throw (std::exception*){

    if (pos < getSize()){
        return listColumns[pos];
    } else {
        throw new std::exception();
    }
}

string CheckValidColumnsAndTypes::getError(){

    return error;
}

string CheckValidColumnsAndTypes::toString(){

    string answer =  "COLUMN || DATA TYPE \n";
    for (int x = 0; x < listColumns.size(); x++){
        if (x > 0)
            answer += "\n";
        answer += listColumns[x].first + " || " + listColumns[x].second;
    }

    return answer;
}

Constraint::Constraint(){

    setType(-1);
}

Constraint::~Constraint(){

    name.clear();
    attributes.clear();
    referenceTableName.clear();
    usingMetricName.clear();
    referenceAttributes.clear();
}

void Constraint::clear(){

    name.clear();
    attributes.clear();
    referenceTableName.clear();
    usingMetricName.clear();
    referenceAttributes.clear();

    setType(-1);
}

void Constraint::setName(string name){

    this->name = name;
}

void Constraint::setType(unsigned char type){

    this->type = type;
}

void Constraint::addAttribute(string attribute){

    attributes.push_back(attribute);
}

void Constraint::setReferenceTableName(string referenceTableName){

    this->referenceTableName = referenceTableName;
}

void Constraint::addMetricName(pair<string, bool> usingMetricName){

    this->usingMetricName.push_back(usingMetricName);
}

void Constraint::addReferenceAttribute(string referenceAttribute){

    referenceAttributes.push_back(referenceAttribute);
}

bool Constraint::isValid(){

    if (!((getType() == Constraint::FOREIGN_KEY) ||
          (getType() == Constraint::PRIMARY_KEY) ||
          (getType() == Constraint::METRIC) ))
        return false;

    if (getType() == Constraint::PRIMARY_KEY){
        if ((!getReferenceTableName().empty()) ||
                (getReferenceAttributes().size() > 0) ||
                (getMetricsNames().size() > 0))
            return false;
    }

    if (getType() == Constraint::FOREIGN_KEY){
        if ((getReferenceTableName().empty()) ||
                (getReferenceAttributes().size() == 0) ||
                (getMetricsNames().size() > 0))
            return false;
    }

    if (getType() == Constraint::METRIC){
        if ((!getReferenceTableName().empty()) ||
                (getReferenceAttributes().size() < 0) ||
                (getMetricsNames().size() == 0))
            return false;
    }

}

bool Constraint::isEqual(Constraint constraint){

    bool answer = true;

    if ((name == constraint.getName()) &&
          (type == constraint.getType()) &&
          (referenceTableName == constraint.getReferenceTableName()) &&
          (attributes.size() == constraint.getAttributes().size()) &&
          (usingMetricName.size() == constraint.getMetricsNames().size()) &&
          (referenceAttributes.size() == constraint.getReferenceAttributes().size())){


        for (int x = 0; x < attributes.size(); x++){
            if (attributes[x] != constraint.getAttributes()[x])
                answer = false;
        }
        for (int x = 0; x < referenceAttributes.size(); x++){
            if (referenceAttributes[x] != constraint.getReferenceAttributes()[x])
                answer = false;
        }

        for (int x = 0; x < usingMetricName.size(); x++){
            if ((usingMetricName[x].first != constraint.getMetricsNames()[x].first) ||
                    (usingMetricName[x].second != constraint.getMetricsNames()[x].second))
                answer = false;
        }

    } else {
        answer = false;
    }

    return answer;
}

string Constraint::getName(){

    return name;
}

unsigned char Constraint::getType(){

    return type;
}

vector<string> Constraint::getAttributes(){

    return attributes;
}

string Constraint::getReferenceTableName(){

    return referenceTableName;
}

vector< pair<string, bool> > Constraint::getMetricsNames(){

    bool def = false;

    for (int x = 0; x < usingMetricName.size(); x++){
        if (usingMetricName[x].second)
            def = true;
    }
    if (!def){
        if (usingMetricName.size() > 0)
            usingMetricName[0].second = true;
    }
    return usingMetricName;
}

vector<string> Constraint::getReferenceAttributes(){

    return referenceAttributes;
}

CheckConstraints::CheckConstraints(DataDictionary *dic){

    this->dic = dic;
}

CheckConstraints::~CheckConstraints(){

    constraintList.clear();
    error.clear();
}

void CheckConstraints::addNewConstraint(Constraint constraint){

    constraintList.push_back(constraint);
}

void CheckConstraints::addError(string error){

    if (this->error.size() > 0){
        this->error += "\n";
    }
    this->error += error;

}

int CheckConstraints::countNames(string constraintName){

    int count = 0;
    for (int x = 0; x < constraintList.size(); x++){
        if ((!constraintName.empty()) && (constraintList[x].getName() == constraintName)){
            count++;
        }
    }

    return count;
}

int CheckConstraints::countMetrics(Constraint constraint){

    int count = 0;

    for (int x = 0; x < constraint.getMetricsNames().size(); x++){
        count = 0;
        for (int y = 0; y < constraint.getMetricsNames().size(); y++){
            if (constraint.getMetricsNames()[x].first == constraint.getMetricsNames()[y].first){
                count++;
            }
        }
        if (count > 1){
            return count;
        }
    }

    return count;
}

int CheckConstraints::countDefaults(Constraint constraint){

    int count = 0;

    for (int x = 0; x < constraint.getMetricsNames().size(); x++){
        if (constraint.getMetricsNames()[x].second){
            count++;
        }
        if (count > 1){
            return count;
        }
    }

    return count;
}

int CheckConstraints::countPrimaryKey(){

    int count = 0;
    for (int x = 0; x < constraintList.size(); x++){
        if (constraintList[x].getType() == Constraint::PRIMARY_KEY){
            count++;
        }
    }

    return count;
}

int CheckConstraints::countUnique(Constraint constraint){

    int count = 0;

    for (int x = 0; x < constraint.getAttributes().size(); x++){
        count = 0;
        for (int y = 0; y < constraint.getAttributes().size(); y++){
            if (constraint.getAttributes()[x] == constraint.getAttributes()[y]){
                count++;
            }
        }
        if (count > 1){
            return count;
        }
    }

    return count;
}

int CheckConstraints::countForeignKey(Constraint constraint){

    int count = 0;

    for (int x = 0; x < constraint.getReferenceAttributes().size(); x++){
        count = 0;
        for (int y = 0; y < constraint.getReferenceAttributes().size(); y++){
            if (constraint.getReferenceAttributes()[x] == constraint.getReferenceAttributes()[y]){
                count++;
            }
        }
        if (count > 1){
            return count;
        }
    }

    return count;
}

bool CheckConstraints::checkAttributes(CheckValidColumnsAndTypes *columnsAndTypes){

    for (int x = 0; x < constraintList.size(); x++){
        for (int y = 0; y < constraintList[x].getAttributes().size(); y++){
            bool found = false;
            for (int z = 0; z < columnsAndTypes->getSize(); z++){
                if (constraintList[x].getAttributes()[y] == columnsAndTypes->getColumnNameAndDataType(z).first){
                    found = true;
                }
            }
            if (!found){
                addError("Attribute: '" + constraintList[x].getAttributes()[y] + "' has not been defined and cannot be used in a constraint expression.");
                return false;
            }
        }
    }
    return true;
}

bool CheckConstraints::isValid(CheckValidColumnsAndTypes *columnsAndTypes){

    if (!checkAttributes(columnsAndTypes))
        return false;

    if (!isConstraints())
        return false;

    for (int x = 0; x < constraintList.size(); x++){

        Constraint aux = constraintList[x];
        int count = 0;

        for (int y = 0; y < constraintList.size(); y++){
            if (aux.isEqual(constraintList[y])){
                count++;
            }
        }

        if (count > 1){
            addError("Duplicate definition of constraint for '" + aux.getAttributes()[0] + "'.");
            return false;
        }

        if (aux.getType() == Constraint::FOREIGN_KEY){
            for (int y = 0; y < aux.getAttributes().size(); y++){
                for (int z = 0; z < columnsAndTypes->getSize(); z++){
                    if ((columnsAndTypes->getColumnNameAndDataType(z).first == aux.getAttributes()[y]) && (dictionary()->toUpper(columnsAndTypes->getColumnNameAndDataType(z).second) != dictionary()->getColumnDataType(aux.getReferenceTableName(), aux.getReferenceAttributes()[y]))){
                        addError("Invalid constraint reference for: '" + columnsAndTypes->getColumnNameAndDataType(z).first + "'. Data-type of the reference column should be: '" + columnsAndTypes->getColumnNameAndDataType(z).second + "'.");
                        return false;
                    }
                }
            }
        }

        if (aux.getType() == Constraint::METRIC){
            for (int z = 0; z < columnsAndTypes->getSize(); z++){
                for (int y = 0; y < aux.getMetricsNames().size(); y++){
                    if ((columnsAndTypes->getColumnNameAndDataType(z).first == aux.getAttributes()[y]) && (dictionary()->toUpper(columnsAndTypes->getColumnNameAndDataType(z).second) != dictionary()->getMetricType(dictionary()->getMetricCode(aux.getMetricsNames()[y].first)))){
                        addError("Metric '" + aux.getMetricsNames()[y].first + "' does not provide support to " + columnsAndTypes->getColumnNameAndDataType(z).second);
                        return false;
                    }
                }
            }
        }
    }

    return true;
}

//rem column_name
bool CheckConstraints::isConstraints(){

    if (countPrimaryKey() > 1){
        addError("Duplicate primary key.");
        return false;
    }

    if (countPrimaryKey() < 1){
        addError("You must specify a primary key.");
        return false;
    }

    for (int x = 0; x < constraintList.size(); x++){
        Constraint aux = constraintList[x];

        if (countNames(aux.getName()) > 1){
            addError("Duplicate name for constraint: '" + aux.getAttributes()[0] + "'.");
            return false;
        }

        if (countMetrics(aux) > 1){
            addError("Duplicate metric for attribute '" + aux.getAttributes()[0] + "'.");
            return false;
        }

        if (countDefaults(aux) > 1){
            addError("There are more than one default metric for '" + aux.getAttributes()[0] + "'.");
            return false;
        }

        if (countUnique(aux) > 1){
            addError("Duplicate unique columns for '" + aux.getAttributes()[0] + "'.");
            return false;
        }

        if (countForeignKey(aux) > 1){
            addError("Ambiguous definition of foreign key on attribute: '" + aux.getAttributes()[0] + "'.");
            return false;
        }
    }
    return true;
}

Constraint CheckConstraints::getConstraint(int pos)  throw (std::exception*){

    if (pos < getSize()){
        return constraintList[pos];
    } else {
        throw new std::exception();
    }
}

int CheckConstraints::getSize(){

    return constraintList.size();
}

string CheckConstraints::getError(){

    return error;
}

DataDictionary* CheckConstraints::dictionary(){

    return dic;
}

string CheckConstraints::toString(){


    string answer =  "CONSTRAINT NAME ||  TYPE || AT LIST || TB || AT REF || METRIC REF \n";
    for (int x = 0; x < constraintList.size(); x++){
        if (x > 0)
            answer += "\n";
        answer += constraintList[x].getName() + " || ";
        if (constraintList[x].getType() == Constraint::PRIMARY_KEY){
            answer += "PK";
        }
        if (constraintList[x].getType() == Constraint::FOREIGN_KEY){
            answer += "FK";
        }
        if (constraintList[x].getType() == Constraint::UNIQUE){
            answer += "UE";
        }
        if (constraintList[x].getType() == Constraint::METRIC){
            answer += "MC";
        }

        if (constraintList[x].getType() == Constraint::NOT_NULL){
            answer += "NN";
        }
        answer += " || ";

        for (int y = 0; y < constraintList[x].getAttributes().size(); y++){
            answer += constraintList[x].getAttributes()[y] + " - ";
        }
        answer += " || ";

        answer += constraintList[x].getReferenceTableName();
        answer += " || ";

        for (int y = 0; y < constraintList[x].getReferenceAttributes().size(); y++){
            answer += constraintList[x].getReferenceAttributes()[y] + " - ";
        }
        answer += " || ";

        for (int y = 0; y < constraintList[x].getMetricsNames().size(); y++){
            answer += constraintList[x].getMetricsNames()[y].first + " - ";
        }
    }

    return answer;
}
