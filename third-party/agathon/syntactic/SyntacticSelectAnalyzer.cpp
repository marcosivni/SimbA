#include "SyntacticSelectAnalyzer.hpp"

SyntacticSelectAnalyzer::~SyntacticSelectAnalyzer(){

    if (listTables != NULL){
        delete (listTables);
    }
    if (listColumns != NULL){
        delete (listColumns);
    }
    if (dic != NULL){
        delete (dic);
    }
}

DataDictionary * SyntacticSelectAnalyzer::getDictionary(){

    return dic;
}

// tie_list ::= TIE LIST
void SyntacticSelectAnalyzer::tie_list(){

    if (currentToken().toUpperLexem() == "TIE") {
        getParser()->match(getParser()->getCurrentToken());
        if (currentToken().toUpperLexem() == "LIST") {
            getParser()->match(getParser()->getCurrentToken());
        } else {
            addError("'LIST' expected. '" + currentToken().getLexem() + "' found instead.");
        }
    } else {
        addError("'TIE' expected. '" + currentToken().getLexem() + "' found instead.");
    }
}

// <k_nearest> ::= TK_INTEGER
void SyntacticSelectAnalyzer::k_nearest(){

    if (currentToken().getTokenType() == Token::TK_INTEGER) {
        getParser()->match(getParser()->getCurrentToken());
    } else {
        addError("Integer expected (k-nn value) : Found '" + currentToken().getLexem() + "' instead.");
    }
}

// <val_range> ::= TK_FLOATING_POINT | TK_INTEGER
void SyntacticSelectAnalyzer::val_range(){

    //Must have a atof - like function that guarantees that is a number
    if ((currentToken().getTokenType() == Token::TK_INTEGER) || (currentToken().getTokenType() == Token::TK_FLOATING_POINT)) {
        getParser()->match(getParser()->getCurrentToken());
    } else {
        addError("Value expected for range: Found '" + currentToken().getLexem() + "' instead.");
    }
}

void SyntacticSelectAnalyzer::metric_name(){

    if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
        getParser()->match(getParser()->getCurrentToken());
    }else{
        addError("Metric name expected. '" + currentToken().getLexem() + "' found instead.");
    }
}

// query_obj_definition ::= complex_attribute_reference
//                        | subquery | particulate_reference
//                        | image_location | audio_location
void SyntacticSelectAnalyzer::query_obj_definition() {

    string tableName;

    // complex_attribute_reference
    if (currentToken().getTokenType() == Token::TK_IDENTIFIER) {

        column_reference();

        bool check_attr_ref = getDictionary()->isComplexAttributeReference(getParser()->getCurrentToken()->getLexem());
        bool tname_check = false;

        if (check_attr_ref) {
            vector<string> tbNameList = getTableNameFromTokenList(getParser()->getTokenList(), getParser()->getCurrentToken()->getLexem());
            if (tbNameList.size() == 0) {
                addError("Invalid complex attribute: '" + currentToken().getLexem() + "'.");
            } else {
                for (int i = 0; i < tbNameList.size(); i++) {
                    tableName = tbNameList[i];
                    if (currentToken().getTokenType() == Token::TK_PERIOD) {
                        if (getParser()->getToken(getParser()->getCurrentPosition()-2)->getLexem() == tableName)
                            tname_check = true;
                    } else {
                        tname_check = true;
                    }
                } if (!tname_check) {
                    addError("Invalid complex attribute: " + tableName + "." + currentToken().getLexem() + ".");
                }
            }
        }
    } else {
        // subquery ::= '(' query_specification ')'
        // or particulate_reference ::=  '(' parameter_val_assoc_list ')'
        if (currentToken().getTokenType() == Token::TK_OPEN_BRACE) {
            getParser()->match("(", Token::TK_OPEN_BRACE);
            if (getParser()->getCurrentToken()->toUpperLexem() == "SELECT") {
                query_specification();
            }
            getParser()->match(")", Token::TK_CLOSE_BRACE);
        } else {

            // image_location ::= 'TK_STRING' or audio_location ::= 'TK_STRING'
            if (currentToken().getTokenType() == Token::TK_STRING) {
                try{
                    string fileName;
                    for (int x = 1; x < currentToken().getLexem().size()-1; x++){
                        fileName += currentToken().getLexem()[x];
                    }

                    ComplexObject *cObject = new ComplexObject(fileName);
                    //last check por enquanto todos são still image
                    //evitar do cara mandar um .wav pra comparar com imagem
                    //cObject->isStillImage();
                    delete (cObject);

                    // @loc: avanca o caminho do arquivo
                    getParser()->match(getParser()->getCurrentToken());
                } catch (...) {
                    addError("This file cannot be open.");
                }
            } else {
                addError("Invalid complex object (query center) definition.");
            }
        }
    }
}

