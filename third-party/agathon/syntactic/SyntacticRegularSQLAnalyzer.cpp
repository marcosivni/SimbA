#include "SyntacticRegularSQLAnalyzer.hpp"

SyntacticRegularSQLAnalyzer::SyntacticRegularSQLAnalyzer(DatabaseManager *dbmg){

    p = NULL;
    this->dbmg = NULL;
    this->dbmg = dbmg;
    this->expression = "";
}

SyntacticRegularSQLAnalyzer::SyntacticRegularSQLAnalyzer(string expression, DatabaseManager *dbmg){

    p = NULL;
    this->dbmg = NULL;
    this->dbmg = dbmg;
    setSQLExpression(expression);
}

SyntacticRegularSQLAnalyzer::~SyntacticRegularSQLAnalyzer(){

    clearErrors();
    expression.clear();

    if (p != NULL){
        delete (p);
    }
}

void SyntacticRegularSQLAnalyzer::clearErrors(){

    for (int x = 0; x < errors.size(); x++){
        errors[x].clear();
    }
    errors.clear();
}

void SyntacticRegularSQLAnalyzer::setSQLExpression(string sqlExpression){

    this->expression = sqlExpression;
    if (p != NULL){
        delete (p);
    }
    p = new Parser(sqlExpression);
}

void SyntacticRegularSQLAnalyzer::addError(string error){

    char buf[8];
    if (getParser()->getCurrentPosition() == -1)
        sprintf(buf, "%d", getParser()->countTokens()-1);
    else
        sprintf(buf, "%d", getParser()->getCurrentPosition()+1);
    string tmpBuf = buf;
    errors.push_back("[" + tmpBuf + "] " + error);
}

string SyntacticRegularSQLAnalyzer::getSQLExpression(){

    return expression;
}

Parser * SyntacticRegularSQLAnalyzer::getParser(){

    return p;
}

Token SyntacticRegularSQLAnalyzer::currentToken(){

    return *(p->getCurrentToken());
}

bool SyntacticRegularSQLAnalyzer::endOfStatement(){

    return (getParser()->getCurrentPosition() == -1);
}

bool SyntacticRegularSQLAnalyzer::hasErrors(){

    return (errors.size() > 0);
}

bool SyntacticRegularSQLAnalyzer::isValid(){

    return (!hasErrors());
}

vector<string> SyntacticRegularSQLAnalyzer::getErrors(){

    return errors;
}

string SyntacticRegularSQLAnalyzer::getError(int pos){

    if ((pos < countErrors()) && (pos >=0 ))
        return errors[pos];
    else
        throw new std::exception();
}

int SyntacticRegularSQLAnalyzer::countErrors(){

    return errors.size();
}
