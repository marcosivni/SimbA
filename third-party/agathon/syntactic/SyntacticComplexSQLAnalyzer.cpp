#include "SyntacticComplexSQLAnalyzer.hpp"

SyntacticComplexSQLAnalyzer::~SyntacticComplexSQLAnalyzer(){

    delete (select);
    delete (update);
    delete (deletee);
    delete (insert);
    delete (createTable);
    delete (createDatabase);
    delete (createIndex);
    delete (createMetric);
    delete (alter);
    delete (dropIndex);
    delete (dropMetric);
    delete (dropTable);
    delete (dropDatabase);
    delete (show);
    delete (desc);
    delete (use);
}


bool SyntacticComplexSQLAnalyzer::isValid(){

    clearErrors();
    vector<string> ws;

    try{
        if (currentToken().toUpperLexem() == "CREATE"){
            getParser()->match(getParser()->getCurrentToken());
            if (currentToken().toUpperLexem() == "DATABASE"){
                if (createDatabase->isValid()){
                    return true;
                } else {
                    ws = createDatabase->getErrors();
                    for (int x = 0; x < ws.size(); x++){
                        addError(ws[x]);
                    }
                }
            } else {
                if (currentToken().toUpperLexem() == "INDEX"){
                    if (createIndex->isValid()){
                        return true;
                    } else {
                        ws = createIndex->getErrors();
                        for (int x = 0; x < ws.size(); x++){
                            addError(ws[x]);
                        }
                    }
                } else {
                    if (currentToken().toUpperLexem() == "TABLE"){
                        if (createTable->isValid()){
                            return true;
                        } else {
                            ws = createTable->getErrors();
                            for (int x = 0; x < ws.size(); x++){
                                addError(ws[x]);
                            }
                        }
                    } else {
                        if (currentToken().toUpperLexem() == "METRIC"){
                            if (createMetric->isValid()){
                                return true;
                            } else {
                                ws = createMetric->getErrors();
                                for (int x = 0; x < ws.size(); x++){
                                    addError(ws[x]);
                                }
                            }
                        } else {
                            throw (new std::exception());
                        }
                    }
                }
            }
        } else{
            if (currentToken().toUpperLexem() == "DROP") {
                getParser()->match(getParser()->getCurrentToken());
                if (currentToken().toUpperLexem() == "INDEX"){
                    if (dropIndex->isValid()){
                        return true;
                    } else {
                        ws = dropIndex->getErrors();
                        for (int x = 0; x < ws.size(); x++){
                            addError(ws[x]);
                        }
                    }
                } else {
                    if (currentToken().toUpperLexem() == "METRIC"){
                        if (dropMetric->isValid()){
                            return true;
                        } else {
                            ws = dropMetric->getErrors();
                            for (int x = 0; x < ws.size(); x++){
                                addError(ws[x]);
                            }
                        }
                    } else {
                        if (currentToken().toUpperLexem() == "TABLE"){
                            if (dropTable->isValid()){
                                return true;
                            } else {
                                ws = dropTable->getErrors();
                                for (int x = 0; x < ws.size(); x++){
                                    addError(ws[x]);
                                }
                            }
                        } else {
                            if (currentToken().toUpperLexem() == "DATABASE"){
                                if (dropDatabase->isValid()){
                                    return true;
                                } else {
                                    ws = dropDatabase->getErrors();
                                    for (int x = 0; x < ws.size(); x++){
                                        addError(ws[x]);
                                    }
                                }
                            } else {
                                throw (new std::exception());
                            }
                        }
                    }
                }
            } else {
                if (currentToken().toUpperLexem() == "ALTER"){
                    if (alter->isValid()){
                        return true;
                    } else {
                        ws = alter->getErrors();
                        for (int x = 0; x < ws.size(); x++){
                            addError(ws[x]);
                        }
                    }
                } else {
                    if (currentToken().toUpperLexem() == "INSERT"){
                        if (insert->isValid()){
                            return true;
                        } else {
                            ws = insert->getErrors();
                            for (int x = 0; x < ws.size(); x++){
                                addError(ws[x]);
                            }
                        }
                    } else {
                        if (currentToken().toUpperLexem() == "DELETE"){
                            if (deletee->isValid()){
                                return true;
                            } else {
                                ws = deletee->getErrors();
                                for (int x = 0; x < ws.size(); x++){
                                    addError(ws[x]);
                                }
                            }
                        } else {
                            if (currentToken().toUpperLexem() == "UPDATE"){
                                if (update->isValid()){
                                    return true;
                                } else {
                                    for (int x = 0; x < update->countErrors(); x++){
                                        addError(update->getError(x));
                                    }
                                }
                            } else {
                                if (currentToken().toUpperLexem() == "SELECT"){
                                    if (select->isValid()){
                                        return true;
                                    } else {
                                        ws = select->getErrors();
                                        for (int x = 0; x < ws.size(); x++){
                                            addError(ws[x]);
                                        }
                                    }
                                } else {
                                    if (currentToken().toUpperLexem() == "SHOW"){
                                        if (show->isValid()){
                                            return true;
                                        } else {
                                            ws = show->getErrors();
                                            for (int x = 0; x < ws.size(); x++){
                                                addError(ws[x]);
                                            }
                                        }
                                    } else {
                                        if (currentToken().toUpperLexem() == "DESC"){
                                            if (desc->isValid()){
                                                return true;
                                            } else {
                                                ws = desc->getErrors();
                                                for (int x = 0; x < ws.size(); x++){
                                                    addError(ws[x]);
                                                }
                                            }
                                        } else {
                                            if (currentToken().toUpperLexem() == "USE"){
                                                if (use->isValid()){
                                                    return true;
                                                } else {
                                                    ws = use->getErrors();
                                                    for (int x = 0; x < ws.size(); x++){
                                                        addError(ws[x]);
                                                    }
                                                }
                                            } else {
                                                throw (new std::exception());
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

    } catch (...) {
        addError("'" + getSQLExpression() + "' is an invalid SQL expression.");
    }

    for (int x = 0; x < ws.size(); x++){
        ws[x].clear();
    }
    ws.clear();

    getParser()->resetParser();

    return (!hasErrors());
}

bool SyntacticComplexSQLAnalyzer::isSelect(){

    return (currentToken().toUpperLexem() == "SELECT");
}

bool SyntacticComplexSQLAnalyzer::isDML(){

    return ( (currentToken().toUpperLexem() == "INSERT") ||
             (currentToken().toUpperLexem() == "DELETE") ||
             (currentToken().toUpperLexem() == "UPDATE") ||
             (currentToken().toUpperLexem() == "SELECT") );

}

bool SyntacticComplexSQLAnalyzer::isDDL(){


    return ((currentToken().toUpperLexem() == "CREATE") ||
            (currentToken().toUpperLexem() == "DROP") ||
            (currentToken().toUpperLexem() == "ALTER") );
}

bool SyntacticComplexSQLAnalyzer::isDCL(){


    return ( (currentToken().toUpperLexem() == "SHOW") ||
             (currentToken().toUpperLexem() == "DESC") );
}

void SyntacticComplexSQLAnalyzer::setSQLExpression(string sqlExpression){

    SyntacticRegularSQLAnalyzer::setSQLExpression(sqlExpression);

    select->setSQLExpression(sqlExpression);
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
}