void SyntacticSelectAnalyzer::near_or_far_predicate(){

    if ((currentToken().toUpperLexem() == "NEAR") || (currentToken().toUpperLexem() == "FAR")) {
        getParser()->match(getParser()->getCurrentToken());
    } else {
        addError("'NEAR | FAR' predicate expected.");
    }

    // Optional similarity grouping
    if (currentToken().getTokenType() == Token::TK_IDENTIFIER) {
        if ((currentToken().toUpperLexem() == "SUM") ||
            (currentToken().toUpperLexem() == "ALL") ||
            (currentToken().toUpperLexem() == "MAX")){
            getParser()->match(getParser()->getCurrentToken());
        }
    }

    query_obj_definition();

    // Optional BY clause
    if ((!endOfStatement()) && (currentToken().toUpperLexem() == "BY")) {
        getParser()->match(getParser()->getCurrentToken());
        metric_name();
    }
    // Optional RANGE clause
    if ((!endOfStatement()) && (currentToken().toUpperLexem() == "RANGE")) {
        getParser()->match(getParser()->getCurrentToken());
        val_range();
    }
    // Optional STOP AFTER clause
    if ((!endOfStatement()) && (currentToken().toUpperLexem() == "STOP")) {
        getParser()->match(getParser()->getCurrentToken());
        if (currentToken().toUpperLexem() == "AFTER") {
            getParser()->match(getParser()->getCurrentToken());
            k_nearest();
            // optional WITH clause
            if ((!endOfStatement()) && (currentToken().toUpperLexem() == "WITH")) {
                getParser()->match(getParser()->getCurrentToken());
                tie_list();
            }
        } else {
            addError("'AFTER' expected. '" + currentToken().getLexem() + "' found instead.");
        }
    }
}

void SyntacticSelectAnalyzer::value_check(){

    // A valid value_expression can be: a string, an integer or a floating point
    if (currentToken().getTokenType() == Token::TK_STRING){
        getParser()->match(getParser()->getCurrentToken());
    } else {
        if (currentToken().getTokenType() == Token::TK_INTEGER){
            getParser()->match(getParser()->getCurrentToken());
        } else {
            if (currentToken().getTokenType() == Token::TK_FLOATING_POINT){
                getParser()->match(getParser()->getCurrentToken());
            } else {
                addError("Invalid expression. Expected integer, float or string for: "+currentToken().getLexem());
            }
        }
    }
}

void SyntacticSelectAnalyzer::value_expression(){

    if (currentToken().getLexem() == "(" ){
        getParser()->match("(",Token::TK_OPEN_BRACE);

        // Checking if its a sub-select
        // This select should have just one item at projection list
        if (currentToken().toUpperLexem() == "SELECT"){
            // Temporary table obtained from another select
            string aux = par_processing();
            SyntacticSelectAnalyzer *az = new SyntacticSelectAnalyzer(aux, getDictionary()->getDatabaseManager(), flagIgnoreSemantics(), flagIgnoreExtraTokens());
            if (!az->isValid()){
                for (int x = 0; x < az->countErrors(); x++){
                    addError(az->getError(x));
                }
            }
            delete(az);
        } else {
            value_check();
        }
        getParser()->match(")", Token::TK_CLOSE_BRACE);
    } else {
        value_check();
    }
}

void SyntacticSelectAnalyzer::between_clause(){

    value_expression();

    if (currentToken().toUpperLexem() == "AND"){
        getParser()->match(getParser()->getCurrentToken());
    } else {
        addError("Expected AND for BETWEEN clause. '" + currentToken().getLexem() + "' found instead.");
    }

    value_expression();
}

