#include "RegularSQLExecutor.hpp"

RegularSQLExecutor::RegularSQLExecutor(DatabaseManager *dbmg){

    p = NULL;
    this->dbmg = NULL;
    this->dbmg = dbmg;
    setSQLExpression("");
}

RegularSQLExecutor::RegularSQLExecutor(string expression, DatabaseManager *dbmg){

    this->p = NULL;
    this->dbmg = NULL;
    this->dbmg = dbmg;
    setSQLExpression(expression);
}

RegularSQLExecutor::~RegularSQLExecutor(){

    if (p != NULL){
        delete (p);
    }
}

void RegularSQLExecutor::setSQLExpression(string sqlExpression){

    this->exp.clear();
    Parser *j = new Parser(sqlExpression);
    delete(p);
    p = j;
    this->exp = sqlExpression;
}

string RegularSQLExecutor::getSQLExpression(){

    return exp;
}


Parser * RegularSQLExecutor::getParser(){

    return p;
}

Token RegularSQLExecutor::currentToken(){

    return *(p->getCurrentToken());
}

bool RegularSQLExecutor::isExecutable(){

    return true;
}

bool RegularSQLExecutor::endOfStatement(){

    return (getParser()->getCurrentPosition() == -1);
}
