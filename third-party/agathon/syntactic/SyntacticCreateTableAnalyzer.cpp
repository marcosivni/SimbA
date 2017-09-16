#include "SyntacticCreateTableAnalyzer.hpp"

SyntacticCreateTableAnalyzer::~SyntacticCreateTableAnalyzer(){

    if (dic != NULL){
        delete (dic);
    }

    if (checkColumns != NULL) {
        delete (checkColumns);
    }

    if (checkConstraints != NULL) {
        delete (checkConstraints);
    }

    currentConstraint.clear();
}

void SyntacticCreateTableAnalyzer::table_name(){

    if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
        if (getParser()->getCurrentPosition() == 2){
            if (getDictionary()->isValidTable(currentToken().getLexem())){
                addError("Table '" + currentToken().getLexem() + "' already exists.");
            } else {
                getParser()->match(getParser()->getCurrentToken());
            }
        } else {
            if (!getDictionary()->isValidTable(currentToken().getLexem())){
                addError("Table '" + currentToken().getLexem() + "' does not exists.");
            } else {
                currentConstraint.setReferenceTableName(currentToken().getLexem());
                getParser()->match(getParser()->getCurrentToken());
            }
        }
    } else {
        addError("Identifier expected. ' " + currentToken().getLexem() + "' found.");
    }
}

void SyntacticCreateTableAnalyzer::column_definition(){

    if (currentToken().getTokenType() == Token::TK_IDENTIFIER) {
        checkColumns->addNewColumn(currentToken().getLexem());
        getParser()->match(getParser()->getCurrentToken());
    } else {
        addError("Identifier expected. '" + currentToken().getLexem() + "' found.");
    }

    data_type();

    if (currentToken().getTokenType() == Token::TK_IDENTIFIER)
        column_constraint_list();
}

void SyntacticCreateTableAnalyzer::column_constraint(){

    // Check the optional constraint name
    constraint_name();


    // If the constraint is 'NOT NULL'
    if (currentToken().toUpperLexem() == "NOT") {
        getParser()->match(getParser()->getCurrentToken());
        if (currentToken().toUpperLexem() == "NULL"){
            getParser()->match(getParser()->getCurrentToken());
        } else {
            addError("NULL expected. '" + currentToken().getLexem() + "' found instead.");
        }
    } else {
        // If the constraint is 'NULL' or 'UNIQUE'
        if (currentToken().toUpperLexem() == "NULL"){
            getParser()->match(getParser()->getCurrentToken());
        }  else {
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
                    if (currentToken().toUpperLexem() == "KEY"){
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
                        addError("KEY expected. '" + currentToken().getLexem() + "' found instead.");
                    }
                } else {
                    if (currentToken().toUpperLexem() == "FOREIGN") {
                        getParser()->match(getParser()->getCurrentToken());
                        if (currentToken().toUpperLexem() == "KEY"){
                            getParser()->match(getParser()->getCurrentToken());
                            if (currentToken().toUpperLexem() == "REFERENCES") {
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
                                } else {
                                    addError("'(' expected. '" + currentToken().getLexem() + "' found instead.");
                                }
                            }
                        } else {
                            addError("KEY expected. '" + currentToken().getLexem() + "' found instead.");
                        }
                    } else {
                        // constraint: METRIC [REFERENCES '(' <parameter_assoc_list> ")"] //references NOT
                        // USING '(' <metric_name> [default], <metric_name> ... ")"
                        if (currentToken().toUpperLexem() == "METRIC") {
                            getParser()->match(getParser()->getCurrentToken());
                            if (currentToken().toUpperLexem() == "USING"){
                                if (getParser()->getToken(getParser()->getCurrentPosition()-2)->getLexem() ==  ")"){
                                    currentConstraint.addAttribute(getParser()->getToken(getParser()->getCurrentPosition()-5)->getLexem());
                                } else {
                                    currentConstraint.addAttribute(getParser()->getToken(getParser()->getCurrentPosition()-3)->getLexem());
                                }
                                getParser()->match(getParser()->getCurrentToken());
                            } else {
                                addError("USING expected. '" + currentToken().getLexem() + "' found instead.");
                            }
                            getParser()->match("(", Token::TK_OPEN_BRACE);
                            metric_name_list();
                            getParser()->match(")", Token::TK_CLOSE_BRACE);
                            currentConstraint.setType(Constraint::METRIC);
                            checkConstraints->addNewConstraint(currentConstraint);
                            currentConstraint.clear();
                        } else {
                            addError("Constraint expected. '" + currentToken().getLexem() + "' found.");
                            // if it is an identifier then is wrong!
                            if (currentToken().getTokenType() == Token::TK_IDENTIFIER)
                                getParser()->match(getParser()->getCurrentToken());
                        }
                    }
                }
            }
        }
    }
}