void SyntacticSelectAnalyzer::in_clause(){

    if (currentToken().getLexem() == "(" ){
        getParser()->match("(",Token::TK_OPEN_BRACE);

        // Checking if its a sub-select
        // This select should have just one item at projection list
        if (currentToken().toUpperLexem() == "SELECT"){
            // Temporary table obtained from another select
            string aux = par_processing();
            if (!flagIgnoreSemantics()){
                //Semantic processing for IN with subselect here
                if (countNumberOfProjections(aux) != 1){
                    addError("Operand of IN clause should contain 1 column(s).");
                }
            }
            SyntacticSelectAnalyzer *az = new SyntacticSelectAnalyzer(aux, getDictionary()->getDatabaseManager(), flagIgnoreSemantics(), flagIgnoreExtraTokens());
            if (!az->isValid()){
                for (int x = 0; x < az->countErrors(); x++){
                    addError(az->getError(x));
                }
            }
            delete(az);
        } else {
            getParser()->match(getParser()->getCurrentToken());
            while (currentToken().getLexem() != ")"){
                if (currentToken().getTokenType() == Token::TK_COMMA){
                    getParser()->match(getParser()->getCurrentToken());
                }
                if ((currentToken().getLexem() != ")") && ((currentToken().getLexem() != ","))){
                    getParser()->match(getParser()->getCurrentToken());
                } else {
                    addError("Identifier expected. '" + currentToken().getLexem() + "' found instead.");
                }
            }
        }

        getParser()->match(")", Token::TK_CLOSE_BRACE);
    } else {
        addError("'(' expected after IN clause. '" + currentToken().getLexem() + "' found instead.");
    }
}

void SyntacticSelectAnalyzer::predicate(int op){

    column_reference(op);

    bool check_attr_ref = getDictionary()->isComplexAttributeReference(getParser()->getToken(getParser()->getCurrentPosition()-1)->getLexem());

    if (check_attr_ref)
        complex_predicate();
    else
        regular_predicate();
}

