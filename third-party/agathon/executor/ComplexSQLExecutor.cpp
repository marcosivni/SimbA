#include "ComplexSQLExecutor.hpp"

ComplexSQLExecutor::~ComplexSQLExecutor(){

    delete(update);
    delete(deletee);
    delete(insert);
    delete(createTable);
    delete(createDatabase);
    delete(createIndex);
    delete(createMetric);
    delete(alter);
    delete(dropIndex);
    delete(dropMetric);
    delete(dropTable);
    delete(dropDatabase);
    delete(show);
    delete(desc);
    delete(use);
    delete(syn);
    delete(sel);
}

bool ComplexSQLExecutor::isExecutable(){

    if (!syn->isValid())
        return false;

    return((update->isExecutable()) ||
           (deletee->isExecutable()) ||
           (insert->isExecutable()) ||
           (createTable->isExecutable()) ||
           (createDatabase->isExecutable()) ||
           (createIndex->isExecutable()) ||
           (createMetric->isExecutable()) ||
           (alter->isExecutable()) ||
           (dropIndex->isExecutable()) ||
           (dropMetric->isExecutable()) ||
           (dropTable->isExecutable()) ||
           (dropDatabase->isExecutable()) ||
           (show->isExecutable()) ||
           (use->isExecutable()) ||
           (desc->isExecutable()) ||
           (sel->isExecutable()) );
}

vector<string> ComplexSQLExecutor::translate(){

    vector<string> answer;

    getParser()->resetParser();

    if (isExecutable()){
        try{
            if (currentToken().toUpperLexem() == "CREATE"){
                getParser()->match(getParser()->getCurrentToken());
                if (currentToken().toUpperLexem() == "DATABASE"){
                    return createDatabase->translate();
                } else {
                    if (currentToken().toUpperLexem() == "INDEX"){
                        return createIndex->translate();
                    } else {
                        if (currentToken().toUpperLexem() == "TABLE"){
                            return createTable->translate();
                        } else {
                            return createMetric->translate();
                        }
                    }
                }
            } else {
                if (currentToken().toUpperLexem() == "DROP") {
                    getParser()->match(getParser()->getCurrentToken());
                    if (currentToken().toUpperLexem() == "INDEX"){
                        return dropIndex->translate();
                    } else {
                        if (currentToken().toUpperLexem() == "METRIC"){
                            return dropMetric->translate();
                        } else {
                            if (currentToken().toUpperLexem() == "TABLE"){
                                return dropTable->translate();
                            } else {
                                if (currentToken().toUpperLexem() == "DATABASE"){
                                    return dropDatabase->translate();
                                }
                            }
                        }
                    }
                } else {
                    if (currentToken().toUpperLexem() == "ALTER"){
                        return alter->translate();
                    } else {
                        if (currentToken().toUpperLexem() == "INSERT"){
                            return insert->translate();
                        } else {
                            if (currentToken().toUpperLexem() == "DELETE"){
                                return deletee->translate();
                            } else {
                                if (currentToken().toUpperLexem() == "UPDATE"){
                                    return update->translate();
                                } else {
                                    if (currentToken().toUpperLexem() == "SELECT"){
                                        return sel->translate();
                                    } else {
                                        if (currentToken().toUpperLexem() == "SHOW"){
                                            return show->translate();
                                        } else {
                                            if (currentToken().toUpperLexem() == "DESC"){
                                                return desc->translate();
                                            } else {
                                                return use->translate();
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        } catch (...){
        }
    }

    getParser()->resetParser();
    return answer;
}

void ComplexSQLExecutor::setSQLExpression(string sqlExpression){

    RegularSQLExecutor::setSQLExpression(sqlExpression);

    update->setSQLExpression(sqlExpression);
    deletee->setSQLExpression(sqlExpression);
    insert->setSQLExpression(sqlExpression);
    createTable->setSQLExpression(sqlExpression);
    createDatabase->setSQLExpression(sqlExpression);
    createIndex->setSQLExpression(sqlExpression);
    createMetric->setSQLExpression(sqlExpression);
    alter->setSQLExpression(sqlExpression);
    dropIndex->setSQLExpression(sqlExpression);
    dropMetric->setSQLExpression(sqlExpression);
    dropTable->setSQLExpression(sqlExpression);
    dropDatabase->setSQLExpression(sqlExpression);
    show->setSQLExpression(sqlExpression);
    desc->setSQLExpression(sqlExpression);
    use->setSQLExpression(sqlExpression);
    sel->setSQLExpression(sqlExpression);
    syn->setSQLExpression(sqlExpression);
}