void SyntacticCreateTableAnalyzer::column_name_list(int op){

    column_name(op);

    if (currentToken().getTokenType() == Token::TK_COMMA) {
        getParser()->match(",", Token::TK_COMMA);
        column_name_list(op);
    }
}

void SyntacticCreateTableAnalyzer::column_name(int op){

    if (currentToken().getTokenType() == Token::TK_IDENTIFIER) {
        //atribute normal
        if (op == 1){
            currentConstraint.addAttribute(currentToken().getLexem());
        } else {
            currentConstraint.addReferenceAttribute(currentToken().getLexem());
        }
        getParser()->match(getParser()->getCurrentToken());
    } else {
        addError("Identifier expected:  '" + currentToken().getLexem() + "' found.");
    }
}

void SyntacticCreateTableAnalyzer::metric_name(){

    string metricName = currentToken().getLexem();
    pair<string, bool> aux;

    if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
        if (getDictionary()->isMetric(currentToken().getLexem())){

            metricName = currentToken().getLexem();

            getParser()->match(getParser()->getCurrentToken());

            if ((!endOfStatement()) && (currentToken().toUpperLexem() == "DEFAULT")){
                aux.second = true;
            } else {
                aux.second = false;
            }

            aux.first = metricName;
            currentConstraint.addMetricName(aux);

        } else {
            addError("Metric '" + currentToken().getLexem() + "' does not exists.");
        }
    } else {
        addError("Metric name expected. " + currentToken().getLexem() + " found.");
    }
}

void SyntacticCreateTableAnalyzer::column_constraint_list(){

    column_constraint();

    if (currentToken().getTokenType() == Token::TK_IDENTIFIER) {
        column_constraint_list();
    }
}

void SyntacticCreateTableAnalyzer::table_constraint(){

    constraint_name();

    if (currentToken().toUpperLexem() == "PRIMARY") {
        getParser()->match(getParser()->getCurrentToken());
        if (getParser()->getCurrentToken()->toUpperLexem() == "KEY"){
            getParser()->match(getParser()->getCurrentToken());
        } else {
            addError("KEY expected. '" + currentToken().getLexem() + "' found instead.");
        }
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
                if (currentToken().toUpperLexem() == "KEY"){
                    getParser()->match(getParser()->getCurrentToken());
                } else {
                    addError("KEY expected. '" + currentToken().getLexem() + "' found instead.");
                }
                getParser()->match("(", Token::TK_OPEN_BRACE);
                column_name_list(1);
                getParser()->match(")", Token::TK_CLOSE_BRACE);
                if (currentToken().toUpperLexem() == "REFERENCES"){
                    getParser()->match(getParser()->getCurrentToken());
                } else {
                    addError("REFERENCES expected. '" + currentToken().getLexem() + "' found instead.");
                }
                currentConstraint.addAttribute(currentToken().getLexem());
                table_name();
                // Optional column name list
                if (currentToken().getLexem() == "(") {
                    getParser()->match("(", Token::TK_OPEN_BRACE);
                    column_name_list();
                    getParser()->match(")", Token::TK_CLOSE_BRACE);
                } else {
                    column_name_list();
                }
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


                    if (!checkColumns->isComplexColumn(currentToken().getLexem())){
                        addError("'" + currentToken().getLexem() + "' column is not defined as a complex data-type.");
                    } else {
                        currentConstraint.addAttribute(currentToken().getLexem());
                        getParser()->match(getParser()->getCurrentToken());
                    }

                    while (currentToken().getTokenType() == Token::TK_COMMA){
                        getParser()->match(getParser()->getCurrentToken());
                        if (!checkColumns->isComplexColumn(currentToken().getLexem())){
                            addError("'" + currentToken().getLexem() + "' column is not defined as a complex data-type.");
                            return;
                        } else {
                            currentConstraint.addAttribute(currentToken().getLexem());
                            getParser()->match(getParser()->getCurrentToken());
                        }
                    }

                    getParser()->match(")", Token::TK_CLOSE_BRACE);

                    if (currentToken().toUpperLexem() == "USING"){
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
                    } else {
                        addError("USING expected. '" + currentToken().getLexem() + "' found instead.");
                    }
                }
            }
        }
    }
}