void SyntacticSelectAnalyzer::regular_predicate(){

    // equals operator
    if (currentToken().getLexem() == "=") {
        getParser()->match("=", Token::TK_EQUAL);
        if (currentToken().getTokenType() == Token::TK_IDENTIFIER) {
            column_reference(0);
        } else {
            value_expression();
        }
    } else {
        // not equals operator
        if (currentToken().getLexem() == "!=") {
            getParser()->match("!=", Token::TK_NE);
            if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
                column_reference(0);
            } else {
                value_expression();
            }
        } else {
            // LIKE operator
            if (currentToken().toUpperLexem() == "LIKE") {
                getParser()->match(getParser()->getCurrentToken());
                value_expression();
            } else {
                // NOT EQUALS operator
                if (currentToken().getLexem() == "<>") {
                    getParser()->match("<>", Token::TK_N2);
                    if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
                        column_reference(0);
                    } else {
                        value_expression();
                    }
                } else {
                    // less than operator
                    if (currentToken().getLexem() == "<") {
                        getParser()->match("<", Token::TK_LQ);
                        if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
                            column_reference(0);
                        } else {
                            value_expression();
                        }
                    } else {
                        // less than or equals operator
                        if (currentToken().getLexem() == "<=") {
                            getParser()->match("<=", Token::TK_LE);
                            if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
                                column_reference(0);
                            } else {
                                value_expression();
                            }
                        } else {
                            // greater than operator
                            if (currentToken().getLexem() == ">") {
                                getParser()->match(">", Token::TK_GQ);
                                if (currentToken().getTokenType() == Token::TK_IDENTIFIER) {
                                    column_reference(0);
                                } else {
                                    value_expression();
                                }
                            } else {
                                // greater than or equals operator
                                if (currentToken().getLexem() == ">=") {
                                    getParser()->match(">=", Token::TK_GE);
                                    if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
                                        column_reference(0);
                                    } else {
                                        value_expression();
                                    }
                                }  else {
                                    // IN-list Operator
                                    if (currentToken().toUpperLexem() == "NOT"){
                                        getParser()->match(getParser()->getCurrentToken());
                                        if (currentToken().toUpperLexem() == "IN"){
                                            getParser()->match(getParser()->getCurrentToken());
                                            in_clause();
                                        }
                                    } else {
                                        if (currentToken().toUpperLexem() == "IN"){
                                            getParser()->match(getParser()->getCurrentToken());
                                            in_clause();
                                        } else {
                                            // BETWEEN operator
                                            if (currentToken().toUpperLexem() == "BETWEEN"){
                                                getParser()->match(getParser()->getCurrentToken());
                                                between_clause();
                                            } else {
                                                addError("Invalid operator '" +currentToken().getLexem()+"' for non-complex attribute.");
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
    }
}

void SyntacticSelectAnalyzer::complex_predicate(){

    string ca;

    ca = getParser()->getToken(getParser()->getCurrentPosition()-1)->getLexem();
    bool tname_check = false;

    vector<string> tbNameList;
    tbNameList = getTableNameFromTokenList(getParser()->getTokenList(), ca);
    if (tbNameList.empty()){
        addError("Invalid complex attribute: '" + currentToken().getLexem() + "'.");
    } else {
        for (int i = 0; i < tbNameList.size(); i++) {
            if (getDictionary()->isComplexAttributeReference(tbNameList[i], ca)){
                tname_check = true;
            }
        }
        if (!tname_check) {
            addError("Invalid complex attribute: '" + ca + "'.");
        }
    }

    // near_predicate
    if ((currentToken().toUpperLexem() == "NEAR") || (currentToken().toUpperLexem() == "FAR")){
        near_or_far_predicate();
    } else {
        // predicate not implemented!
        addError("Invalid operator '" +currentToken().getLexem()+"' for complex attribute.");
    }
}

// <search_condition> ::= NOT search_condition
//                      | predicate
//                      | search_condition AND search_condition
//                      | search_condition OR search_condition
void SyntacticSelectAnalyzer::search_condition(int op){

    if ((!endOfStatement()) && (currentToken().toUpperLexem() == "NOT")) {
        getParser()->match(getParser()->getCurrentToken());
        if (currentToken().getTokenType() == Token::TK_IDENTIFIER) {
            search_condition(op);
        } else {
            addError("Identifier expected. '" + currentToken().getLexem() + "' found instead.");
        }
    }

    predicate(op);

    if ((!endOfStatement()) && ((currentToken().toUpperLexem() == "AND") || (currentToken().toUpperLexem() == "OR"))){
        getParser()->match(getParser()->getCurrentToken());
        if (currentToken().getTokenType() == Token::TK_IDENTIFIER) {
            search_condition(op);
        } else {
            addError("'AND | OR' predicates are expected. '" + currentToken().getLexem() + "' found instead.");
        }
    }
}

// <having_clause> ::= HAVING <search_condition>
void SyntacticSelectAnalyzer::having_clause(){

    if (currentToken().toUpperLexem() == "HAVING"){
        getParser()->match(getParser()->getCurrentToken());
        search_condition(1);
    } else {
        addError("Aggregate function expected. '" + currentToken().getLexem() + "' found instead.");
    }
}

void SyntacticSelectAnalyzer::groupby_clause(){

    if (currentToken().toUpperLexem() == "GROUP"){
        getParser()->match(getParser()->getCurrentToken());
        if (currentToken().toUpperLexem() == "BY"){
            getParser()->match(getParser()->getCurrentToken());
        } else {
            addError("'BY' expression expected. '" + currentToken().getLexem() + "' found instead.");
        }
    }

    getParser()->match("(", Token::TK_OPEN_BRACE);

    if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
        column_reference();
        while ((!endOfStatement()) && (currentToken().getTokenType() == Token::TK_COMMA)){
            getParser()->match(",", Token::TK_COMMA);
            column_reference();
        }
    } else {
        addError("Identifier expected. '" + currentToken().getLexem() + "' found instead.");
    }

    getParser()->match(")", Token::TK_CLOSE_BRACE);
}

void SyntacticSelectAnalyzer::where_clause(){

    if (currentToken().toUpperLexem() == "WHERE"){
        getParser()->match(getParser()->getCurrentToken());

        if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
            search_condition();
        } else {
            addError("Identifier expected. '" + currentToken().getLexem() + "' found instead.");
        }
    }
}

// <table_reference_list> ::= <table_name>
//                          | <table_name> ',' <table_name_list>
void SyntacticSelectAnalyzer::table_reference_list(){

    read_table();

    // If a comma is found, read the next table
    if (!endOfStatement()){
        if (currentToken().getTokenType() == Token::TK_COMMA){
            getParser()->match(",", Token::TK_COMMA);
            if ((currentToken().getTokenType() == Token::TK_IDENTIFIER) || (currentToken().getTokenType() == Token::TK_OPEN_BRACE)){
                table_reference_list();
            } else {
                addError("Identifier expected. '" + currentToken().getLexem() + "' found instead.");
            }
        } else {
            if (currentToken().toUpperLexem() != "WHERE"){
                join_reference_list();
                if ((!endOfStatement()) && (currentToken().getTokenType() == Token::TK_COMMA)){
                    getParser()->match(",", Token::TK_COMMA);
                    if ((currentToken().getTokenType() == Token::TK_IDENTIFIER) && (currentToken().toUpperLexem() != "WHERE")){
                        table_reference_list();
                    } else {
                        addError("Identifier expected. '" + currentToken().getLexem() + "' found instead.");
                    }
                }
            }
        }
    }
}

string SyntacticSelectAnalyzer::par_processing(){

    string aux = "";
    int matching = 0;

    matching++;

    // While the parenthisation is not finished increase the string
    while (matching > 0){

        if (getParser()->getCurrentToken()->getLexem() == "("){
            matching ++;
        }
        if (getParser()->getCurrentToken()->getLexem() == ")"){
            matching --;
        }

        if (matching > 0){
            aux.append(currentToken().getLexem());
            getParser()->match(getParser()->getCurrentToken());
            while (currentToken().getTokenType() == Token::TK_PERIOD) {
                aux.append(currentToken().getLexem());
                getParser()->match(getParser()->getCurrentToken());
                aux.append(currentToken().getLexem());
                getParser()->match(getParser()->getCurrentToken());
            }
            aux.append(" ");
        }
    }

    return aux;
}

void SyntacticSelectAnalyzer::read_table(){

    if (currentToken().getLexem() == "(" ){
        getParser()->match("(",Token::TK_OPEN_BRACE);
    }

    // Checking if its a sub-select
    if (currentToken().toUpperLexem() == "SELECT"){
        // Temporary table obtained from another select
        string aux = par_processing();

        SyntacticSelectAnalyzer *az = new SyntacticSelectAnalyzer(aux, getDictionary()->getDatabaseManager(), flagIgnoreSemantics(), flagIgnoreExtraTokens());
        if (!az->isValid()){
            for (int x = 0; x < az->countErrors(); x++){
                addError(az->getError(x));
            }
        }
        delete(az);
        addTable(aux); //SM
    } else {
        addTable(currentToken().getLexem()); //SM
        getParser()->match(getParser()->getCurrentToken());

        if ((!endOfStatement()) &&
                ((currentToken().toUpperLexem() == "AS") || (currentToken().toUpperLexem() != "WHERE") &&
                 (currentToken().toUpperLexem() != "FROM")) &&
                (currentToken().toUpperLexem() != "JOIN") &&
                (currentToken().toUpperLexem() != "INNER") &&
                (currentToken().toUpperLexem() != "LEFT") &&
                (currentToken().toUpperLexem() != "RIGHT") &&
                (currentToken().toUpperLexem() != "FULL") &&
                (currentToken().toUpperLexem() != "NATURAL") &&
                (currentToken().toUpperLexem() != "CLOSEST") &&
                (currentToken().toUpperLexem() != "NEAREST") &&
                (currentToken().toUpperLexem() != "RANGE") &&
                (currentToken().toUpperLexem() != "ON") ){
            as_clause(1);
        }
    }



    if (!endOfStatement()){
        if (currentToken().getLexem() == ")" ){
            getParser()->match(")", Token::TK_CLOSE_BRACE);
            if ((!endOfStatement()) && (currentToken().toUpperLexem() == "AS"))
                getParser()->match(getParser()->getCurrentToken());
            if ((!endOfStatement()) && (currentToken().getTokenType() == Token::TK_IDENTIFIER) && (currentToken().getLexem() != "ON")){
                addTableAlias(currentToken().getLexem());
                getParser()->match(getParser()->getCurrentToken());
            } else {
                addError("You should specify an alias to a derived table.");
            }
        }
    }
}

// <from_clause> ::= FROM <table_reference_list>
void SyntacticSelectAnalyzer::from_clause(){

    if (currentToken().toUpperLexem() == "FROM"){
        getParser()->match(getParser()->getCurrentToken());
    }
    table_reference_list();
}

// <table_expression> ::= from_clause
//                      | where_clause
//                      | group_by_clause
//                      | having_clause
void SyntacticSelectAnalyzer::table_expression(){

    if (currentToken().toUpperLexem() == "FROM") {
        from_clause();

        // Optional WHERE clause
        if ((!endOfStatement()) && (currentToken().toUpperLexem() == "WHERE")){
            where_clause();
        }


        // Optional GROUP BY clause
        if ((!endOfStatement()) && (currentToken().toUpperLexem() == "GROUP")){
            groupby_clause();
            // Optional HAVING clause
            if ((!endOfStatement()) && (currentToken().toUpperLexem() == "HAVING")){
                having_clause();
            }
        }

        // Optional ORDER BY clause
        if ((!endOfStatement()) && (currentToken().toUpperLexem() == "ORDER")){
            order_by_clause();
        }

        // If its not any of above, then its an error
        if ((!endOfStatement()) && (getParser()->getActiveScope() > 0) && (currentToken().toUpperLexem() != "WHERE") && (currentToken().toUpperLexem() != "GROUP")  && (currentToken().toUpperLexem() != "ORDER")){
            addError("Invalid clause: '" + currentToken().getLexem() + "' found. 'WHERE | GROUP BY | ORDER BY' expected.");
        }
    } else {
        addError("'FROM' clause expected . '" + currentToken().getLexem() + "' found instead.");
    }
}

void SyntacticSelectAnalyzer::order_by_clause(){


    if (currentToken().toUpperLexem() == "ORDER"){
        getParser()->match(getParser()->getCurrentToken());
        if (currentToken().toUpperLexem() == "BY"){
            getParser()->match(getParser()->getCurrentToken());
            if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
                column_reference();
                while ((!endOfStatement()) && (currentToken().getTokenType() == Token::TK_COMMA)){
                    getParser()->match(",", Token::TK_COMMA);
                    column_reference();
                }
                if ((!endOfStatement()) && ((currentToken().toUpperLexem() == "ASC") || (currentToken().toUpperLexem() == "DESC"))){
                    getParser()->match(getParser()->getCurrentToken());
                }
            } else {
                addError("Identifier expected. '" + currentToken().getLexem() + "'found instead.");
            }
        } else {
            addError("ORDER BY clause expected. '" + currentToken().getLexem() + "'found instead.");
        }
    } else {
        addError("ORDER BY clause expected. '" + currentToken().getLexem() + "'found instead.");
    }
}

// <as_clause> ::= AS <identifier>
void SyntacticSelectAnalyzer::as_clause(int op){

    if (currentToken().toUpperLexem() == "AS"){
        getParser()->match(getParser()->getCurrentToken());
        if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
            if (op == 1){
                addTableAlias(currentToken().getLexem());// SM
            }
            getParser()->match(getParser()->getCurrentToken());
        } else {
            addError("Identifier expected to rename. '" + currentToken().getLexem() + "' found instead.");
        }
    } else {
        if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
            if (op == 1){
                addTableAlias(currentToken().getLexem());//SM
            }
            getParser()->match(getParser()->getCurrentToken());
        }
    }
}

