#include "SelectExecutor.hpp"

SelectExecutor::~SelectExecutor(){

    if (dic != NULL)
        delete (dic);
    if (listTables != NULL){
        delete (listTables);
    }
    if (listColumns != NULL){
        delete (listColumns);
    }
}

void SelectExecutor::tie_list(){

    getParser()->match(getParser()->getCurrentToken());
    getParser()->match(getParser()->getCurrentToken());
}

void SelectExecutor::k_nearest(){

    getParser()->match(getParser()->getCurrentToken());
}

void SelectExecutor::val_range(){

    getParser()->match(getParser()->getCurrentToken());
}

void SelectExecutor::metric_name(){

    getParser()->match(getParser()->getCurrentToken());
}

void SelectExecutor::query_obj_definition() {

    Token *t;
    vector<Token *> tList;

    if (currentToken().getTokenType() == Token::TK_IDENTIFIER) {
        column_reference();
    } else {
        if (currentToken().getTokenType() == Token::TK_OPEN_BRACE) {
            getParser()->match("(", Token::TK_OPEN_BRACE);
            if (getParser()->getCurrentToken()->toUpperLexem() == "SELECT") {

                int bef, after;
                bef = getParser()->getCurrentPosition();
                string txt = par_processing();
                after = getParser()->getCurrentPosition();

                for (int x = 0; x < (after-bef); x++){
                    getParser()->removeToken(bef);
                }

                SelectExecutor *aux = new SelectExecutor(txt, dictionary()->getDatabaseManager());
                txt = aux->translate()[0];
                Parser *p = new Parser(txt);
                for (int x = 0; x < p->countTokens(); x++){
                    t = new Token(*p->getToken(x));
                    tList.push_back(t);
                }
                getParser()->insertTokenList(tList, bef);

                for (int x = (after - bef); x < tList.size(); x++){
                    getParser()->match(getParser()->getCurrentToken());
                }

                delete (p);
                delete (aux);
                tList.clear();
            }
            getParser()->match(")", Token::TK_CLOSE_BRACE);
        } else {
            try{
                string fileName;
                for (int x = 1; x < currentToken().getLexem().size()-1; x++){
                    fileName += currentToken().getLexem()[x];
                }
                ComplexObject *cObject = new ComplexObject(fileName);
                delete (cObject);
                // @loc: avanca o caminho do arquivo
                getParser()->match(getParser()->getCurrentToken());
            } catch (...) {
            }
        }
    }
}

