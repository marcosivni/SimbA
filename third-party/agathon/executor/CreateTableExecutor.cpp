#include "CreateTableExecutor.hpp"

CreateTableExecutor::~CreateTableExecutor(){

    if (dic != NULL){
        delete (dic);
    }
}

DataDictionary* CreateTableExecutor::dictionary(){

    return dic;
}

void CreateTableExecutor::table_name(){

    if (getParser()->getCurrentPosition() == 2){
        getParser()->match(getParser()->getCurrentToken());
    } else {
        currentConstraint.setReferenceTableName(currentToken().getLexem());
        getParser()->match(getParser()->getCurrentToken());
    }
}

void CreateTableExecutor::column_definition(){

    checkColumns->addNewColumn(currentToken().getLexem());
    getParser()->match(getParser()->getCurrentToken());

    data_type();

    if (currentToken().getTokenType() == Token::TK_IDENTIFIER)
        column_constraint_list();
}

void CreateTableExecutor::column_constraint(){

    constraint_name();

    if (currentToken().toUpperLexem() == "NOT") {
        getParser()->match(getParser()->getCurrentToken());
        getParser()->match(getParser()->getCurrentToken());
        if (getParser()->getToken(getParser()->getCurrentPosition()-2)->getLexem() ==  ")"){
            currentConstraint.addAttribute(getParser()->getToken(getParser()->getCurrentPosition()-6)->getLexem());
        } else {
            currentConstraint.addAttribute(getParser()->getToken(getParser()->getCurrentPosition()-3)->getLexem());
        }
        currentConstraint.setType(Constraint::NOT_NULL);
        checkConstraints->addNewConstraint(currentConstraint);
        currentConstraint.clear();
    } else {
        if (currentToken().toUpperLexem() == "NULL"){
            getParser()->match(getParser()->getCurrentToken());
            if (getParser()->getToken(getParser()->getCurrentPosition()-2)->getLexem() ==  ")"){
                currentConstraint.addAttribute(getParser()->getToken(getParser()->getCurrentPosition()-5)->getLexem());
            } else {
                currentConstraint.addAttribute(getParser()->getToken(getParser()->getCurrentPosition()-3)->getLexem());
            }
            currentConstraint.setType(Constraint::NULL_);
            checkConstraints->addNewConstraint(currentConstraint);
            currentConstraint.clear();
        } else {
            if (currentToken().toUpperLexem() == "UNIQUE"){
                getParser()->match(getParser()->getCurrentToken());
                if (getParser()->getToken(getParser()->getCurrentPosition()-2)->getLexem() ==  ")"){
                    currentConstraint.addAttribute(getParser()->getToken(getParser()->getCurrentPosition()-5)->getLexem());
                } else {
                    currentConstraint.addAttribute(getParser()->getToken(getParser()->getCurrentPosition()-3)->getLexem());
                }
                currentConstraint.setType(Constraint::UNIQUE);
                checkConstraints->addNewConstraint(currentConstraint);
                currentConstraint.clear();
            } else {
                // If the constraint is PRIMARY KEY
                if (currentToken().toUpperLexem() == "PRIMARY") {
                    getParser()->match(getParser()->getCurrentToken());
                    getParser()->match(getParser()->getCurrentToken());
                    if (getParser()->getToken(getParser()->getCurrentPosition()-3)->getLexem() ==  ")"){
                        currentConstraint.addAttribute(getParser()->getToken(getParser()->getCurrentPosition()-6)->getLexem());
                    } else {
                        currentConstraint.addAttribute(getParser()->getToken(getParser()->getCurrentPosition()-4)->getLexem());
                    }
                    currentConstraint.setType(Constraint::PRIMARY_KEY);
                    checkConstraints->addNewConstraint(currentConstraint);
                    currentConstraint.clear();
                } else {
                    if (currentToken().toUpperLexem() == "FOREIGN") {
                        getParser()->match(getParser()->getCurrentToken());
                        getParser()->match(getParser()->getCurrentToken());
                        if (getParser()->getToken(getParser()->getCurrentPosition()-3)->getLexem() ==  ")"){
                            currentConstraint.addAttribute(getParser()->getToken(getParser()->getCurrentPosition()-6)->getLexem());
                        } else {
                            currentConstraint.addAttribute(getParser()->getToken(getParser()->getCurrentPosition()-4)->getLexem());
                        }
                        getParser()->match(getParser()->getCurrentToken());
                        table_name();
                        // optional column name list
                        if (currentToken().getLexem() == "(") {
                            getParser()->match("(", Token::TK_OPEN_BRACE);
                            column_name_list();
                            getParser()->match(")", Token::TK_CLOSE_BRACE);
                            currentConstraint.setType(Constraint::FOREIGN_KEY);
                            checkConstraints->addNewConstraint(currentConstraint);
                            currentConstraint.clear();
                        }
                    } else {
                        // constraint: METRIC [REFERENCES '(' <parameter_assoc_list> ")"] //references NOT
                        // USING '(' <metric_name> [default], <metric_name> ... ")"
                        if (currentToken().toUpperLexem() == "METRIC") {
                            getParser()->match(getParser()->getCurrentToken());
                            if (getParser()->getToken(getParser()->getCurrentPosition()-2)->getLexem() ==  ")"){
                                currentConstraint.addAttribute(getParser()->getToken(getParser()->getCurrentPosition()-5)->getLexem());
                            } else {
                                currentConstraint.addAttribute(getParser()->getToken(getParser()->getCurrentPosition()-3)->getLexem());
                            }
                            getParser()->match(getParser()->getCurrentToken());

                            getParser()->match("(", Token::TK_OPEN_BRACE);
                            metric_name_list();
                            getParser()->match(")", Token::TK_CLOSE_BRACE);
                            currentConstraint.setType(Constraint::METRIC);
                            checkConstraints->addNewConstraint(currentConstraint);
                            currentConstraint.clear();
                        }
                    }
                }
            }
        }
    }
}