void SyntacticSelectAnalyzer::join_reference(){

    if (currentToken().toUpperLexem() == "JOIN" ||
            currentToken().toUpperLexem() == "INNER" ||
            currentToken().toUpperLexem() == "LEFT" ||
            currentToken().toUpperLexem() == "RIGHT" ||
            currentToken().toUpperLexem() == "FULL" ||
            currentToken().toUpperLexem() == "NATURAL"
            ){
        process_join();
    } else {
        if(currentToken().toUpperLexem() == "CLOSEST" ||
                currentToken().toUpperLexem() == "NEAREST" ||
                currentToken().toUpperLexem() == "RANGE"){
            process_similarity_join();
        } else {
            addError("'JOIN' expected. '" + currentToken().getLexem() + "' found instead.");
        }
    }
}

void SyntacticSelectAnalyzer::join_reference_list(){

    join_reference();

    if (!endOfStatement()){
        if (currentToken().toUpperLexem() == "JOIN" ||
                currentToken().toUpperLexem() == "INNER" ||
                currentToken().toUpperLexem() == "LEFT" ||
                currentToken().toUpperLexem() == "RIGHT" ||
                currentToken().toUpperLexem() == "FULL" ||
                currentToken().toUpperLexem() == "NATURAL" ||
                currentToken().toUpperLexem() == "CLOSEST" ||
                currentToken().toUpperLexem() == "NEAREST" ||
                currentToken().toUpperLexem() == "RANGE"){
            join_reference_list();
        }
    }
}