void SyntacticCreateTableAnalyzer::table_element(){

    if ((currentToken().toUpperLexem() != "CONSTRAINT") && (currentToken().toUpperLexem() != "PRIMARY") && (currentToken().toUpperLexem() != "UNIQUE") && (currentToken().toUpperLexem() != "FOREIGN") && (currentToken().toUpperLexem() != "METRIC")){
        column_definition();
    } else {
        table_constraint();
    }
}

void SyntacticCreateTableAnalyzer::table_element_list(){

    table_element();

    if (currentToken().getTokenType() == Token::TK_COMMA) {
        getParser()->match(",", Token::TK_COMMA);
        table_element_list();
    }
}

void SyntacticCreateTableAnalyzer::data_type(){

    if (getDictionary()->isValidDataType(currentToken().toUpperLexem())){
        checkColumns->addNewDataType(currentToken().getLexem());
        getParser()->match(getParser()->getCurrentToken());
    } else {
        addError("Valid data type expected. '" + currentToken().getLexem() + "' found instead.");
    }

    // If next token is "(", an integer and an ")" are expected. E.g. varchar(20)
    if (currentToken().getTokenType() == Token::TK_OPEN_BRACE) {
        getParser()->match("(", Token::TK_OPEN_BRACE);
        if (currentToken().getTokenType() == Token::TK_INTEGER){
            getParser()->match(getParser()->getCurrentToken()->getLexem(), Token::TK_INTEGER);
        } else {
            addError("Integer expected. " + currentToken().getLexem() + " found.");
        }
        getParser()->match(")", Token::TK_CLOSE_BRACE);
    }
}

void SyntacticCreateTableAnalyzer::metric_name_list(){

    string metricName = currentToken().getLexem();

    metric_name();

    string tableName;
    tableName = getParser()->getToken(2)->getLexem();

    // Check if the metric exists
    if (!getDictionary()->isMetric(metricName)){
        addError("Invalid metric. '" + metricName + "' does not exists.");
    }

    // If there is an optional: DEFAULT
    if (currentToken().toUpperLexem() == "DEFAULT"){
        getParser()->match(getParser()->getCurrentToken());
    }

    if (currentToken().getTokenType() == Token::TK_COMMA) {
        getParser()->match(",", Token::TK_COMMA);
        metric_name_list();
    }
}

void SyntacticCreateTableAnalyzer::constraint_name(){

    if (currentToken().toUpperLexem() == "CONSTRAINT") {
        if (getDictionary()->isComplexAttributeIndex(currentToken().getLexem())){
            addError("Constraint '" + currentToken().getLexem() + "' already exists.");
        } else {
            getParser()->match(currentToken().getLexem(), Token::TK_IDENTIFIER);
            if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
                currentConstraint.setName(currentToken().getLexem());
                getParser()->match(getParser()->getCurrentToken());
            } else {
                addError("Identifier expected. '" + currentToken().getLexem() + "' found.");
            }
        }
    }
}

DataDictionary * SyntacticCreateTableAnalyzer::getDictionary(){

    return dic;
}

void SyntacticCreateTableAnalyzer::create_table_specification(){

    try{
        if (currentToken().toUpperLexem() == "CREATE"){
            getParser()->match(getParser()->getCurrentToken());
            if (currentToken().toUpperLexem() == "TABLE"){
                getParser()->match(getParser()->getCurrentToken());
            } else {
                addError("TABLE expected. '" + currentToken().getLexem() + "' found instead.");
            }

            table_name(); //match (<table_name>)
            getParser()->match("(", Token::TK_OPEN_BRACE);
            table_element_list(); //Check if the table structure are ok
            getParser()->match(")", Token::TK_CLOSE_BRACE);
            if (!endOfStatement()){
                addError("Extra tokens at the end of the statement: '" + currentToken().getLexem() + "'.");
            }
        } else {
            addError("CREATE TABLE expected. '" + currentToken().getLexem() + "' found instead.");
        }
    } catch (...) {
        addError("Syntax error when processing CREATE TABLE statement.");
    }
}

bool SyntacticCreateTableAnalyzer::isValid(){

    clearErrors();

    create_table_specification();

    if (!hasErrors()){
        if (!checkColumns->isValid()){
            addError(checkColumns->getError());
        } else {
            if (!checkConstraints->isValid(checkColumns)){
                addError(checkConstraints->getError());
            }
        }
    }

    delete (checkColumns);
    delete (checkConstraints);

    checkColumns = new CheckValidColumnsAndTypes(dic);
    checkConstraints = new CheckConstraints(dic);

    getParser()->resetParser();

    return (!hasErrors());
}