void CreateTableExecutor::column_name_list(int op){

    column_name(op);

    if (currentToken().getTokenType() == Token::TK_COMMA) {
        getParser()->match(",", Token::TK_COMMA);
        column_name_list(op);
    }
}

void CreateTableExecutor::column_name(int op){

    if (op == 1){
        currentConstraint.addAttribute(currentToken().getLexem());
    } else {
        currentConstraint.addReferenceAttribute(currentToken().getLexem());
    }
    getParser()->match(getParser()->getCurrentToken());
}

void CreateTableExecutor::metric_name(){

    string metricName = currentToken().getLexem();
    pair<string, bool> aux;

    getParser()->match(getParser()->getCurrentToken());

    if ((!endOfStatement()) && (currentToken().toUpperLexem() == "DEFAULT")){
        aux.second = true;
    } else {
        aux.second = false;
    }

    aux.first = metricName;

    currentConstraint.addMetricName(aux);
}

void CreateTableExecutor::column_constraint_list(){

    column_constraint();

    if (currentToken().getTokenType() == Token::TK_IDENTIFIER) {
        column_constraint_list();
    }
}

void CreateTableExecutor::table_constraint(){

    constraint_name();

    if (currentToken().toUpperLexem() == "PRIMARY") {
        getParser()->match(getParser()->getCurrentToken());
        getParser()->match(getParser()->getCurrentToken());
        getParser()->match("(", Token::TK_OPEN_BRACE);
        column_name_list(1);
        getParser()->match(")", Token::TK_CLOSE_BRACE);
        currentConstraint.setType(Constraint::PRIMARY_KEY);
        checkConstraints->addNewConstraint(currentConstraint);
        currentConstraint.clear();
    } else {
        // Unique constraint definition: UNIQUE '(' <column_name_list> ")"
        if (currentToken().toUpperLexem() == "UNIQUE") {
            getParser()->match(getParser()->getCurrentToken());
            getParser()->match("(", Token::TK_OPEN_BRACE);
            column_name_list(1);
            getParser()->match(")", Token::TK_CLOSE_BRACE);
            currentConstraint.setType(Constraint::UNIQUE);
            checkConstraints->addNewConstraint(currentConstraint);
            currentConstraint.clear();
        } else {
            // Referential constraint definition: FOREIGN KEY
            // '(' <column_name_list> ")" REFERENCES <table> ['(' <column_name_list> ")"]
            if (currentToken().toUpperLexem() == "FOREIGN") {
                getParser()->match(getParser()->getCurrentToken());
                getParser()->match(getParser()->getCurrentToken());
                getParser()->match("(", Token::TK_OPEN_BRACE);
                column_name_list(1);
                getParser()->match(")", Token::TK_CLOSE_BRACE);
                getParser()->match(getParser()->getCurrentToken());
                currentConstraint.addAttribute(currentToken().getLexem());
                table_name();
                getParser()->match("(", Token::TK_OPEN_BRACE);
                column_name_list();
                getParser()->match(")", Token::TK_CLOSE_BRACE);
                currentConstraint.setType(Constraint::FOREIGN_KEY);
                checkConstraints->addNewConstraint(currentConstraint);
                currentConstraint.clear();
            } else {
                // metric constraint definition: METRIC {'(' <stillimage_column_name_list> ")"
                //   | '(' <audio_column_name_list> ")"}
                //   |  USING '(' <metric_name_list> ")"
                if (currentToken().toUpperLexem() == "METRIC") {
                    getParser()->match(getParser()->getCurrentToken());
                    getParser()->match("(", Token::TK_OPEN_BRACE);
                    currentConstraint.addAttribute(currentToken().getLexem());
                    getParser()->match(getParser()->getCurrentToken());

                    while (currentToken().getTokenType() == Token::TK_COMMA){
                        getParser()->match(getParser()->getCurrentToken());
                        currentConstraint.addAttribute(currentToken().getLexem());
                        getParser()->match(getParser()->getCurrentToken());
                    }

                    getParser()->match(")", Token::TK_CLOSE_BRACE);

                    getParser()->match(getParser()->getCurrentToken());
                    if (currentToken().getLexem() == "("){
                        getParser()->match("(", Token::TK_OPEN_BRACE);
                        metric_name_list();
                        getParser()->match(")", Token::TK_CLOSE_BRACE);
                    } else {
                        metric_name();
                    }
                    currentConstraint.setType(Constraint::METRIC);
                    checkConstraints->addNewConstraint(currentConstraint);
                    currentConstraint.clear();
                }
            }
        }
    }
}