void SyntacticSelectAnalyzer::process_join(){

    if (currentToken().toUpperLexem() == "OUTER" ||
            currentToken().toUpperLexem() == "JOIN"){
        getParser()->match(getParser()->getCurrentToken());
    }

    if (currentToken().toUpperLexem() == "JOIN"){
        getParser()->match(getParser()->getCurrentToken());
    }

    read_table();

    if ((!endOfStatement()) && (currentToken().toUpperLexem() == "ON")){
        getParser()->match(getParser()->getCurrentToken());
    } else {
        addError("'ON' expected. '" + currentToken().getLexem() + "' found instead.");
    }
    predicate();
}

void SyntacticSelectAnalyzer::process_similarity_join(){

    if (currentToken().toUpperLexem() == "CLOSEST"){
        getParser()->match(getParser()->getCurrentToken());
    } else {
        if (currentToken().toUpperLexem() == "NEAREST"){
            getParser()->match(getParser()->getCurrentToken());
        } else {
            if(currentToken().toUpperLexem() == "RANGE"){
                getParser()->match(getParser()->getCurrentToken());
            } else {
                addError("CLOSEST | NEAREST | RANGE JOIN expected.");
            }
        }
    }

    if (currentToken().toUpperLexem() == "JOIN"){
        getParser()->match(getParser()->getCurrentToken());
    } else {
        addError("'JOIN' expected. '" + currentToken().getLexem() + "' found instead.");
    }

    read_table();

    if (currentToken().toUpperLexem() == "ON"){
        getParser()->match(getParser()->getCurrentToken());
    } else {
        addError("'ON' expected. '" + currentToken().getLexem() + "' found instead.");
    }

    predicate();
}

