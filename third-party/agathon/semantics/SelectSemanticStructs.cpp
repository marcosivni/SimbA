#include "SelectSemanticStructs.hpp"

CheckValidTables::CheckValidTables(DataDictionary *dic){

    this->dic = dic;
}

CheckValidTables::~CheckValidTables(){

    for (int x = 0; x < listTables.size(); x++){
        listTables[x].first.clear();
        listTables[x].second.clear();
    }
    listTables.clear();
    error.clear();
}

void CheckValidTables::addNewTable(string tableName, string aliasName){

    pair<string, string> aux(tableName, aliasName);
    listTables.push_back(aux);
}


void CheckValidTables::addNewTable(string tableName){

    pair<string, string> aux(tableName, tableName);
    listTables.push_back(aux);
}

void CheckValidTables::addNewAlias(string alias){

    if (listTables.size() > 0){
        listTables[listTables.size()-1].second = alias;
    }
}

void CheckValidTables::addError(string error){

    if (this->error.size() > 0){
        this->error += "\n";
    }

    this->error += error;
}

int CheckValidTables::countTableAliases(string tableName){

    int sum = 0;
    for (int x = 0; x < listTables.size(); x++){
        if (listTables[x].first == tableName){
            sum++;
        }
    }

    return sum;
}

int CheckValidTables::countTable(string alias){

    int sum = 0;
    for (int x = 0; x < listTables.size(); x++){
        if (listTables[x].second == alias){
            sum++;
        }
    }

    return sum;
}

bool CheckValidTables::isValid(){

    error.clear();

    for (int x = 0; x < listTables.size(); x++){
        if (!(dictionary()->isValidTable(listTables[x].first))){
            addError("Table '" + listTables[x].first + "' does not exists on the database.");
            return false;
        }
    }

    if (!isUniqueAliases()){
        return false;
    }

    string tableName;

    for (int x = 0; x < listTables.size(); x++){
        tableName = listTables[x].first;
    }

    return true;
}

bool CheckValidTables::isUniqueAliases(){

    error.clear();

    for (int x = 0; x < listTables.size(); x++){
        if (countTable(listTables[x].second) > 1){
            addError("Not unique table/alias: '" + listTables[x].first + "' is used for more than one table.");
            return false;
        }
    }

    return true;
}

string CheckValidTables::tableAlias(string tableName){

    string answer = "";
    for (int x = 0; x < listTables.size(); x++){
        if (listTables[x].first == tableName)
            return listTables[x].second;
    }

    return answer;
}

string CheckValidTables::tableName(string alias){

    string answer = "";
    for (int x = 0; x < listTables.size(); x++){
        if (listTables[x].second == alias)
            return listTables[x].first;
    }

    return answer;
}

DataDictionary * CheckValidTables::dictionary(){

    return dic;
}

int CheckValidTables::getSize(){

    return listTables.size();
}

pair<string, string> CheckValidTables::getTableNameAndAlias(int pos) throw (std::exception*){

    if (pos < getSize()){
        return listTables[pos];
    } else {
        throw new std::exception();
    }
}

string CheckValidTables::getError(){

    return error;
}

string CheckValidTables::toString(){

    string answer =  "TABLE || ALIAS \n";
    for (int x = 0; x < listTables.size(); x++){
        if (x > 0)
            answer += "\n";
        answer += listTables[x].first + " || " + listTables[x].second;
    }

    return answer;
}


CheckValidColumns::CheckValidColumns(DataDictionary *dic){

    this->dic = dic;
}

CheckValidColumns::~CheckValidColumns(){

    for (int x = 0; x < listColumns.size(); x++){
        listColumns[x].first.clear();
        listColumns[x].second.clear();
    }
    listColumns.clear();
    error.clear();
}

void CheckValidColumns::addNewColumn(string tableName, string columnName){

    pair<string, string> aux(tableName, columnName);
    listColumns.push_back(aux);
}

void CheckValidColumns::addNewTable(string tableName){

    pair<string, string> aux(tableName, "");
    listColumns.push_back(aux);
}

void CheckValidColumns::addNewColumn(string column){

    if ((listColumns.size() > 0) && (listColumns[listColumns.size()-1].second.empty())) {
        listColumns[listColumns.size()-1].second = column;
    } else {
        pair<string, string> aux("", column);
        listColumns.push_back(aux);
    }
}

void CheckValidColumns::addError(string error){

    if (this->error.size() > 0)
        this->error += "\n";
    this->error += error;
}

bool CheckValidColumns::isValid(CheckValidTables *validTables){

    if (!validTables->isValid()){
        addError(validTables->getError());
        return false;
    }

    for (int x = 0; x < listColumns.size(); x++){
        if (countAppearences(listColumns[x].first, listColumns[x].second, validTables) == 0){
            addError("Column '" + listColumns[x].second + "' does not exists in the specified tables.");
            return false;
        }
        if (countAppearences(listColumns[x].first, listColumns[x].second, validTables) > 1){
            addError("'" + listColumns[x].second + "' is an ambiguous column and/or defined in multiple tables.");
            return false;
        }
    }
    return true;
}

int CheckValidColumns::countAppearences(string tableAlias, string column, CheckValidTables *validTables){

    int count = 0;
    int cdup = 0;

    for (int x = 0; x < listColumns.size(); x++){
        if ((listColumns[x].second == column) && (listColumns[x].first.empty()))
           cdup++;
    }

    for (int x = 0; x < validTables->getSize(); x++){
        if ((tableAlias == validTables->getTableNameAndAlias(x).second) || (tableAlias.empty())){
            if (dictionary()->isValidColumn(validTables->getTableNameAndAlias(x).first, column)){
                count++;
            }
        }
    }

    if ((count == 1) && (cdup > 1))
        count = cdup;

    return count;
}

DataDictionary* CheckValidColumns::dictionary(){

    return dic;
}

int CheckValidColumns::getSize(){

    return listColumns.size();
}

pair<string, string> CheckValidColumns::getTableNameAndColumnName(int pos) throw (std::exception*){

    if (pos < getSize()){
        return listColumns[pos];
    } else {
        throw new std::exception();
    }
}

string CheckValidColumns::getError(){

    return error;
}

string CheckValidColumns::toString(){

    string answer =  "TABLE || COLUMN \n";
    for (int x = 0; x < listColumns.size(); x++){
        if (x > 0)
            answer += "\n";
        answer += listColumns[x].first + " || " + listColumns[x].second;
    }

    return answer;
}