string CreateTableExecutor::originalCreateTable()
{
    string answer = "CREATE TABLE " + getParser()->getToken(2)->getLexem() + " ( ";

    for (int x = 0; x < checkColumns->getSize(); x++){
        if (x > 0)
            answer += " , ";
        answer += " " + checkColumns->getColumnNameAndDataType(x).first;
        answer += " " + checkColumns->getColumnNameAndDataType(x).second + " ";

        bool notNull = false;
        bool null_ = false;

        for (int y = 0; y < checkConstraints->getSize(); y++){
            Constraint aux = checkConstraints->getConstraint(y);
            if (aux.getType() == Constraint::NOT_NULL){
                for (int z = 0; z < aux.getAttributes().size(); z++){
                    if (aux.getAttributes()[z] == checkColumns->getColumnNameAndDataType(x).first)
                        notNull = true;
                }
            }
            if (aux.getType() == Constraint::NULL_){
                for (int z = 0; z < aux.getAttributes().size(); z++){
                    if (aux.getAttributes()[z] == checkColumns->getColumnNameAndDataType(x).first)
                        null_ = true;
                }
            }
        }
        if (notNull){
            answer += "NOT NULL ";
        } else {
            if (null_)
                answer += "NULL ";
        }
    }

    for (int x = 0; x < checkConstraints->getSize(); x++){
        Constraint aux = checkConstraints->getConstraint(x);

        if (aux.getType() == Constraint::PRIMARY_KEY){
            answer += ", PRIMARY KEY ( ";
            for (int y = 0; y < aux.getAttributes().size(); y++){
                if (y > 0)
                    answer += ",";
                answer += aux.getAttributes()[y] + " ";
            }
            answer += ")";
        } else {
            if (aux.getType() == Constraint::UNIQUE){
                answer += ", UNIQUE ( ";
                for (int y = 0; y < aux.getAttributes().size(); y++){
                    if (y > 0)
                        answer += ",";
                    answer += aux.getAttributes()[y] + " ";
                }
                answer += ")";
            } else {
                if (aux.getType() == Constraint::FOREIGN_KEY){
                    answer += ", FOREIGN KEY ( ";
                    for (int y = 0; y < aux.getAttributes().size(); y++){
                        if (y > 0)
                            answer += ",";
                        answer += aux.getAttributes()[y] + " ";
                    }
                    answer += ") REFERENCES " + aux.getReferenceTableName() + "( ";
                    for (int y = 0; y < aux.getReferenceAttributes().size(); y++){
                        if (y > 0)
                            answer += ",";
                        answer += aux.getReferenceAttributes()[y] + " ";
                    }
                    answer += ")";
                }
            }
        }
    }

    answer += " )";

    return answer;
}