void SyntacticSelectAnalyzer::asterisk_name(){

    if (currentToken().getLexem() == "*"){
        getParser()->match(getParser()->getCurrentToken());

        if (currentToken().toUpperLexem() != "FROM"){
        }
    }
}

void SyntacticSelectAnalyzer::column_reference(int op){

    string tableAlias;
    string columnName;

    if (endOfStatement()){
        addError("Missing column name.");
    }

    // Supporting grouping functions - just for single data?
    if ((op == 1) && ((currentToken().toUpperLexem() == "COUNT") || (currentToken().toUpperLexem() == "MIN") || (currentToken().toUpperLexem() == "MAX") || (currentToken().toUpperLexem() == "SUM") || (currentToken().toUpperLexem() == "AVG"))) {
        getParser()->match(getParser()->getCurrentToken());
        getParser()->match("(", Token::TK_OPEN_BRACE);
        if (currentToken().getLexem() == "*"){
            asterisk_name();
        } else {
            if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
                tableAlias = currentToken().getLexem();
                getParser()->match(getParser()->getCurrentToken());
                if ((!endOfStatement()) && (currentToken().getTokenType() == Token::TK_PERIOD)) {
                    getParser()->match(getParser()->getCurrentToken());
                    if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
                        columnName = currentToken().getLexem();
                        getParser()->match(getParser()->getCurrentToken());
                    } else {
                        addError("Identifier expected. '" + currentToken().getLexem() + "' found instead.");
                    }
                }
            } else {
                addError("Identifier expected. '" + currentToken().getLexem() + "' found instead.");
            }
        }
        getParser()->match(")", Token::TK_CLOSE_BRACE);
    } else {
        if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
            tableAlias =  currentToken().getLexem();
            getParser()->match(getParser()->getCurrentToken());
            if ((!endOfStatement()) && (currentToken().getTokenType() == Token::TK_PERIOD)) {
                getParser()->match(getParser()->getCurrentToken());
                if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
                    //column_reference(); -> Need for catch oriented-like objects
                    columnName = currentToken().getLexem();
                    getParser()->match(getParser()->getCurrentToken());
                } else {
                    addError("Identifier expected. '" + currentToken().getLexem() + "' found instead.");
                }
            }
        } else {
            addError("Identifier expected. '" + currentToken().getLexem() + "' found instead.");
        }
    }

    if (columnName.empty()){
        addColumn(tableAlias);
    } else {
        addColumn(columnName, tableAlias);
    }

    tableAlias.clear();
    columnName.clear();
}

void SyntacticSelectAnalyzer::column_reference_list(){

    column_reference(1);

    if (currentToken().toUpperLexem() != "FROM"){
        as_clause();
    }

    if (currentToken().getTokenType() == Token::TK_COMMA){
        getParser()->match(",", Token::TK_COMMA);
        if ((currentToken().getTokenType() == Token::TK_IDENTIFIER) && (currentToken().toUpperLexem() != "FROM")){
            column_reference_list();
        } else {
            addError("Identifier expected. '" + currentToken().getLexem() + "' found instead.");
        }
    }
}


// <select_list> ::= '*' | column_reference_list
void SyntacticSelectAnalyzer::select_list(){

    if (currentToken().getLexem() == "*"){
        asterisk_name();
        if (currentToken().getTokenType() == Token::TK_COMMA){
            getParser()->match(",", Token::TK_COMMA);
            column_reference_list();
        }
    } else {
        if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
            column_reference_list();
        } else {
            addError("Invalid column reference: " + currentToken().getLexem());
        }
    }
}

