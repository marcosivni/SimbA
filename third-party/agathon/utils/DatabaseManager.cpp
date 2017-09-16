#include "DatabaseManager.hpp"

DatabaseManager::DatabaseManager(){
}

DatabaseManager::DatabaseManager(string username, string password, string ip, int sgbd, string port,  string databaseName){

    setUsername(username);
    setPassword(password);
    setIp(ip);
    setSGBD(sgbd);
    setPassword(port);

    if (sgbd == this->MYSQL)
    {
        db = QSqlDatabase::addDatabase("QMYSQL");
    }
    else if (sgbd == this->ORACLE)
    {
        db = QSqlDatabase::addDatabase("QOCI");
    }
    else if (sgbd == this->POSTGRES)
    {
        db = QSqlDatabase::addDatabase("QPSQL");
    }
    else
    {
        throw new std::exception();
    }

    db.setHostName(getIp().c_str());
    db.setPort(atoi(port.c_str()));
    db.setDatabaseName(databaseName.c_str());
    db.setUserName(getUsername().c_str());
    db.setPassword(password.c_str());
    if (!db.open())
    {
        db.close();
        if (sgbd == this->MYSQL) db.removeDatabase("QMYSQL");
        else if (sgbd == this->ORACLE) db.removeDatabase("QOCI");
        else if (sgbd == this->POSTGRES) db.removeDatabase("QPSQL");
        throw new std::exception();
    }
}

bool DatabaseManager::isValid(){

    return db.open();
}

DatabaseManager::~DatabaseManager(){

    db.close();
}

void DatabaseManager::setUsername(string username){

    this->username = username;
}

void DatabaseManager::setPassword(string password){

    this->password = password;
}

void DatabaseManager::setPort(string port){

    this->port = port;
}

void DatabaseManager::setIp(string ip){

    this->ip = ip;
}

void DatabaseManager::setSGBD(int sgbd){

    if ((sgbd <= 3) || (sgbd >= 1)){
        this->sgbd = sgbd;
    } else {
        throw new std::exception();
    }
}

int DatabaseManager::getSGBD(){

    return sgbd;
}

string DatabaseManager::getIp(){

    return ip;
}

string DatabaseManager::getUsername(){

    return username;
}

string DatabaseManager::getPort(){

    return port;
}

string DatabaseManager::getDataBaseName(){

    return db.databaseName().toStdString();
}

string DatabaseManager::clearGarbageBlob(QByteArray array){

    string answer;

    for (int x = 0; x < array.size(); x++){
        if ((array.at(x) != '0') &&
                (array.at(x) != '1') &&
                (array.at(x) != '2') &&
                (array.at(x) != '3') &&
                (array.at(x) != '4') &&
                (array.at(x) != '5') &&
                (array.at(x) != '6') &&
                (array.at(x) != '7') &&
                (array.at(x) != '8') &&
                (array.at(x) != '9') &&
                (array.at(x) != 'A') &&
                (array.at(x) != 'B') &&
                (array.at(x) != 'C') &&
                (array.at(x) != 'D') &&
                (array.at(x) != 'E') &&
                (array.at(x) != 'F') &&
                (array.at(x) != 'x')){
        } else {
            answer += array.at(x);
        }
    }
    return answer;
}

vector< vector<string> > DatabaseManager::runSelect(string query){

    QSqlQuery q(db);
    QSqlRecord record;
    vector<string> aux;
    vector< vector<string> > answer;

    bool b = q.exec(query.c_str());

    if(!b)
    {
        cout << "\033[1;31m" << "deu pau... nao selec \033[0m" << endl;
        cout << q.lastError().text().toStdString() << endl;
        exit(1);
    }

    record = q.record();

    while(q.next()){
        for (int i = 0; i < record.count(); i++){
            aux.push_back(q.value(i).toString().toStdString());
        }
        answer.push_back(aux);
        aux.clear();

    }
    return answer;
}

vector<string> DatabaseManager::getFieldNames(string query){

    QSqlQuery q(db);
    QSqlRecord record;
    vector<string> answer;

    // @loc
    //q.prepare();
    q.exec(query.c_str());

    record = q.record();

    for (int x = 0; x < record.count(); x++)
        answer.push_back(record.fieldName(x).toStdString());

    return answer;
}

void DatabaseManager::close(){

    db.close();
}

int DatabaseManager::getNumberOfPossibleTuples(string query){

    if (runSelect("select count(*) as amount from (" + query + ") AS TMP").size() > 0){
        string aux = runSelect("select count(*) as amount from (" + query + ") AS TMP")[0][0];
        return atoi(aux.c_str());
    } else {
        return -1;
    }

}

string DatabaseManager::getEntireBlob(string queryId){

    QSqlQuery q(db);
    QSqlRecord record;
    QByteArray b;

    // @loc
    //q.prepare();
    q.exec(queryId.c_str());

    record = q.record();

    while(q.next()){
        for (int i = 0; i < record.count(); i++){
            b = q.value(i).toByteArray();
        }
    }

    return clearGarbageBlob(b);
}

bool DatabaseManager::runInsertStatement(string query){

    QSqlQuery q(db);

    bool b = q.exec(query.c_str());

    if(!b)
    {
        cout << "\033[1;31m" << "deu pau... nao exec \033[0m" << endl;
        cout << q.lastError().text().toStdString() << endl;
        exit(1);
    }

    return b;
}