string CreateTableExecutor::additionalCreateTable(string table, string column, string dataType, Constraint metric){

    string answer = "";

    // @loc
    if(dbmg->getSGBD() == DatabaseManager::POSTGRES)
    {
        answer += "CREATE TABLE \"" + dictionary()->getTablePrefix(dataType);
        answer += "$" + table + "_" + column + "\" ( " + column + "_id INTEGER, ";
    }
    else
    {
        answer += "CREATE TABLE " + dictionary()->getTablePrefix(dataType);
        answer += "$" + table + "_" + column + " ( " + column + "_id INTEGER, ";
    }

    // @loc
    if(dbmg->getSGBD() == DatabaseManager::POSTGRES)
    {
        answer += column + " BYTEA ";
    }
    else
    {
        answer += column + " BLOB ";
    }

    vector<string> columns;
    vector< pair<string, string> > aux;

    for (int x = 0; x < metric.getMetricsNames().size(); x++){
        aux = dictionary()->getParameters(dictionary()->getMetricCode(metric.getMetricsNames()[x].first));
        for (int y = 0; y < aux.size(); y++){
            columns.push_back(aux[y].first);
        }
    }

    if (columns.size() > 0){
        answer += ", ";
    }

    for (int x = 0; x < columns.size(); x++){
        if (x > 0){
            answer += ", ";
        }

        // @loc
        if(dbmg->getSGBD() == DatabaseManager::POSTGRES)
        {
            answer += " " + columns[x] + " BYTEA ";
        }
        else
        {
            answer += " " + columns[x] + " BLOB ";
        }
    }

    answer += ", PRIMARY KEY (" + column + "_id) ";
    answer += ", FOREIGN KEY (" + column + "_id) REFERENCES " + table;
    answer += " (" + column + ")";
    answer += " )";

    return answer;
}

string CreateTableExecutor::additionalInsertIntoComplexAttribute(string column, string table, string dataType){

    string strSql = "";

    // @loc
    if(dbmg->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "INSERT INTO \"CDD$ComplexAttribute\" VALUES ( '" + table + "' , '" + column + "' , '" + dataType + "' )";
    }
    else
    {
        strSql = "INSERT INTO CDD$ComplexAttribute VALUES ( '" + table + "' , '" + column + "' , '" + dataType + "' )";
    }

    return strSql;
}

vector<string> CreateTableExecutor::additionalInsertIntoComplexAttibMetric(Constraint constraint, string column, string table){

    string insert;
    vector<string> answer;
    for (int x = 0; x < constraint.getMetricsNames().size(); x++){
        string cid = dictionary()->getNewIndexId();
        // @loc
        if(dbmg->getSGBD() == DatabaseManager::POSTGRES)
        {
            insert = "INSERT INTO \"CDD$ComplexAttribMetric\" VALUES ( '";
        }
        else
        {
            insert = "INSERT INTO CDD$ComplexAttribMetric VALUES ( '";
        }

        insert += table + "', '" + column + "', ";
        insert += dictionary()->getMetricCode(constraint.getMetricsNames()[x].first) + ", ";
        if (constraint.getMetricsNames()[x].second)
            insert += "'Y', '";
        else
            insert += "'N', '";
        insert += "index" + cid + "', '";
        insert += "metrictree" + cid + ".dat' )";
        answer.push_back(insert);
        insert.clear();
    }

    return answer;
}

void CreateTableExecutor::table_element(){

    if ((currentToken().toUpperLexem() != "CONSTRAINT") && (currentToken().toUpperLexem() != "PRIMARY") && (currentToken().toUpperLexem() != "UNIQUE") && (currentToken().toUpperLexem() != "FOREIGN") && (currentToken().toUpperLexem() != "METRIC")){
        column_definition();
    } else {
        table_constraint();
    }
}

void CreateTableExecutor::table_element_list(){

    table_element();

    if (currentToken().getTokenType() == Token::TK_COMMA) {
        getParser()->match(",", Token::TK_COMMA);
        table_element_list();
    }
}