void SelectExecutor::near_or_far_predicate(){

    getParser()->match(getParser()->getCurrentToken());

    // Optional similarity grouping
    if (currentToken().getTokenType() == Token::TK_IDENTIFIER) {
        if ((currentToken().toUpperLexem() == "SUM") || (currentToken().toUpperLexem() == "AVG") || (currentToken().toUpperLexem() == "ALL") || (currentToken().toUpperLexem() == "EVERY") || (currentToken().toUpperLexem() == "ANY") || (currentToken().toUpperLexem() == "MAX")) {
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
        getParser()->match(getParser()->getCurrentToken());
        k_nearest();
        // optional WITH clause
        if ((!endOfStatement()) && (currentToken().toUpperLexem() == "WITH")) {
            getParser()->match(getParser()->getCurrentToken());
            tie_list();
        }
    }
}

void SelectExecutor::value_check(){

    getParser()->match(getParser()->getCurrentToken());
}

void SelectExecutor::value_expression(){

    Token *t;
    vector<Token *> tList;

    if (currentToken().getLexem() == "(" ){
        getParser()->match("(",Token::TK_OPEN_BRACE);

        if (currentToken().toUpperLexem() == "SELECT"){

            int bef, after;
            bef = getParser()->getCurrentPosition();
            string txt = par_processing();
            after = getParser()->getCurrentPosition();

            for (int x = 0; x < (after-bef); x++){
                getParser()->removeToken(bef);
            }

            SelectExecutor *aux = new SelectExecutor(txt, dictionary()->getDatabaseManager());
            txt = aux->translate()[0];
            Parser *p = new Parser(txt);
            for (int x = 0; x < p->countTokens(); x++){
                t = new Token(*p->getToken(x));
                tList.push_back(t);
            }
            getParser()->insertTokenList(tList, bef);

            for (int x = (after - bef); x < tList.size(); x++){
                getParser()->match(getParser()->getCurrentToken());
            }

            delete (p);
            delete (aux);
            tList.clear();
        } else {
            value_check();
        }
        getParser()->match(")", Token::TK_CLOSE_BRACE);
    } else {
        value_check();
    }
}

void SelectExecutor::between_clause(){

    value_expression();

    getParser()->match(getParser()->getCurrentToken());

    value_expression();
}

void SelectExecutor::in_clause(){

    Token *t;
    vector<Token *> tList;

    getParser()->match("(",Token::TK_OPEN_BRACE);

    if (currentToken().toUpperLexem() == "SELECT"){;

        int bef, after;
        bef = getParser()->getCurrentPosition();
        string txt = par_processing();
        after = getParser()->getCurrentPosition();

        for (int x = 0; x < (after-bef); x++){
            getParser()->removeToken(bef);
        }

        SelectExecutor *aux = new SelectExecutor(txt, dictionary()->getDatabaseManager());
        txt = aux->translate()[0];
        Parser *p = new Parser(txt);
        for (int x = 0; x < p->countTokens(); x++){
            t = new Token(*p->getToken(x));
            tList.push_back(t);
        }
        getParser()->insertTokenList(tList, bef);

        for (int x = (after - bef); x < tList.size(); x++){
            getParser()->match(getParser()->getCurrentToken());
        }

        delete (p);
        delete (aux);
        tList.clear();
    } else {
        getParser()->match(getParser()->getCurrentToken());
        while (currentToken().getLexem() != ")"){
            if (currentToken().getTokenType() == Token::TK_COMMA){
                getParser()->match(getParser()->getCurrentToken());
            }
            if ((currentToken().getLexem() != ")") && ((currentToken().getLexem() != ","))){
                getParser()->match(getParser()->getCurrentToken());
            }
        }
    }

    getParser()->match(")", Token::TK_CLOSE_BRACE);
}

void SelectExecutor::predicate(){

    column_reference();

    bool check_attr_ref = dictionary()->isComplexAttributeReference(getParser()->getToken(getParser()->getCurrentPosition()-1)->getLexem());

    if (check_attr_ref)
        complex_predicate();
    else
        regular_predicate();
}

void SelectExecutor::regular_predicate(){

    // equals operator
    if (currentToken().getLexem() == "=") {
        getParser()->match("=", Token::TK_EQUAL);
        if (currentToken().getTokenType() == Token::TK_IDENTIFIER) {
            column_reference();
        } else {
            value_expression();
        }
    } else {
        // not equals operator
        if (currentToken().getLexem() == "!=") {
            getParser()->match("!=", Token::TK_NE);
            if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
                column_reference();
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
                        column_reference();
                    } else {
                        value_expression();
                    }
                } else {
                    // less than operator
                    if (currentToken().getLexem() == "<") {
                        getParser()->match("<", Token::TK_LQ);
                        if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
                            column_reference();
                        } else {
                            value_expression();
                        }
                    } else {
                        // less than or equals operator
                        if (currentToken().getLexem() == "<=") {
                            getParser()->match("<=", Token::TK_LE);
                            if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
                                column_reference();
                            } else {
                                value_expression();
                            }
                        } else {
                            // greater than operator
                            if (currentToken().getLexem() == ">") {
                                getParser()->match(">", Token::TK_GQ);
                                if (currentToken().getTokenType() == Token::TK_IDENTIFIER) {
                                    column_reference();
                                } else {
                                    value_expression();
                                }
                            } else {
                                // greater than or equals operator
                                if (currentToken().getLexem() == ">=") {
                                    getParser()->match(">=", Token::TK_GE);
                                    if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
                                        column_reference();
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

void SelectExecutor::complex_predicate(){

    near_or_far_predicate();
}

void SelectExecutor::search_condition(){

    if ((!endOfStatement()) && (currentToken().toUpperLexem() == "NOT")) {
        getParser()->match(getParser()->getCurrentToken());
        search_condition();
    }

    predicate();

    if ((!endOfStatement()) && ((currentToken().toUpperLexem() == "AND") || (currentToken().toUpperLexem() == "OR"))){
        getParser()->match(getParser()->getCurrentToken());
        search_condition();
    }
}

void SelectExecutor::having_clause(){

    getParser()->match(getParser()->getCurrentToken());
    search_condition();
}

void SelectExecutor::groupby_clause(){

    if (currentToken().toUpperLexem() == "GROUP"){
        getParser()->match(getParser()->getCurrentToken());
        getParser()->match(getParser()->getCurrentToken());
    }

    getParser()->match("(", Token::TK_OPEN_BRACE);

    column_reference();
    while ((!endOfStatement()) && (currentToken().getTokenType() == Token::TK_COMMA)){
        getParser()->match(",", Token::TK_COMMA);
        column_reference();
    }

    getParser()->match(")", Token::TK_CLOSE_BRACE);
}

void SelectExecutor::where_clause(){

    if (currentToken().toUpperLexem() == "WHERE"){
        getParser()->match(getParser()->getCurrentToken());
    }

    search_condition();
}

void SelectExecutor::table_reference_list(){

    read_table();

    // If a comma is found, read the next table
    if (!endOfStatement()){
        if (currentToken().getTokenType() == Token::TK_COMMA){
            getParser()->match(",", Token::TK_COMMA);
            table_reference_list();
        } else {
            if (currentToken().toUpperLexem() != "WHERE"){
                join_reference_list();
                if ((!endOfStatement()) && (currentToken().getTokenType() == Token::TK_COMMA)){
                    getParser()->match(",", Token::TK_COMMA);
                    if ((currentToken().getTokenType() == Token::TK_IDENTIFIER) && (currentToken().toUpperLexem() != "WHERE")){
                        table_reference_list();
                    }
                }
            }
        }
    }
}

string SelectExecutor::par_processing(){

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

void SelectExecutor::read_table(){

    if (currentToken().getLexem() == "(" ){
        getParser()->match("(",Token::TK_OPEN_BRACE);
    }

    // Checking if its a sub-select
    if (currentToken().toUpperLexem() == "SELECT"){
        string aux = par_processing();
        /*
        SyntacticSelectAnalyzer *az = new SyntacticSelectAnalyzer(aux, getDictionary()->getDatabaseManager(), flagIgnoreSemantics(), flagIgnoreExtraTokens());
        if (!az->isValid()){
            for (int x = 0; x < az->countErrors(); x++){
                addError(az->getError(x));
            }
        }
        delete(az);*/
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
            }
        }
    }
}

void SelectExecutor::from_clause(){

    if (currentToken().toUpperLexem() == "FROM"){
        getParser()->match(getParser()->getCurrentToken());
    }
    table_reference_list();
}

// <table_expression> ::= from_clause
//                      | where_clause
//                      | group_by_clause
//                      | having_clause
void SelectExecutor::table_expression(){

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
}

void SelectExecutor::order_by_clause(){

    getParser()->match(getParser()->getCurrentToken());
    getParser()->match(getParser()->getCurrentToken());
    column_reference();
    while ((!endOfStatement()) && (currentToken().getTokenType() == Token::TK_COMMA)){
        getParser()->match(",", Token::TK_COMMA);
        column_reference();
    }
    if ((!endOfStatement()) && ((currentToken().toUpperLexem() == "ASC") || (currentToken().toUpperLexem() == "DESC"))){
        getParser()->match(getParser()->getCurrentToken());
    }

}

void SelectExecutor::as_clause(int op){

    if (currentToken().toUpperLexem() == "AS"){
        getParser()->match(getParser()->getCurrentToken());
        if (op == 1){
            addTableAlias(currentToken().getLexem());// SM
        }
        getParser()->match(getParser()->getCurrentToken());
    } else {
        if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
            if (op == 1){
                addTableAlias(currentToken().getLexem());//SM
            }
            getParser()->match(getParser()->getCurrentToken());
        }
    }
}

void SelectExecutor::join_reference(){

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
        }
    }
}

void SelectExecutor::join_reference_list(){

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

void SelectExecutor::process_join(){

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
    }
    predicate();
}

void SelectExecutor::process_similarity_join(){

    getParser()->match(getParser()->getCurrentToken());
    getParser()->match(getParser()->getCurrentToken());

    read_table();

    getParser()->match(getParser()->getCurrentToken());

    predicate();
}

void SelectExecutor::asterisk_name(){

    if (currentToken().getLexem() == "*"){
        getParser()->match(getParser()->getCurrentToken());
    }
}

void SelectExecutor::column_reference(){

    string tableAlias;


    if ((currentToken().toUpperLexem() == "COUNT") || (currentToken().toUpperLexem() == "MIN") || (currentToken().toUpperLexem() == "MAX") || (currentToken().toUpperLexem() == "SUM") || (currentToken().toUpperLexem() == "AVG")) {
        getParser()->match(getParser()->getCurrentToken());
        getParser()->match("(", Token::TK_OPEN_BRACE);
        if (currentToken().getLexem() == "*"){
            asterisk_name();
        } else {
            tableAlias = currentToken().getLexem();
            getParser()->match(getParser()->getCurrentToken());
            if ((!endOfStatement()) && (currentToken().getTokenType() == Token::TK_PERIOD)) {
                getParser()->match(getParser()->getCurrentToken());
                getParser()->match(getParser()->getCurrentToken());
            }
        }
        getParser()->match(")", Token::TK_CLOSE_BRACE);
    } else {
        tableAlias =  currentToken().getLexem();
        getParser()->match(getParser()->getCurrentToken());
        if ((!endOfStatement()) && (currentToken().getTokenType() == Token::TK_PERIOD)) {
            getParser()->match(getParser()->getCurrentToken());
            //column_reference(); -> Need for catch oriented-like objects
            getParser()->match(getParser()->getCurrentToken());
        }
    }

    tableAlias.clear();
}

void SelectExecutor::column_reference_list(){

    column_reference();

    if (currentToken().toUpperLexem() != "FROM"){
        as_clause();
    }

    if (currentToken().getTokenType() == Token::TK_COMMA){
        getParser()->match(",", Token::TK_COMMA);
        if ((currentToken().getTokenType() == Token::TK_IDENTIFIER) && (currentToken().toUpperLexem() != "FROM")){
            column_reference_list();
        }
    }
}

void SelectExecutor::select_list(){

    if (currentToken().getLexem() == "*"){
        asterisk_name();
        if (currentToken().getTokenType() == Token::TK_COMMA){
            getParser()->match(",", Token::TK_COMMA);
            column_reference_list();
        }
    } else {
        if ((getParser()->getCurrentToken()->toUpperLexem() == "DISTINCT") || (getParser()->getCurrentToken()->toUpperLexem() == "ALL")){
            getParser()->match(getParser()->getCurrentToken()->getLexem(), Token::TK_IDENTIFIER);
        }
        column_reference_list();
    }
}



void SelectExecutor::addTable(string tableName, string alias){

    if (alias.empty()){
        listTables->addNewTable(tableName);
    } else {
        listTables->addNewTable(tableName, alias);
    }
}

void SelectExecutor::addTableAlias(string alias){

    listTables->addNewAlias(alias);
}

int SelectExecutor::countNumberOfProjections(string subselect){

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


DataDictionary * SelectExecutor::dictionary(){

    return dic;
}

vector<string> SelectExecutor::translate(){

    vector<string> answer;
    string ax;

    //Building auxiliary structures and processing internal selects
    getParser()->match(getParser()->getCurrentToken());
    select_list();
    table_expression();

    //Selection rewriting
    morph_complex_to_regular_selection();

    //Projection rewriting
    morph_asterisk_to_regular_columns();
    morph_complex_to_regular_columns();


    //Getting all tokens from token as new answer
    for (int x = 0; x < getParser()->countTokens(); x++){
        if (ax.size() > 0)
            ax += " ";
        ax += getParser()->getToken(x)->getLexem();
    }

    answer.push_back(ax);
    return answer;
}

bool SelectExecutor::isExecutable(){

    return (currentToken().toUpperLexem() == "SELECT");
}

void SelectExecutor::morph_complex_to_regular_selection(){

    Token *t;
    vector<Token *> tList;
    Lexical lex;

    int x = 0;
    int pos = 0;

    try{

        while (getParser()->getToken(x)->toUpperLexem() != "WHERE"){
            x++;
        }
        x++;

        while (getParser()->getToken(x)->toUpperLexem() != "GROUP"){
            //Getting complex predicate
            string tab, alias, col;

            while ((getParser()->getToken(x)->toUpperLexem() != "NEAR") &&
                   (getParser()->getToken(x)->toUpperLexem() != "FAR")){

                if (getParser()->getToken(x)->getTokenType() == Token::TK_PERIOD){
                    alias = col;
                    col.clear();
                    pos = x - 1;
                } else {
                    if (!(getParser()->getToken(x-1)->getTokenType() == Token::TK_PERIOD)){
                        alias.clear();
                        pos = x;
                    }
                    col = getParser()->getToken(x)->getLexem();
                }
                x++;
            }

            if (alias.empty()){
                tab = discoverTableName(col);
            } else {
                tab = listTables->tableName(alias);
                if (tab.empty())
                    tab = alias;
            }

            bool near = false;
            if ((getParser()->getToken(x)->toUpperLexem() == "NEAR") ||
                    (getParser()->getToken(x)->toUpperLexem() == "FAR")){

                if (getParser()->getToken(x)->toUpperLexem() == "NEAR"){
                    near = true;
                }
                x++;

                int aggStandard = 0;
                if ((getParser()->getToken(x)->toUpperLexem() == "SUM") ||
                        (getParser()->getToken(x)->toUpperLexem() == "ALL") ||
                        (getParser()->getToken(x)->toUpperLexem() == "MAX")){
                    if (getParser()->getToken(x)->toUpperLexem() == "SUM"){
                        aggStandard = 1;
                    }
                    if (getParser()->getToken(x)->toUpperLexem() == "ALL"){
                        aggStandard = 2;
                    }
                    if (getParser()->getToken(x)->toUpperLexem() == "MAX"){
                        aggStandard = 3;
                    }
                    x++;
                }


                vector<string> centersName;
                if (getParser()->getToken(x)->getTokenType() == Token::TK_OPEN_BRACE){
                    x++;

                    string aux;

                    int matching = 0;
                    matching++;

                    while (matching > 0){
                        if (getParser()->getToken(x)->getLexem() == "("){
                            matching ++;
                        }
                        if (getParser()->getToken(x)->getLexem() == ")"){
                            matching --;
                        }
                        if (matching > 0){
                            aux.append(getParser()->getToken(x)->getLexem());
                            aux.append(" ");
                        }
                        x++;
                    }

                    vector< vector < string > > subQuery = dictionary()->runSubQuery(aux);
                    for (int x = 0; x < subQuery.size(); x++){
                        centersName.push_back(subQuery[x][0]);
                    }
                    aux.clear();
                } else {
                    string fname;
                    string ename = getParser()->getToken(x)->getLexem();
                    for (int m = 1; m < ename.size()-1; m++){
                        fname += ename[m];
                    }
                    centersName.push_back(lex.toBase64(fname));

                    x++;
                }

                string metricName;
                if (getParser()->getToken(x)->toUpperLexem() == "BY"){
                    x++;
                    metricName = getParser()->getToken(x)->getLexem();
                    x++;
                } else {
                    metricName = dictionary()->getDefaultMetric(tab, col);
                }

                Signature *centerQuery = NULL;
                Signature **centers = NULL;

                if (!aggStandard){
                    centerQuery = getCenterSignature(centersName, metricName, dictionary()->getColumnDataType(tab, col));
                } else {
                    centers = getCentersSignatures(centersName, metricName, dictionary()->getColumnDataType(tab, col));
                }

                Evaluator<Signature> *eval = new Evaluator<Signature>(dictionary()->getDistanceName(dictionary()->getDistanceFunction(tab, col, metricName)), dictionary());
                string idxName = dictionary()->getIndexFile(tab, col, metricName);
                IndexManager *idx = new IndexManager(dictionary(), *eval, IndexManager::PAGE_SIZE, idxName);
                ComplexResult *resultSet = NULL;
                vector<string> inList;

                if (getParser()->getToken(x)->toUpperLexem() == "STOP"){
                    x += 2;
                    double k = atof(getParser()->getToken(x)->getLexem().c_str());
                    x++;

                    bool tie = false;
                    if ((x < getParser()->countTokens()) && (getParser()->getToken(x)->toUpperLexem() == "WITH")){
                        tie = true;
                        x += 3;
                    }

                    if (near){
                        if (!aggStandard){
                            resultSet = idx->nearestSelectQuery(centerQuery, k, tie);
                        } else {
                            if (aggStandard == 1){
                                resultSet = idx->nearestSumSelectQuery(centers, centersName.size(), k, tie);
                            } else {
                                if (aggStandard == 2){
                                    resultSet = idx->nearestAllSelectQuery(centers, centersName.size(), k, tie);
                                } else {
                                    resultSet = idx->nearestMaxSelectQuery(centers, centersName.size(), k, tie);
                                }
                            }
                        }
                    } else {
                        if (!aggStandard){
                            resultSet = idx->farthestSelectQuery(centerQuery, k, tie);
                        } else {
                            if (aggStandard == 1){
                                resultSet = idx->farthestSumSelectQuery(centers, centersName.size(), k, tie);
                            } else {
                                if (aggStandard == 2){
                                    resultSet = idx->farthestAllSelectQuery(centers, centersName.size(), k, tie);
                                } else {
                                    resultSet = idx->farthestMaxSelectQuery(centers, centersName.size(), k, tie);
                                }
                            }
                        }
                    }
                } else {
                    x++;
                    double range = atof(getParser()->getToken(x)->getLexem().c_str());
                    x++;

                    if (near){
                        if (!aggStandard){
                            resultSet = idx->rangeSelectQuery(centerQuery, range);
                        } else {
                            if (aggStandard == 1){
                                resultSet = idx->rangeSumSelectQuery(centers, centersName.size(), range);
                            } else {
                                if (aggStandard == 2){
                                    resultSet = idx->rangeAllSelectQuery(centers, centersName.size(), range);
                                } else {
                                    resultSet = idx->rangeMaxSelectQuery(centers, centersName.size(), range);
                                }
                            }
                        }
                    } else {
                        if (!aggStandard){
                            resultSet = idx->reverseRangeSelectQuery(centerQuery, range);
                        } else {
                            if (aggStandard == 1){
                                resultSet = idx->reverseRangeSumSelectQuery(centers, centersName.size(), range);
                            } else {
                                if (aggStandard == 2){
                                    resultSet = idx->reverseRangeAllSelectQuery(centers, centersName.size(), range);
                                } else {
                                    resultSet = idx->reverseRangeMaxSelectQuery(centers, centersName.size(), range);
                                }
                            }
                        }
                    }
                }

                if (resultSet != NULL){
                    for (int m = 0; m < resultSet->GetNumOfEntries(); m++){
                        Signature *qq = (Signature *)  ((*resultSet)[m]->GetObject());
                        char buf[16];
                        sprintf(buf, "%ld", qq->GetOID());
                        string tmpBuf = buf;
                        inList.push_back(tmpBuf);
                        tmpBuf.clear();
                    }
                }

                if (resultSet != NULL){
                    delete (resultSet);
                }

                idxName.clear();
                if (eval != NULL){
                    delete (eval);
                }
                if (idx != NULL){
                    delete (idx);
                }

                for (int k = pos; k < x; k++){
                    getParser()->removeToken(pos);
                }

                if (centerQuery != NULL){
                    delete (centerQuery);
                }

                if (centers != NULL){
                    delete [] centers;
                }

                if (!alias.empty()){
                    t = new Token(alias, Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                    tList.push_back(t);
                } else {
                    t = new Token(tab, Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                    tList.push_back(t);
                }
                t = new Token(".", Token::TK_PERIOD, Lexical::LK_UNDEFINED);
                tList.push_back(t);
                t = new Token(col, Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                tList.push_back(t);
                t = new Token("IN", Token::TK_IN, Lexical::LK_UNDEFINED);
                tList.push_back(t);
                t = new Token("(", Token::TK_OPEN_BRACE, Lexical::LK_UNDEFINED);
                tList.push_back(t);
                for (int k = 0; k < inList.size(); k++){
                    if (k > 0){
                        t = new Token(",", Token::TK_COMMA, Lexical::LK_UNDEFINED);
                        tList.push_back(t);
                    }
                    t = new Token(inList[k], Token::TK_INTEGER, Lexical::LK_UNDEFINED);
                    tList.push_back(t);
                }
                t = new Token(")", Token::TK_CLOSE_BRACE, Lexical::LK_UNDEFINED);
                tList.push_back(t);

                for (int k = 0; k < inList.size(); k++){
                    inList[k].clear();
                }
                inList.clear();

                for (int k = 0; k < centersName.size(); k++){
                    centersName[k].clear();
                }
                centersName.clear();

                getParser()->insertTokenList(tList, pos);
                x += tList.size();
                tList.clear();

            }
        }
    } catch (...) {
        //Actually, theres no exception.
        //Just, no where clause or no group by clause found
    }
}

void SelectExecutor::morph_asterisk_to_regular_columns(){

    pair<string, string> tableAndAlias;
    vector<string> columns;

    delete (listColumns);
    listColumns = new CheckValidColumns(dictionary());

    int pos = 1;

    if ((getParser()->getToken(pos)->toUpperLexem() == "DISTINCT") || (getParser()->getToken(pos)->toUpperLexem() == "ALL")){
        pos = 2;
    }

    //Get all columns to project
    if (getParser()->getToken(1)->getLexem() == "*"){

        for (int x = 0; x < listTables->getSize() ; x++){
            tableAndAlias = listTables->getTableNameAndAlias(x);

            if (dictionary()->isValidTable(tableAndAlias.first)){
                columns = dictionary()->getAllColumnsFromTable(tableAndAlias.first);
            } else {
                columns = dictionary()->getFieldsName(tableAndAlias.first);
            }


            for (int y = 0; y < columns.size(); y++){

                string tName;
                if (tableAndAlias.second.empty()){
                    tName = tableAndAlias.first;
                } else {
                    tName = tableAndAlias.second;
                }
                listColumns->addNewColumn(tName, columns[y]);
                tName.clear();
            }
        }
        while (getParser()->getToken(pos)->toUpperLexem() != "FROM"){
            getParser()->removeToken(pos);
        }

        //Rewrite the query by replacing * to the real columns name
        Token *t;
        vector<Token *> tList;
        pair<string, string> tabAndColName;

        for (int x = 0; x < listColumns->getSize(); x++){
            if (x > 0){
                t = new Token(",", Token::TK_COMMA, Lexical::LK_UNDEFINED);
                tList.push_back(t);
            }
            tabAndColName = listColumns->getTableNameAndColumnName(x);
            if (!tabAndColName.first.empty()){
                t = new Token(tabAndColName.first, Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                tList.push_back(t);
                t = new Token(".", Token::TK_PERIOD, Lexical::LK_UNDEFINED);
                tList.push_back(t);
            }
            t = new Token(tabAndColName.second, Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
            tList.push_back(t);
        }

        getParser()->insertTokenList(tList, 1);
        tList.clear();
    }
}

string SelectExecutor::discoverTableName(string columnName){

    string answer;
    pair<string, string> tableNameAndAlias;


    for (int x = 0; x < listTables->getSize(); x++){
        tableNameAndAlias = listTables->getTableNameAndAlias(x);
        if (dictionary()->isValidColumn(tableNameAndAlias.first, columnName)){
            answer = tableNameAndAlias.first;
        }
    }
    return answer;
}

Signature * SelectExecutor::getCenterSignature(vector<string> centerNames, string metricName, string dataType){

    Lexical lex;

    string mCode = dictionary()->getMetricCode(metricName);
    vector<string> eNames = dictionary()->getExtractorsName(mCode);

    Signature *center = new Signature();

    for (int x = 0; x < centerNames.size(); x++){
        ComplexObject *cpObj = new ComplexObject(lex.fromBase64(centerNames[x]));
        Signature *one = new Signature();

        try{
            for (int y = 0; y < eNames.size(); y++){

                vector<string> params = dictionary()->getParametersName(dictionary()->getExtractorCode(eNames[y]));

                for (int z = 0; z < params.size(); z++){

                    SirenExtractors<Signature> *ext = new SirenExtractors<Signature>(dataType, eNames[y], dictionary());

                    Signature *aux = ext->extract(cpObj);

                    for (int k = 0; k < aux->getSize(); k++){
                        double weigth = atof(dictionary()->getWeight(mCode, params[z]).c_str());
                        one->set((* (double *) aux->get(k))*weigth);
                    }

                    delete (aux);
                    delete (ext);
                }
                params.clear();
            }

            if (one->getSize() == center->getSize()){
                for (int k = 0; k < one->getSize(); k++){
                    center->set(k, (((* (double *) one->get(k)) + (* (double *) center->get(k)))/2));
                }
            } else {
                for (int k = 0; k < one->getSize(); k++){
                    center->set((* (double *) one->get(k)));
                }
            }

            delete (one);
            delete (cpObj);
        } catch(...){
        }
    }

    return center;
}

Signature** SelectExecutor::getCentersSignatures(vector<string> centerNames, string metricName, string dataType){

    Lexical lex;

    string mCode = dictionary()->getMetricCode(metricName);
    vector<string> eNames = dictionary()->getExtractorsName(mCode);

    Signature** centers = new Signature*[centerNames.size()];

    try{
        for (int x = 0; x < centerNames.size(); x++){

            ComplexObject *cpObj = new ComplexObject(lex.fromBase64(centerNames[x]));
            Signature *one = new Signature();

            for (int y = 0; y < eNames.size(); y++){

                vector<string> params = dictionary()->getParametersName(dictionary()->getExtractorCode(eNames[y]));

                for (int z = 0; z < params.size(); z++){

                    SirenExtractors<Signature> *ext = new SirenExtractors<Signature>(dataType, eNames[y], dictionary());

                    Signature *aux = ext->extract(cpObj);

                    for (int k = 0; k < aux->getSize(); k++){
                        double weigth = atof(dictionary()->getWeight(mCode, params[z]).c_str());
                        one->set((* (double *) aux->get(k))*weigth);
                    }

                    delete (aux);
                    delete (ext);
                }
                params.clear();
            }

            centers[x] = one;

            delete (cpObj);
        }
    } catch(...){
    }

    return centers;
}

void SelectExecutor::morph_complex_to_regular_columns(){

    int x = 1;
    int pos = 0;
    string tab, alias, col;
    Token *t;
    vector<Token *> tList;

    if ((getParser()->getToken(x)->toUpperLexem() == "DISTINCT") || (getParser()->getToken(x)->toUpperLexem() == "ALL")){
        x++;
    }

    while (getParser()->getToken(x)->toUpperLexem() != "FROM"){

        if ((getParser()->getToken(x)->getTokenType() == Token::TK_COMMA) || (getParser()->getToken(x+1)->toUpperLexem() == "FROM")){

            if (getParser()->getToken(x+1)->toUpperLexem() == "FROM"){
                if (!(getParser()->getToken(x-1)->getTokenType() == Token::TK_PERIOD)){
                    alias.clear();
                    pos = x;
                }
                col = getParser()->getToken(x)->getLexem();
            }

            if (alias.empty()){
                tab = discoverTableName(col);
            } else {
                tab = listTables->tableName(alias);
                if (tab.empty())
                    tab = alias;
            }

            if (alias == tab){
                alias.clear();
            }

            if (dictionary()->isComplexAttributeReference(tab, col)){

                for (int m = 0; m <= (x-pos); m++){
                    getParser()->removeToken(pos);
                }
                x = x - (x-pos);

                t = new Token(dictionary()->getTablePrefix(dictionary()->getColumnDataType(tab, col)) + "$" + tab + "_" + col, Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                tList.push_back(t);
                t = new Token(".", Token::TK_PERIOD, Lexical::LK_UNDEFINED);
                tList.push_back(t);
                t = new Token(col, Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                tList.push_back(t);
                t = new Token("AS", Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                tList.push_back(t);
                t = new Token(col, Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                tList.push_back(t);

                morph_complex_to_regular_tables(tab, alias, col);

                getParser()->insertTokenList(tList, pos);
                x = x - 1 + tList.size();
                tList.clear();
            }
        } else {
            if (getParser()->getToken(x)->getTokenType() == Token::TK_PERIOD){
                alias = col;
                col.clear();
                pos = x - 1;
            } else {
                if (!(getParser()->getToken(x-1)->getTokenType() == Token::TK_PERIOD)){
                    alias.clear();
                    pos = x;
                }
                col = getParser()->getToken(x)->getLexem();
            }
        }
        x++;
    }
}

void SelectExecutor::morph_complex_to_regular_tables(string tab, string alias, string col){

    int x = 0;
    bool found = false;
    Token *t;
    vector<Token *> tList;

    while (getParser()->getToken(x)->toUpperLexem() != "FROM"){
        x++;
    }
    x++;

    while (!found){

        if (getParser()->getToken(x)->getLexem() == tab){
            int pos = x;
            if (!alias.empty()){
                if (getParser()->getToken(x+1)->toUpperLexem() == "AS"){
                    pos = x+2;
                } else {
                    pos = x+1;
                }

                if (getParser()->getToken(pos)->getLexem() == alias){
                    found = true;
                }
            } else {
                found = true;
            }

            if (found){

                getParser()->removeToken(x);

                for (int k = x; k < pos; k++){
                    getParser()->removeToken(x);
                }

                t = new Token(tab, Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                tList.push_back(t);

                if (!alias.empty()){
                    t = new Token("AS", Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                    tList.push_back(t);
                    t = new Token(alias, Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                    tList.push_back(t);
                }

                t = new Token("JOIN", Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                tList.push_back(t);

                t = new Token(dictionary()->getTablePrefix(dictionary()->getColumnDataType(tab, col)) + "$" + tab + "_" + col, Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                tList.push_back(t);

                t = new Token("ON", Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                tList.push_back(t);

                if (!alias.empty()){
                    t = new Token(alias, Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                    tList.push_back(t);
                } else {
                    t = new Token(tab, Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                    tList.push_back(t);
                }

                t = new Token(".", Token::TK_PERIOD, Lexical::LK_UNDEFINED);
                tList.push_back(t);

                t = new Token(col, Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                tList.push_back(t);

                t = new Token("=", Token::TK_EQUAL, Lexical::LK_UNDEFINED);
                tList.push_back(t);

                t = new Token(dictionary()->getTablePrefix(dictionary()->getColumnDataType(tab, col)) + "$" + tab + "_" + col, Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                tList.push_back(t);

                t = new Token(".", Token::TK_PERIOD, Lexical::LK_UNDEFINED);
                tList.push_back(t);

                t = new Token(col + "_id", Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                tList.push_back(t);

                getParser()->insertTokenList(tList, x);
                tList.clear();
            }
        }

        x++;
    }
}