void SyntacticSelectAnalyzer::query_specification(){

    try{
        if (currentToken().toUpperLexem() == "SELECT"){
            getParser()->match(getParser()->getCurrentToken());
            // optional quantifier
            if ((currentToken().toUpperLexem() == "DISTINCT") || (currentToken().toUpperLexem() == "ALL")){
                getParser()->match(getParser()->getCurrentToken());
            }

            select_list();
            table_expression();

            if (!hasErrors()) {
                getParser()->defineRightScopes();
            }

            if (!flagIgnoreExtraTokens()){
                if ((!endOfStatement()) && ((currentToken().getLexem() != ")") || (getParser()->getCurrentPosition() == getParser()->countTokens()-1))){
                    addError("Syntax Error (1).");
                    addError("Extra tokens after the end of statement. Wrong expression after: '" + getParser()->getToken(getParser()->getCurrentPosition()-1)->getLexem() + "'.");
                }
            }
        }
    } catch (...) {
        if (!endOfStatement()){
            addError("Syntax Error (1).");
            addError("Wrong expression after: '" + getParser()->getToken(getParser()->getCurrentPosition()-1)->getLexem() + "'.");
        } else {
            addError("Wrong expression after: '" + getParser()->getToken(getParser()->countTokens()-1)->getLexem() + "'.");
            addError("Syntax Error (1).");
        }
    }
}

vector<string> SyntacticSelectAnalyzer::getTableNameFromTokenList(vector<Token*> tokenList, string attribName){

    vector<string> result;
    int x = 0;
    string tName;

    while ((x < tokenList.size()) && (tokenList[x]->toUpperLexem() != "FROM")){
        x++;
    }

    if (x >= tokenList.size())
        return result;

    int tk2 = x + 1;

    while ((tk2 < tokenList.size()) && (tokenList[tk2]->toUpperLexem() != "WHERE")){
        tName = tokenList[tk2]->getLexem();

        if (getDictionary()->isComplexAttributeReference(tName, attribName)){
            bool already = false;
            for (int y = 0; y < result.size(); y++){
                if (result[y] == tName)
                    already = true;
            }
            if (!already)
                result.push_back(tName);
        }
        tk2++;
    }

    return result;
}

bool SyntacticSelectAnalyzer::isValid(){

    clearErrors();

    query_specification();
    getParser()->resetParser();

    if (!flagIgnoreSemantics()){
        if (!listTables->isValid()){
            addError(listTables->getError());
        }
        if (!listColumns->isValid(listTables)){
            addError(listColumns->getError());
        }
    }


    if (listTables != NULL){
        delete (listTables);
    }
    if (listColumns != NULL){
        delete (listColumns);
    }

    listTables = new CheckValidTables(dic);
    listColumns = new CheckValidColumns(dic);

    return (!hasErrors());
}

void SyntacticSelectAnalyzer::setFlagIgnoreSemantics(bool ignoreSemantics){

    is = ignoreSemantics;
}

void SyntacticSelectAnalyzer::setFlagIgnoreExtraTokens(bool ignoreExtraTokens){

    iext = ignoreExtraTokens;
}

void SyntacticSelectAnalyzer::addTable(string tableName, string alias){

    if (alias.empty()){
        listTables->addNewTable(tableName);
    } else {
        listTables->addNewTable(tableName, alias);
    }
}

void SyntacticSelectAnalyzer::addTableAlias(string alias){

    listTables->addNewAlias(alias);
}

void SyntacticSelectAnalyzer::addColumn(string column, string tableName){

    if (tableName.empty()){
        listColumns->addNewColumn(column);
    } else {
        listColumns->addNewColumn(tableName, column);
    }
}

bool SyntacticSelectAnalyzer::flagIgnoreSemantics(){

    return is;
}

bool SyntacticSelectAnalyzer::flagIgnoreExtraTokens(){

    return iext;
}

int SyntacticSelectAnalyzer::countNumberOfProjections(string subselect){

    Parser *p = new Parser(subselect);

    int count = 0;
    if (p->getCurrentToken()->toUpperLexem() == "SELECT"){
        p->match(p->getCurrentToken());
        while ((p->getCurrentPosition() != -1) && (p->getCurrentToken()->toUpperLexem() != "FROM")){
            if ((p->getCurrentToken()->getLexem() != "*") && (p->getCurrentToken()->getTokenType() != Token::TK_COMMA)){
                count++;
            }
            p->match(p->getCurrentToken());
        }
    }
    delete (p);

    return count;
}