void CreateTableExecutor::data_type(){

    string dType = currentToken().getLexem();
    getParser()->match(getParser()->getCurrentToken());

    // If next token is "(", an integer and an ")" are expected. E.g. varchar(20)
    if (currentToken().getTokenType() == Token::TK_OPEN_BRACE) {
        dType += " ( ";
        getParser()->match("(", Token::TK_OPEN_BRACE);
        dType += currentToken().getLexem();
        getParser()->match(getParser()->getCurrentToken()->getLexem(), Token::TK_INTEGER);
        getParser()->match(")", Token::TK_CLOSE_BRACE);
        dType += " )";
    }

    checkColumns->addNewDataType(dType);
}

void CreateTableExecutor::metric_name_list(){

    metric_name();

    string tableName;
    tableName = getParser()->getToken(2)->getLexem();

    if (currentToken().toUpperLexem() == "DEFAULT"){
        getParser()->match(getParser()->getCurrentToken());
    }

    if (currentToken().getTokenType() == Token::TK_COMMA) {
        getParser()->match(",", Token::TK_COMMA);
        metric_name_list();
    }
}

void CreateTableExecutor::constraint_name(){

    if (currentToken().toUpperLexem() == "CONSTRAINT") {
        getParser()->match(currentToken().getLexem(), Token::TK_IDENTIFIER);
        currentConstraint.setName(currentToken().getLexem());
        getParser()->match(getParser()->getCurrentToken());
    }
}

vector<string> CreateTableExecutor::translate(){

    vector<string> answer;
    try{
        getParser()->match(getParser()->getCurrentToken());
        getParser()->match(getParser()->getCurrentToken());
        getParser()->match(getParser()->getCurrentToken());
        getParser()->match("(", Token::TK_OPEN_BRACE);
        table_element_list();
        getParser()->match(")", Token::TK_CLOSE_BRACE);

        //Create internal tables for complex data
        for (int x = 0; x < checkColumns->getSize(); x++){
            if (dictionary()->isValidComplexDataType(checkColumns->getColumnNameAndDataType(x).second)){
                for (int y = 0; y < checkConstraints->getSize(); y++){
                    Constraint aux = checkConstraints->getConstraint(y);
                    if (aux.getType() == Constraint::METRIC){
                        for (int z = 0; z < aux.getAttributes().size(); z++){
                            if (checkColumns->getColumnNameAndDataType(x).first == aux.getAttributes()[z]){
                                if (!dictionary()->isComplexAttributeReference(getParser()->getToken(2)->getLexem(), checkColumns->getColumnNameAndDataType(x).first)){
                                    answer.push_back(additionalInsertIntoComplexAttribute(checkColumns->getColumnNameAndDataType(x).first, getParser()->getToken(2)->getLexem(), checkColumns->getColumnNameAndDataType(x).second));
                                }

                                vector<string> insert = additionalInsertIntoComplexAttibMetric(aux, checkColumns->getColumnNameAndDataType(x).first, getParser()->getToken(2)->getLexem());
                                for (int k = 0; k < insert.size(); k++){
                                    answer.push_back(insert[k]);
                                    insert[k].clear();
                                }
                                insert.clear();

                                answer.push_back(additionalCreateTable(getParser()->getToken(2)->getLexem(), checkColumns->getColumnNameAndDataType(x).first, checkColumns->getColumnNameAndDataType(x).second, aux));
                            }
                        }
                    }
                }
                // @loc: unique necessario devido a fk na tabela $
                checkColumns->setDataType(x, "INTEGER UNIQUE");
                //checkColumns->setDataType(x, "INTEGER");
                currentConstraint.clear();
                currentConstraint.addAttribute(checkColumns->getColumnNameAndDataType(x).first);
                currentConstraint.setType(Constraint::NOT_NULL);
                checkConstraints->addNewConstraint(currentConstraint);
                currentConstraint.clear();
            }
        }

        //Criar inserts da métrica adicional por constaint do tipo metric
        //answer.push_back(originalCreateTable());
        // @loc: insert original primeiro de tudo, devido a fk na tabela $
        answer.insert(answer.begin(), originalCreateTable());

        delete (checkColumns);
        delete (checkConstraints);

        checkColumns = new CheckValidColumnsAndTypes(dic);
        checkConstraints = new CheckConstraints(dic);

        getParser()->resetParser();

    } catch (...) {
    }

    return answer;
}

bool CreateTableExecutor::isExecutable(){

    return (translate().size() > 0);
}
