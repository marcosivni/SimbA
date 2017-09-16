#include "ParseTreeFactory.hpp"

ParseTreeFactory::ParseTreeFactory(){

    p = NULL;
    //Criar novos blocos
    flushACTLine();
    flushPTBlock();
    flushParseTree();

    lastSQLStatement.clear();
}

ParseTreeFactory::~ParseTreeFactory(){

    if (p != NULL){
        delete (p);
    }

    joinedTables.clear();
    lastSQLStatement.clear();
}

Parser *ParseTreeFactory::getParser(){

    return p;
}

Token ParseTreeFactory::currentToken(){

    return *(p->getCurrentToken());
}

bool ParseTreeFactory::endOfStatement(){

    return (getParser()->getCurrentPosition() == -1);
}

void ParseTreeFactory::select_list(){

    if (currentToken().getLexem() == "*"){

        actLine.setAttribute("*");
        ptBlock.setOperation(ParseTreeBlock::PROJECTION);
        ptBlock.setLeftEdge(pt.getPTSize() - 1 - mergeCounter);

        setEqualizer(0);
//cout << ptBlock.getOperation() << ptBlock.getTable() << endl;
        pt.addParseTreeBlock((ptBlock), (actLine));

        flushPTBlock();
        flushACTLine();

        getParser()->match("*", Token::TK_MUL);
    } else {
        if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
            column_reference_list();
        }
    }
}

void ParseTreeFactory::column_reference_list(){

    column_reference();

    // Optional AS clause or blank space for renaming
    if (( currentToken().toUpperLexem() == "AS"  ) ||
            ((currentToken().getTokenType() == Token::TK_IDENTIFIER)  &&
             (currentToken().getLexem() != "FROM")) ){
        as_clause();
    }

    if( !endOfStatement() ){
        if (currentToken().getTokenType() == Token::TK_COMMA){
            getParser()->match(",", Token::TK_COMMA);
            if ((currentToken().getTokenType() == Token::TK_IDENTIFIER) && (currentToken().getLexem() != "FROM")){
                column_reference_list();
            }
        }
    }
}

void ParseTreeFactory::column_reference(){

    // Supporting grouping functions - just for single data?
    if ((currentToken().toUpperLexem() == "COUNT") ||
            (currentToken().toUpperLexem() == "MIN") ||
            (currentToken().toUpperLexem() == "MAX") ||
            (currentToken().toUpperLexem() == "SUM") ||
            (currentToken().toUpperLexem() == "AVG") ){

        actLine.setValue(currentToken().getLexem());
        getParser()->match(getParser()->getCurrentToken());
        getParser()->match("(", Token::TK_OPEN_BRACE);
        add_column("atr");
        getParser()->match(")", Token::TK_CLOSE_BRACE);
    } else {
        add_column("atr");
    }
    ptBlock.setOperation(ParseTreeBlock::PROJECTION);
    ptBlock.setLeftEdge(pt.getPTSize() - 1 - getEqualizer());

    setEqualizer(0);
    //cout << ptBlock.getOperation() << ptBlock.getACTReference() << endl;
    pt.addParseTreeBlock((ptBlock), (actLine));

    flushPTBlock();
    flushACTLine();
}

void ParseTreeFactory::as_clause(){

    if (currentToken().toUpperLexem() == "AS"){
        getParser()->match(getParser()->getCurrentToken());
    }    

    if (currentToken().getTokenType() == Token::TK_IDENTIFIER){

        string temp = to_lower (currentToken().getLexem());

        actLine.setOperator("");
        actLine.setValue( temp );


        // When the previous operation was not a read,
        // look for table/atr name on previous ACTLine
        if (pt.getParseTreeBlock( pt.getPTSize() - 1 - getEqualizer() ).getOperation() != ParseTreeBlock::READ ){
            actLine.setAttribute( pt.getACTLine(pt.getParseTreeBlock(pt.getPTSize() - 1).getACTReference()).getAttribute() );
        } else {
            // Else, in case of READ operations look for the table name on the previous PTLine
            actLine.setAttribute( pt.getParseTreeBlock( pt.getPTSize() - 1 - getEqualizer() ).getTable() );
            // And set the joinedTables also            
            joinedTables[ temp ] = 0;
            // And the tables vector
            if (tables.size() > 0) tables.pop_back();
            tables.push_back( temp );            

            // Check for previous references of the table, if they exist (another table, from a previous read) do not remove them, else remove
            int exists = 0;
            for( int i = 0; i < tables.size(); i ++ ){
                if ( tables[i] == actLine.getAttribute() ) exists = 1;
            }
            if (!exists) joinedTables.erase( actLine.getAttribute() );            

        }

        ptBlock.setOperation(ParseTreeBlock::RENAME);
        ptBlock.setLeftEdge( pt.getPTSize() - 1 - getEqualizer() );

        // Setting the rename variable to track renames for future use
        rename[ actLine.getAttribute() ] = actLine.getValue();        

        setEqualizer(0);
//cout << ptBlock.getOperation() << ptBlock.getTable() << endl;
        pt.addParseTreeBlock((ptBlock), (actLine));

        flushPTBlock();
        flushACTLine();

        if ( !endOfStatement() )
            getParser()->match(getParser()->getCurrentToken());

    }
}

void ParseTreeFactory::from_clause(){

    if (currentToken().toUpperLexem() == "FROM"){
        getParser()->match(getParser()->getCurrentToken());
    }
    if ((currentToken().getTokenType() == Token::TK_IDENTIFIER) ||
            (currentToken().getTokenType() == Token::TK_OPEN_BRACE) ){
        table_reference_list();
    }    
    build();    
}

void ParseTreeFactory::table_reference_list(){    

    // Reading a table
    if ( !endOfStatement() ){
        if ( currentToken().toUpperLexem() == "AS" ||
             currentToken().getTokenType() == Token::TK_IDENTIFIER ||
             currentToken().getTokenType() == Token::TK_OPEN_BRACE ) {
           if ( currentToken().toUpperLexem() != "JOIN" &&
             currentToken().toUpperLexem() != "INNER" &&
             currentToken().toUpperLexem() != "LEFT" &&
             currentToken().toUpperLexem() != "RIGHT" &&
             currentToken().toUpperLexem() != "NATURAL" &&
             currentToken().toUpperLexem() != "FULL" &&
             currentToken().toUpperLexem() != "CLOSEST" &&
             currentToken().toUpperLexem() != "NEAREST" &&
             currentToken().toUpperLexem() != "RANGE" &&
             currentToken().toUpperLexem() != "WHERE" )
                read_table();
        }
    }

    // Renaming, if necessary
    if ( !endOfStatement() ){
        if ( currentToken().toUpperLexem() == "AS" ||
             currentToken().getTokenType() == Token::TK_IDENTIFIER ||
             currentToken().getTokenType() == Token::TK_OPEN_BRACE ) {
           if ( currentToken().toUpperLexem() != "JOIN" &&
             currentToken().toUpperLexem() != "INNER" &&
             currentToken().toUpperLexem() != "LEFT" &&
             currentToken().toUpperLexem() != "RIGHT" &&
             currentToken().toUpperLexem() != "NATURAL" &&
             currentToken().toUpperLexem() != "FULL" &&
             currentToken().toUpperLexem() != "CLOSEST" &&
             currentToken().toUpperLexem() != "NEAREST" &&
             currentToken().toUpperLexem() != "RANGE" &&
             currentToken().toUpperLexem() != "WHERE" )
                as_clause();
        }
    }

//    //cout << currentToken().toUpperLexem() << endl;


    // If a comma is found, read the next table
    if (!endOfStatement()){
        if (currentToken().getTokenType() == Token::TK_COMMA){
            getParser()->match(",", Token::TK_COMMA);
            if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
                table_reference_list();
            }
        }
        // Else, if it is not a comma nor a WHERE, should be another JOIN
        else if(currentToken().getLexem() != "WHERE"){            
            join_reference_list();            
        }
    }
}

void ParseTreeFactory::join_reference_list(){    

    if (!endOfStatement()){

        // If there is a non-similarity JOIN
        if (currentToken().toUpperLexem() == "JOIN" ||
                currentToken().toUpperLexem() == "INNER" ||
                currentToken().toUpperLexem() == "LEFT" ||
                currentToken().toUpperLexem() == "RIGHT" ||
                currentToken().toUpperLexem() == "FULL" ||
                currentToken().toUpperLexem() == "NATURAL"
                ){
            getParser()->match(getParser()->getCurrentToken());
            process_join();
        } else {
            // Now similarity JOINs
            if(currentToken().toUpperLexem() == "CLOSEST" ||
                    currentToken().toUpperLexem() == "NEAREST" ||
                    currentToken().toUpperLexem() == "RANGE"){
                process_similarity_join();
            }
        }
    }

    table_reference_list();

}

// Processing regular, non-similarity, JOINs
void ParseTreeFactory::process_join(){    

    // Remove the possible keywords from all the possible JOINs
    if (currentToken().toUpperLexem() == "OUTER" ||
            currentToken().toUpperLexem() == "JOIN"){
        getParser()->match(getParser()->getCurrentToken());
    }
    if (currentToken().toUpperLexem() == "JOIN"){
        getParser()->match(getParser()->getCurrentToken());
    }    

    // Reading the second column that will be JOINed
    read_table();    

    // Optional AS clause or blank space for renaming
    if ((currentToken().toUpperLexem() == "AS" || currentToken().getTokenType() == Token::TK_IDENTIFIER) &&
            (currentToken().toUpperLexem()) != "ON") {
        as_clause();
    }

    // Remove the ON keyword
    if (currentToken().toUpperLexem() == "ON"){
        getParser()->match(getParser()->getCurrentToken());
    }

    // Process the JOIN condition after the ON clause
    // Traditional JOIN
    actLine.setAttributeType(ACTLine::ATTRIBUTE_TRADITIONAL);
    // Setting the first column
    add_column("atr");
    // Setting the operation
    actLine.setOperator(currentToken().getLexem());
    getParser()->match(getParser()->getCurrentToken());
    // Setting the second column (on value)
    add_column("val");

    // Processing the references properly and adding to the Parse Tree
    ptBlock.setOperation(ParseTreeBlock::JOIN);
//    ptBlock.setLeftEdge( get_ref( get_table( actLine.getAttribute() ) ) );

    //merge counter
    setEqualizer(0);

    set_join();

    joinMap.push_back( make_pair ( ptBlock, actLine ) );

//cout << ptBlock.getOperation() << ptBlock.getTable() << endl;
//    pt.addParseTreeBlock((ptBlock), (actLine));

    flushPTBlock();
    flushACTLine();
}

void ParseTreeFactory::process_similarity_join(){    

    int operation;

    // Check for CLOSEST/NEAREST or RANGE
    if (currentToken().toUpperLexem() == "CLOSEST"){
        getParser()->match(getParser()->getCurrentToken());
        // Setting the default k value for the JOIN
        operation = ParseTreeBlock::CLOSEST_JOIN;
    } else {
        if (currentToken().toUpperLexem() == "NEAREST"){
            getParser()->match(getParser()->getCurrentToken());
            // Setting the default k value for the JOIN
            operation = ParseTreeBlock::NEAREST_JOIN;
        } else {
            if(currentToken().toUpperLexem() == "RANGE"){
                getParser()->match(getParser()->getCurrentToken());
                // Setting the default r value for the JOIN
                operation = ParseTreeBlock::RANGE_JOIN;
            }
        }
    }    

    // Remove JOIN keyword
    if (currentToken().toUpperLexem() == "JOIN"){
        getParser()->match(getParser()->getCurrentToken());
    }

    // Reading the second column that will be JOINed
    read_table();

    //Check for language default definitions
    //@todo Discuss about need for parameter (knn or range) specification

    // Optional AS clause or blank space for renaming
    if ((currentToken().toUpperLexem() == "AS" || currentToken().getTokenType() == Token::TK_IDENTIFIER) &&
            (currentToken().toUpperLexem()) != "ON"){
        as_clause();
    }    

    // Remove the ON keyword
    if (currentToken().toUpperLexem() == "ON"){
        getParser()->match(getParser()->getCurrentToken());
    }

    // Process the JOIN condition after the ON clause
    // similarity JOIN
    actLine.setAttributeType(ACTLine::ATTRIBUTE_MONOLITHIC);

    // Setting the first column
    add_column("atr");

    // Setting the operation {NEAR | FAR}
    if (currentToken().toUpperLexem() == "NEAR"){
        near_predicate();
    } else {
        if (currentToken().toUpperLexem() == "FAR") {
            far_predicate();
        }
    }        

    // Processing the references properly and adding to the Parse Tree
    ptBlock.setOperation(operation);

    setEqualizer(0);    

    set_join();    

    joinMap.push_back( make_pair ( ptBlock, actLine ) );

//    pt.addParseTreeBlock((ptBlock), (actLine));

    flushPTBlock();
    flushACTLine();    
}

void ParseTreeFactory::where_clause(){    

    if (currentToken().toUpperLexem() == "WHERE"){
        getParser()->match(getParser()->getCurrentToken());
        if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
            search_condition();
        }
    }
}

void ParseTreeFactory::search_condition(){    


    if (!endOfStatement()){
        if (currentToken().toUpperLexem() == "NOT") {
            // TO DO + IN + EXISTS
            getParser()->match(getParser()->getCurrentToken());
            if (currentToken().getTokenType() == Token::TK_IDENTIFIER) {
                search_condition();
            }
        }

        if (!endOfStatement()){
            if(currentToken().getTokenType() == Token::TK_OPEN_BRACE)
                getParser()->match("(",Token::TK_OPEN_BRACE);
        }

        int s1 = getEqualizer();

        add_column("atr");        
        predicate();        

        int s2 = getEqualizer();

        if (!endOfStatement()){
            if(currentToken().getTokenType() == Token::TK_CLOSE_BRACE)
                getParser()->match(")",Token::TK_CLOSE_BRACE);
        }        

        ptBlock.setLeftEdge(  pt.getPTSize() - 1 - getEqualizer() );               

        // Checking if its an implicit JOIN
        if ( joinedTables.count( get_table( actLine.getValue() ) ) &&
             joinedTables.count( get_table( actLine.getAttribute() ) ) &&
             get_table( actLine.getValue() ) != get_table( actLine.getAttribute() )
             ){

            if ( joinedTables[ get_table( actLine.getValue() ) ] == 0 ||
                 joinedTables[ get_table( actLine.getAttribute() ) ] == 0
                 ){
                    ptBlock.setOperation(ParseTreeBlock::JOIN);
                    set_join();
            }
        }        

        //cout << ptBlock.getOperation() << ptBlock.getTable() << endl;
        if( s1 >= s2 ){
            pt.addParseTreeBlock((ptBlock), (actLine));
            setEqualizer(0);
        }

        flushPTBlock();
        flushACTLine();

        if (!endOfStatement()){
            if (((currentToken().toUpperLexem() == "AND") ||
                 (currentToken().toUpperLexem() == "OR"))) {
                getParser()->match(getParser()->getCurrentToken());
                if (currentToken().getTokenType() == Token::TK_IDENTIFIER) {
                    search_condition();
                }
            }
        }
    }
}

void ParseTreeFactory::predicate(){

    actLine.setAttributeType(ACTLine::ATTRIBUTE_TRADITIONAL);
    ptBlock.setOperation(ParseTreeBlock::SELECT);
    if(currentToken().getTokenType() == Token::TK_IDENTIFIER){

        if( currentToken().getLexem() == "NEAR" || currentToken().getLexem() == "FAR" ){
            actLine.setAttributeType(ACTLine::ATTRIBUTE_MONOLITHIC);
            ptBlock.setOperation(ParseTreeBlock::SIMILARITY_SELECT);
            //.............................
            // near_predicate
            //.............................
            if (currentToken().toUpperLexem() == "NEAR"){
                near_predicate();
            } else {
                //.............................
                // far_predicate
                //.............................
                if (currentToken().toUpperLexem() == "FAR"){
                    far_predicate();
                }
            }
        }else{

            string aux = "";
            if( currentToken().getLexem() == "NOT" ){
                aux.append(getParser()->getCurrentToken()->getLexem());
                aux.append(" ");
                getParser()->match(getParser()->getCurrentToken());
            }

            if( currentToken().getLexem() == "IN" ){
                aux.append(getParser()->getCurrentToken()->getLexem());
                getParser()->match(getParser()->getCurrentToken());
            }

            if( currentToken().getLexem() == "BETWEEN" ){
                aux.append(getParser()->getCurrentToken()->getLexem());
                getParser()->match(getParser()->getCurrentToken());

                string op;
                if(currentToken().getTokenType() == Token::TK_OPEN_BRACE){
                    getParser()->match("(",Token::TK_OPEN_BRACE);
                    op.append("(");
                }

                value_expression();
                op.append(actLine.getValue());
                op.append(" ");
                // AND || OR
                op.append(getParser()->getCurrentToken()->getLexem());
                getParser()->match( getParser()->getCurrentToken() );
                op.append(" ");
                value_expression();
                op.append(actLine.getValue());


                if( !endOfStatement() ){
                    if(currentToken().getTokenType() == Token::TK_CLOSE_BRACE){
                        getParser()->match(")",Token::TK_CLOSE_BRACE);
                        op.append(")");
                    }
                }

                actLine.setOperator(aux);
                actLine.setValue(op);

            } else{
                actLine.setOperator(aux);
                add_column("val");
            }

            aux.clear();

        }


    }
    else{
        // equals operator
        if (currentToken().getLexem() == "=") {
                actLine.setOperator(getParser()->getCurrentToken()->getLexem());
                getParser()->match("=", Token::TK_EQUAL);
                if (currentToken().getTokenType() == Token::TK_IDENTIFIER || currentToken().getTokenType() == Token::TK_OPEN_BRACE) {
                    add_column("val");
                } else {
                    value_expression();
                }
        } else {
            // not equals operator
            if (currentToken().getLexem() == "!=") {
                    actLine.setOperator(getParser()->getCurrentToken()->getLexem());
                    getParser()->match("!=", Token::TK_NE);
                    if (currentToken().getTokenType() == Token::TK_IDENTIFIER  || currentToken().getTokenType() == Token::TK_OPEN_BRACE){
                        add_column("val");
                    } else {
                        value_expression();
                    }
            } else {
                // LIKE operator
                if (currentToken().toUpperLexem() == "LIKE") {
                        actLine.setOperator(getParser()->getCurrentToken()->getLexem());
                        getParser()->match(getParser()->getCurrentToken());
                        value_expression();
                } else {
                    // NOT EQUALS operator
                    if (currentToken().getLexem() == "<>") {
                            actLine.setOperator(getParser()->getCurrentToken()->getLexem());
                            getParser()->match("<>", Token::TK_N2);
                            if (currentToken().getTokenType() == Token::TK_IDENTIFIER  || currentToken().getTokenType() == Token::TK_OPEN_BRACE){
                                add_column("val");
                            } else {
                                value_expression();
                            }
                    } else {
                        // less than operator
                        if (currentToken().getLexem() == "<") {
                                actLine.setOperator(getParser()->getCurrentToken()->getLexem());
                                getParser()->match("<", Token::TK_LQ);
                                if (currentToken().getTokenType() == Token::TK_IDENTIFIER  || currentToken().getTokenType() == Token::TK_OPEN_BRACE){
                                    add_column("val");
                                } else {
                                    value_expression();
                                }
                        } else {
                            // less than or equals operator
                            if (currentToken().getLexem() == "<=") {
                                    actLine.setOperator(getParser()->getCurrentToken()->getLexem());
                                    getParser()->match("<=", Token::TK_LE);
                                    if (currentToken().getTokenType() == Token::TK_IDENTIFIER  || currentToken().getTokenType() == Token::TK_OPEN_BRACE){
                                        add_column("val");
                                    } else {
                                        value_expression();
                                    }
                            } else {
                                // greater than operator
                                if (currentToken().getLexem() == ">") {
                                        actLine.setOperator(getParser()->getCurrentToken()->getLexem());
                                        getParser()->match(">", Token::TK_GQ);
                                        if (currentToken().getTokenType() == Token::TK_IDENTIFIER || currentToken().getTokenType() == Token::TK_OPEN_BRACE) {
                                            add_column("val");
                                        } else {
                                            value_expression();
                                        }
                                } else {
                                    // greater than or equals operator
                                    if (currentToken().getLexem() == ">=") {
                                            actLine.setOperator(getParser()->getCurrentToken()->getLexem());
                                            getParser()->match(">=", Token::TK_GE);
                                            if (currentToken().getTokenType() == Token::TK_IDENTIFIER || currentToken().getTokenType() == Token::TK_OPEN_BRACE){
                                                add_column("val");
                                            } else {
                                                value_expression();
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

void ParseTreeFactory::value_expression(){

    // A valid value_expression can be: a string, an integer or a floating point
    actLine.setValue(currentToken().getLexem());
    if (currentToken().getTokenType() == Token::TK_STRING){
        getParser()->match(getParser()->getCurrentToken());
    } else {
        if (currentToken().getTokenType() == Token::TK_INTEGER){
            getParser()->match(getParser()->getCurrentToken());
        } else {
            if (currentToken().getTokenType() == Token::TK_FLOATING_POINT){
                getParser()->match(getParser()->getCurrentToken());
            }
        }
    }
}

void ParseTreeFactory::near_predicate(){

    if (currentToken().toUpperLexem() == "NEAR") {
        getParser()->match(getParser()->getCurrentToken());
    }

    // Optional similarity grouping
    if (currentToken().getTokenType() == Token::TK_IDENTIFIER) {
        if ((currentToken().toUpperLexem() == "SUM") ||
                (currentToken().toUpperLexem() == "AVG") ||
                (currentToken().toUpperLexem() == "ALL") ||
                (currentToken().toUpperLexem() == "EVERY") ||
                (currentToken().toUpperLexem() == "ANY") ||
                (currentToken().toUpperLexem() == "MAX")) {
            actLine.setParameter(currentToken().getLexem(), "1");
            getParser()->match(getParser()->getCurrentToken());
        }
    }

    // TO DO -> PROCESS image_definition PROPERLY HERE
    add_column("val");

    // Optional BY clause
    if ((getParser()->getCurrentPosition() != -1) && (currentToken().toUpperLexem() == "BY")) {
        getParser()->match(getParser()->getCurrentToken());
        metric_name();
    }

    // Optional RANGE clause
    if ((getParser()->getCurrentPosition() != -1) && (currentToken().toUpperLexem() == "RANGE")) {
        actLine.setOperator("RQ");
        getParser()->match(getParser()->getCurrentToken());
        val_range();
    }

    // Optional STOP AFTER clause
    if ((getParser()->getCurrentPosition() != -1) && (currentToken().toUpperLexem() == "STOP")) {
        getParser()->match(getParser()->getCurrentToken());
        if (currentToken().toUpperLexem() == "AFTER") {
            getParser()->match(getParser()->getCurrentToken());
            actLine.setOperator("KNN");
            k_nearest();
            // optional WITH clause
            if ((getParser()->getCurrentPosition() != -1) && (currentToken().toUpperLexem() == "WITH")) {
                getParser()->match(getParser()->getCurrentToken());
                tie_list();
            }
        }
    }

    // If the operator is blank, we set the default RQ, r = 0 query
    if(actLine.getOperator().empty()){
        actLine.setOperator("RQ");
        actLine.setParameter("r","0");
    }
}

void ParseTreeFactory::val_range(){

    actLine.setParameter("r", currentToken().getLexem());
    if ((currentToken().getTokenType() == Token::TK_INTEGER) ||
            (currentToken().getTokenType() == Token::TK_FLOATING_POINT)) {
        getParser()->match(getParser()->getCurrentToken());
    }
}

void ParseTreeFactory::metric_name(){

    actLine.setParameter("BY", currentToken().getLexem());
    if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
        getParser()->match(getParser()->getCurrentToken());
    }
}

void ParseTreeFactory::tie_list(){

    actLine.setParameter("TIE_LIST","1");
    if (currentToken().toUpperLexem() == "TIE") {
        getParser()->match(getParser()->getCurrentToken());
        if (currentToken().toUpperLexem() == "LIST") {
            getParser()->match(getParser()->getCurrentToken());
        }
    }
}

void ParseTreeFactory::k_nearest(){

    actLine.setParameter("k", currentToken().getLexem());
    if (currentToken().getTokenType() == Token::TK_INTEGER) {
        getParser()->match(getParser()->getCurrentToken());
    }
}

void ParseTreeFactory::far_predicate(){

    if (currentToken().toUpperLexem() == "FAR") {
        getParser()->match(getParser()->getCurrentToken());
    }

    // Optional similarity grouping
    if (currentToken().getTokenType() == Token::TK_IDENTIFIER) {
        if ((currentToken().toUpperLexem() == "SUM") ||
                (currentToken().toUpperLexem() == "AVG") ||
                (currentToken().toUpperLexem() == "ALL") ||
                (currentToken().toUpperLexem() == "EVERY") ||
                (currentToken().toUpperLexem() == "ANY") ||
                (currentToken().toUpperLexem() == "MAX")) {
            actLine.setParameter(currentToken().getLexem(), "1");
            getParser()->match(getParser()->getCurrentToken());
        }
    }

    // TO DO -> PROCESS image_definition PROPERLY HERE
    add_column("val");

    // Optional BY clause
    if ((getParser()->getCurrentPosition() != -1) && (currentToken().toUpperLexem() == "BY")) {
        getParser()->match(getParser()->getCurrentToken());
        metric_name();
    }
    // Optional RANGE clause
    if ((getParser()->getCurrentPosition() != -1) && (currentToken().toUpperLexem() == "RANGE")) {
        getParser()->match(getParser()->getCurrentToken());
        actLine.setOperator("FQ");
        val_range();
    }
    // Optional STOP AFTER clause
    if ((getParser()->getCurrentPosition() != -1) && (currentToken().toUpperLexem() == "STOP")) {
        getParser()->match(getParser()->getCurrentToken());
        if (currentToken().toUpperLexem() == "AFTER") {
            getParser()->match(getParser()->getCurrentToken());
            actLine.setOperator("KFN");
            k_nearest();
            // optional WITH clause
            if ((getParser()->getCurrentPosition() != -1) && (currentToken().toUpperLexem() == "WITH")) {
                getParser()->match(getParser()->getCurrentToken());
                tie_list();
            }
        }
    }

    // If the operator is blank, we set the default RQ, r = 0 query
    if(actLine.getOperator().empty()){
        actLine.setOperator("FQ");
        actLine.setParameter("r", "0");
    }
}

void ParseTreeFactory::select_specification(){

    if (currentToken().toUpperLexem() == "SELECT"){
        getParser()->match(getParser()->getCurrentToken());
        // optional quantifier
        if ((currentToken().toUpperLexem() == "DISTINCT") || (currentToken().toUpperLexem() == "ALL")){
            // How to store DISTINCT ??? Parameter ? Operation ?
            getParser()->match(getParser()->getCurrentToken());
        }
        select_list();
    }
}

string ParseTreeFactory::add_column(string param){


    if( !endOfStatement() ){
        if (currentToken().getLexem() == "(")
            getParser()->match("(",Token::TK_OPEN_BRACE);
    }

    string aux;

    if (currentToken().toUpperLexem() == "SELECT"){

        aux = par_processing();

        ptBlock.setOperation(ParseTreeBlock::SELECT);
        actLine.setValueType(ACTLine::VT_TREE);
        ptBlock.setLeftEdge(pt.getPTSize() - 1 - getEqualizer());

        setEqualizer(0);

        ParseTreeFactory factory;
        ParseTree *sub_select = factory.canonicalParseTree(aux);

        // Value will be the size of the sub-select Parse Tree as a string
        stringstream ss;
        ss << sub_select->getPTSize();

        actLine.setValue(ss.str());

        //cout << ptBlock.getOperation() << ptBlock.getTable() << endl;
        pt.addParseTreeBlock(ptBlock,actLine);

        // Merging both trees
        merge_pt(sub_select);        

    } else {        

        aux = to_lower( currentToken().getLexem() );
        getParser()->match(getParser()->getCurrentToken());        

        if( actLine.getOperator() == "IN" || actLine.getOperator() == "NOT IN" ){
            if( !endOfStatement() ){
                while ( currentToken().getTokenType() == Token::TK_PERIOD ||
                        currentToken().getTokenType() == Token::TK_COMMA){
                    // If the token is a TK_PERIOD or a COMMA then the next token must be an identifier
                    aux.append(getParser()->getCurrentToken()->getLexem());
                    if ( currentToken().getTokenType() == Token::TK_PERIOD ) getParser()->match(".", Token::TK_PERIOD);
                    if ( currentToken().getTokenType() == Token::TK_COMMA ) getParser()->match(",", Token::TK_COMMA);
                    if (currentToken().getTokenType() == Token::TK_IDENTIFIER ||
                        currentToken().getTokenType() == Token::TK_INTEGER ||
                        currentToken().getTokenType() == Token::TK_FLOATING_POINT){
                        aux.append(getParser()->getCurrentToken()->getLexem());
                        getParser()->match(getParser()->getCurrentToken());
                    }
                    if( endOfStatement() ) break;
                }
            }
        }
        else{
            if( !endOfStatement() ){
                while ( currentToken().getTokenType() == Token::TK_PERIOD){
                    // If the token is a TK_PERIOD or a COMMA then the next token must be an identifier
                    aux.append(getParser()->getCurrentToken()->getLexem());
                    getParser()->match(".", Token::TK_PERIOD);
                    if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
                        aux.append(getParser()->getCurrentToken()->getLexem());
                        getParser()->match(getParser()->getCurrentToken());
                    }
                    if( endOfStatement() ) break;
                }
            }
        }


        if (param == "val"){
            actLine.setValue(aux);
        } else {
            if (param == "atr") {
                actLine.setAttribute(aux);
            }
        }
    }        

    if( !endOfStatement() ){
        if (currentToken().getLexem() == ")") {
            getParser()->match(")",Token::TK_CLOSE_BRACE);
        }
    }

    return aux;
}

void ParseTreeFactory::read_table(){

    if (!endOfStatement()){
        if (currentToken().getLexem() == "("){
            getParser()->match("(",Token::TK_OPEN_BRACE);
        }
    }

    // Checking if its a sub-select
    if (currentToken().toUpperLexem() == "SELECT"){
        string aux = par_processing();        

        ptBlock.setTable(aux);
        ptBlock.setOperation(ParseTreeBlock::READ);
        actLine.setValueType(ACTLine::VT_TREE);
        ptBlock.setLeftEdge(pt.getPTSize() - 1 - getEqualizer());
        tables.push_back( aux );

        setEqualizer(0);

        ParseTreeFactory factory;
        ParseTree *sub_select = factory.canonicalParseTree(aux);

        // Value will be the size of the sub-select Parse Tree as a string
        stringstream ss;
        ss << sub_select->getPTSize();

        actLine.setValue(ss.str());

        //cout << ptBlock.getOperation() << ptBlock.getTable() << endl;
        pt.addParseTreeBlock(ptBlock,actLine);

        // Merging both trees
        merge_pt(sub_select);


    } else {
        string table = to_lower( currentToken().getLexem() );
        ptBlock.setTable( table );
        joinedTables[ table ] = 0;
        ptBlock.setOperation(ParseTreeBlock::READ);        
        pt.addParseTreeBlock(ptBlock);
        tables.push_back( table );
        getParser()->match(getParser()->getCurrentToken());        
    }    

    if (!endOfStatement()){
        if (currentToken().getLexem() == ")" ){
            getParser()->match(")", Token::TK_CLOSE_BRACE);
        }
    }

    flushPTBlock();
    flushACTLine();    
}

void ParseTreeFactory::groupby_clause(){

    if (currentToken().toUpperLexem() == "GROUP"){
        getParser()->match(getParser()->getCurrentToken());
        if (currentToken().toUpperLexem() == "BY"){
            getParser()->match(getParser()->getCurrentToken());

            if ( currentToken().getTokenType() == Token::TK_OPEN_BRACE ) getParser()->match(getParser()->getCurrentToken());

            ptBlock.setOperation(ParseTreeBlock::AGGREGATION);
            ptBlock.setLeftEdge(pt.getPTSize() - 1 - getEqualizer());
            setEqualizer(0);
            actLine.setAttributeType(ACTLine::ATTRIBUTE_TRADITIONAL);

            string aux = currentToken().getLexem();            
            getParser()->match(getParser()->getCurrentToken());

            if( !endOfStatement()){
                while (currentToken().getTokenType() == Token::TK_PERIOD ||
                       currentToken().getTokenType() == Token::TK_COMMA){
                    // If the token is a TK_PERIOD or TK_COMMA then the next token must be an identifier
                    aux.append(currentToken().getLexem());
                    if( !endOfStatement()) getParser()->match(".", Token::TK_PERIOD);
                    if( !endOfStatement()) getParser()->match(",", Token::TK_COMMA);
                    if (currentToken().getTokenType() == Token::TK_IDENTIFIER ||
                            currentToken().getTokenType() == Token::TK_MUL){
                        aux.append(currentToken().getLexem());
                        getParser()->match(getParser()->getCurrentToken());
                    }
                    if( endOfStatement() ) break;
                }
            }

            if( !endOfStatement()){
                if ( currentToken().getTokenType() == Token::TK_CLOSE_BRACE ) getParser()->match(getParser()->getCurrentToken());
            }

            actLine.setAttribute(aux);
            aux.clear();
//cout << ptBlock.getOperation() << ptBlock.getTable() << endl;
            pt.addParseTreeBlock((ptBlock), (actLine));

            flushPTBlock();
            flushACTLine();
        }
    }
}

void ParseTreeFactory::having_clause(){

    if (currentToken().toUpperLexem() == "HAVING"){
        getParser()->match(getParser()->getCurrentToken());
        if ((currentToken().toUpperLexem() == "COUNT") ||
                (currentToken().toUpperLexem() == "MIN") ||
                (currentToken().toUpperLexem() == "MAX") ||
                (currentToken().toUpperLexem() == "SUM") ||
                (currentToken().toUpperLexem() == "AVG")) {
            agg_functions();
        }
    }
}

void ParseTreeFactory::agg_functions(){

    ptBlock.setOperation(ParseTreeBlock::SELECT);
    ptBlock.setLeftEdge(pt.getPTSize() - 1 - getEqualizer());
    setEqualizer(0);
    actLine.setAttributeType(ACTLine::ATTRIBUTE_TRADITIONAL);

    string aux = "";

    // Adding the aggregator
    aux.append(currentToken().getLexem());
    getParser()->match(getParser()->getCurrentToken());

    // Adding the (
    aux.append("(");
    if (currentToken().getTokenType() == Token::TK_OPEN_BRACE){
        getParser()->match("(", Token::TK_OPEN_BRACE);
    }

    aux.append(add_column("atr"));
    aux.append(")");

    if( !endOfStatement()){
        if ( currentToken().getTokenType() == Token::TK_CLOSE_BRACE ) getParser()->match(")", Token::TK_CLOSE_BRACE);
    }

    // Setting attribute ::= agg_function(col_name)
    actLine.setAttribute(aux);
    aux.clear();

    // Setting operation
    actLine.setOperator(currentToken().getLexem());
    getParser()->match(getParser()->getCurrentToken());

    // Setting Value
    actLine.setValue(currentToken().getLexem());
    getParser()->match(getParser()->getCurrentToken());
//cout << ptBlock.getOperation() << ptBlock.getTable() << endl;
    pt.addParseTreeBlock((ptBlock), (actLine));

    flushPTBlock();
    flushACTLine();

    if( !endOfStatement() ){
        if (getParser()->getCurrentToken()->getLexem() == "AND" ||
                getParser()->getCurrentToken()->getLexem() == "OR"){
            getParser()->match(getParser()->getCurrentToken());
            agg_functions();
        }
    }
}

void ParseTreeFactory::orderby_clause(){

    // Order By is using a single entry for the whole aggregation, even if it takes several columns
    if (currentToken().toUpperLexem() == "ORDER"){
        getParser()->match(getParser()->getCurrentToken());
        if (currentToken().toUpperLexem() == "BY"){
            getParser()->match(getParser()->getCurrentToken());

            ptBlock.setOperation(ParseTreeBlock::PROJECTION);
            ptBlock.setLeftEdge(pt.getPTSize() - 1 - getEqualizer());
            setEqualizer(0);
            actLine.setAttributeType(ACTLine::ATTRIBUTE_TRADITIONAL);

            if( !endOfStatement() ){
                if( currentToken().getTokenType() == Token::TK_OPEN_BRACE );
                    getParser()->match("(", Token::TK_OPEN_BRACE);
            }

            string aux = "";
            actLine.setValue( "ORDER BY" );
            aux.append(currentToken().getLexem());
            getParser()->match(getParser()->getCurrentToken());

            if( !endOfStatement() ){
                while (currentToken().getTokenType() == Token::TK_PERIOD ||
                       currentToken().getTokenType() == Token::TK_COMMA){
                    // If the token is a TK_PERIOD or TK_COMMA then the next token must be an identifier
                    aux.append(currentToken().getLexem());
                    if( !endOfStatement() ) getParser()->match(".", Token::TK_PERIOD);
                    if( !endOfStatement() ) getParser()->match(",", Token::TK_COMMA);
                    if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
                        aux.append(currentToken().getLexem());
                        getParser()->match(getParser()->getCurrentToken());
                    }
                    if( endOfStatement() ) break;
                }
            }

            if( !endOfStatement() ){
                if( currentToken().getTokenType() == Token::TK_CLOSE_BRACE );
                    getParser()->match(")", Token::TK_CLOSE_BRACE);
            }



            if( !endOfStatement() ){
                if( currentToken().getTokenType() == Token::TK_IDENTIFIER && currentToken().getLexem() == "DESC"){
                    getParser()->match(getParser()->getCurrentToken());
                    actLine.setParameter( "DESC" , "1" );
                }
                else{
                    if( currentToken().getTokenType() == Token::TK_IDENTIFIER && currentToken().getLexem() == "ASC"){
                        getParser()->match(getParser()->getCurrentToken());
                    }
                    actLine.setParameter( "ASC" , "1" );
                }                
            } else actLine.setParameter( "ASC" , "1" );

            actLine.setAttribute(aux);
            aux.clear();
//cout << ptBlock.getOperation() << ptBlock.getTable() << endl;
            pt.addParseTreeBlock((ptBlock), (actLine));

            flushPTBlock();
            flushACTLine();
        }
    }
}

string ParseTreeFactory::par_processing(){

    string aux = "";
    int matching = 0;

    // Skip the parenthesis    
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

string ParseTreeFactory::get_table(string table){

    int pos = -1;
    pos = table.find_first_of(".");
    string aux = table.substr(0, pos);

    return aux;
}

void ParseTreeFactory::cartesian_product(){

    //cout << "Cartesiano" << endl;

    int ptref = -1;   

    for (std::map<string,bool>::iterator it = joinedTables.begin(); it!=joinedTables.end(); ++it){

//        //cout << it->first << "  " << it->second << endl;

        if (joinedTables.size() > 1){            
            ptref = -1;
            if (!it->second){
                ptref = get_ref( it->first );
            }

            if ( ptref == (pt.getPTSize() - 1 - getEqualizer()) ){//
                ptref = -1;
            }

            if (ptref >= 0){
                ptBlock.setOperation(ParseTreeBlock::CARTESIAN_PRODUCT);
                ptBlock.setLeftEdge(pt.getPTSize() - 1 - getEqualizer());

                setEqualizer(0);

                ptBlock.setRightEdge(ptref);
                //cout << ptBlock.getOperation() << ptBlock.getTable() << endl;
                pt.addParseTreeBlock(ptBlock);

                // Marking the new junctions                
                joinedTables[ it->first ] = 1;
                if ( pt.getParseTreeBlock( ptBlock.getLeftEdge() ).getOperation() == ParseTreeBlock::READ )
                    joinedTables[ pt.getParseTreeBlock( ptBlock.getLeftEdge() ).getTable() ] = 1;
                else if ( pt.getParseTreeBlock( ptBlock.getLeftEdge() ).getOperation() == ParseTreeBlock::RENAME ){
                    joinedTables[ pt.getACTLine( pt.getParseTreeBlock( ptBlock.getLeftEdge() ).getACTReference() ).getValue() ] = 1;
                }

                flushACTLine();
                flushPTBlock();
            }
        }
    }    

    // Sweeps the branches vector to see if all branches connect
    vector < vector < string > > tracker;

    for (int i = 0; i < pt.getPTSize(); i++){

        if ( pt.getParseTreeBlock(i).getACTReference() >= 0 ){
            // Skipping sub-selects if necessary
            if ( pt.getACTLine( pt.getParseTreeBlock(i).getACTReference() ).getValueType() == ACTLine::VT_TREE ){
                std::istringstream is(pt.getACTLine(pt.getParseTreeBlock(i).getACTReference()).getValue());
                int value;
                is >> value;
                i = i + value;

            }
        }

        if (pt.getParseTreeBlock(i).getOperation() == ParseTreeBlock::JOIN ||
            pt.getParseTreeBlock(i).getOperation() == ParseTreeBlock::RANGE_JOIN ||
            pt.getParseTreeBlock(i).getOperation() == ParseTreeBlock::CLOSEST_JOIN ||
            pt.getParseTreeBlock(i).getOperation() == ParseTreeBlock::NEAREST_JOIN
            ){

            string t1 = get_table( pt.getACTLine( pt.getParseTreeBlock(i).getACTReference()).getAttribute() );
            string t2 = get_table( pt.getACTLine( pt.getParseTreeBlock(i).getACTReference()).getValue() );

            if ( tracker.size() == 0 ){
                vector< string > temp;
                temp.push_back( t1 ) ;
                temp.push_back( t2 );
                tracker.push_back(temp);
                temp.clear();

            }
            else{

                int needNew = 0;
                int inserted = 0;
                for (int j = 0; j < tracker.size(); j++){
                    for (int k = 0; k < tracker[j].size(); k++){
                        if( tracker[j][k] == t1 || tracker[j][k] == t2  ){
                            tracker[j].push_back(t1);
                            tracker[j].push_back(t2);

                            inserted = 1;

                        }
                        else needNew = 1;

                        if ( needNew || inserted ) break;
                    }
                    if ( needNew || inserted ) break;
                }

                if (needNew){
                    vector< string > temp;
                    temp.push_back( t1 );
                    temp.push_back( t2 );
                    tracker.push_back(temp);
                    temp.clear();

                }

            }

        }

    }

    int sweeping = 1;

    while( sweeping ){

        int match = 0;

        for (int j = 0; j < tracker.size(); j++){
            for (int k = j + 1; k < tracker.size(); k++){
                for(int l = 0; l < tracker[j].size(); l++){
                    for(int m = 0; m < tracker[k].size(); m++){
                        if ( tracker[j][l] == tracker[k][m] ) match = 1;
                    }
                }
                if (match){
                    for (int i = 0; i < tracker[k].size(); i++){
                        tracker[j].push_back( tracker[k][i] );
                    }
                    tracker.erase( tracker.begin() + k );
                }
            }
        }
        if (!match || tracker.size() == 1) sweeping = 0;
    }

    while ( tracker.size() > 1 ){

        int ref1 = -1;
        int ref2 = -1;

        for (int j = 0; j < tracker[0].size(); j++){
            if( ref1 < get_ref( tracker[0][j] ) )
                ref1 = get_ref( tracker[0][j] );
        }

        for (int j = 0; j < tracker[1].size(); j++){
            if( ref2 < get_ref( tracker[1][j] ) )
                ref2 = get_ref( tracker[1][j] );
        }

        for (int j = 0; j < tracker[1].size(); j++){
            tracker[0].push_back( tracker[1][j] );
        }

        tracker.erase( tracker.begin() + 1 );

        ptBlock.setOperation(ParseTreeBlock::CARTESIAN_PRODUCT);
        ptBlock.setLeftEdge(ref1);

        ptBlock.setRightEdge(ref2);
        pt.addParseTreeBlock(ptBlock);

        flushACTLine();
        flushPTBlock();

    }

}

bool ParseTreeFactory::in_merge( string table, vector< string > mv ){

    for ( int i = 0; i < mv.size(); i++ )
        if ( table == mv[i] ) return true;

    return false;

}


int ParseTreeFactory::get_ref( string table ){

    int ptref = -1;

    vector< string > merged;
    for ( int i = 0; i < tables.size(); i++ ){
        if ( bins[ table ] == bins[ tables[ i ] ] ) merged.push_back( tables[i] );
    }

    for (int i = 0; i < pt.getPTSize(); i++){

        if (pt.getParseTreeBlock(i).getOperation() == ParseTreeBlock::READ && ( in_merge( pt.getParseTreeBlock(i).getTable() , merged ) ) ){
            if ( pt.getPTSize() > i + 1 ){
                if ( pt.getParseTreeBlock(i+1).getOperation() != ParseTreeBlock::RENAME )
                    ptref = i;
            }
            else ptref = i;
        }

        if (pt.getParseTreeBlock(i).getOperation() == ParseTreeBlock::RENAME && ( in_merge( pt.getACTLine( pt.getParseTreeBlock(i).getACTReference() ).getValue() , merged ) ) ){
            ptref = i;
        }

        if (pt.getParseTreeBlock(i).getOperation() == ParseTreeBlock::SELECT || pt.getParseTreeBlock(i).getOperation() == ParseTreeBlock::SIMILARITY_SELECT ){
            if ( in_merge( get_table( pt.getACTLine( pt.getParseTreeBlock(i).getACTReference() ).getAttribute() ) , merged ) ){
                ptref = i;
            }
        }


        if ( pt.getParseTreeBlock(i).getOperation() == ParseTreeBlock::JOIN ||
             pt.getParseTreeBlock(i).getOperation() == ParseTreeBlock::RANGE_JOIN ||
             pt.getParseTreeBlock(i).getOperation() == ParseTreeBlock::CLOSEST_JOIN ||
             pt.getParseTreeBlock(i).getOperation() == ParseTreeBlock::NEAREST_JOIN ){

            if ( get_table( pt.getACTLine( pt.getParseTreeBlock(i).getACTReference() ).getAttribute() ) == table ||
                 get_table( pt.getACTLine( pt.getParseTreeBlock(i).getACTReference() ).getValue() ) == table ){
                ptref = i;
            }

        }


        if ( pt.getParseTreeBlock(i).getACTReference() >= 0 ){
            // Skipping sub-selects if necessary
            if ( pt.getACTLine( pt.getParseTreeBlock(i).getACTReference() ).getValueType() == ACTLine::VT_TREE ){
                std::istringstream is(pt.getACTLine(pt.getParseTreeBlock(i).getACTReference()).getValue());
                int value;
                is >> value;
                i = i + value;
            }
        }
    }


//    //cout << ptref << endl;

    return ptref;

}

void ParseTreeFactory::flushACTLine(){

    actLine.setAttribute("");
    actLine.setAttributeType(-1);
    actLine.setValue("");
    actLine.setValueType(-1);
    actLine.setOperator("");
    actLine.clearParameter();
}

void ParseTreeFactory::flushPTBlock(){

    ptBlock.setOperation(-1);
    ptBlock.setTable("");
    ptBlock.setLeftEdge(-1);
    ptBlock.setRightEdge(-1);
    ptBlock.setCost(-1);
    ptBlock.setACTReference(-1);
}

void ParseTreeFactory::flushParseTree(){

    //pt.clear();
}

void ParseTreeFactory::setLastSQLStatement(string lastSQLStatement){

    this->lastSQLStatement = lastSQLStatement;
    if (p != NULL){
        delete (p);
    }
    p = new Parser(lastSQLStatement);
}

void ParseTreeFactory::setEqualizer(int mergeCount){

    this->mergeCounter = mergeCount;

}

ParseTree* ParseTreeFactory::lastParseTree(){

    return new ParseTree(pt);
}

ParseTree* ParseTreeFactory::canonicalParseTree(string sqlExpression){


    string sql;

    if (sqlExpression.empty()){
        if (getLastSQLStatement().empty()){
            return NULL;
        } else {
            sql = getLastSQLStatement();
        }
    } else {
        sql = sqlExpression;
    }

    // Setting the SQL expression and the parser
    setLastSQLStatement(sql);
    setEqualizer(0);


    //cout << "Bulding Tree ..." << endl;

    // Skip the SELECT to reach the FROM and start building
    while(currentToken().toUpperLexem() != "FROM"){
        getParser()->match(getParser()->getCurrentToken());
    }

    //cout << "FROM CLAUSE" << endl;
    // FROM -> READ the relations
    from_clause();

    // Adds the remaining Cartesian Products, if necessary
    cartesian_product();

    // WHERE -> process the conditions and if necessary generate Cartesian Products / Joins
    if (!endOfStatement()){
        //cout << "WHERE CLAUSE" << endl;
        where_clause();
    }    

    //cout << "GROUP BY CLAUSE" << endl;
    // GROUP BY -> process the aggregation attribute list
    if (!endOfStatement()){
        groupby_clause();
    }

    //cout << "HAVING CLAUSE" << endl;
    // At HAVING -> process the conditions
    if (!endOfStatement()){
        having_clause();
    }

    //cout << "ORDER BY CLAUSE" << endl;
    // At ORDER BY -> process the ordering
    if (!endOfStatement()){
        orderby_clause();
    }

    //cout << "RESET" << endl;
    // Return to SELECT, generate the projection and additional parameters if necessary (DISTINCT, UNION)
    getParser()->resetParser();

    //    //cout << "Current Token:" << getParser()->getCurrentPosition() << " - " << getParser()->getCurrentToken()->getLexem() << endl;
   // //cout << "SELECT SP" << endl;
    select_specification();
    //cout << "terminei" << getParser()->getCurrentPosition() << endl;

    return new ParseTree(pt);
}

string ParseTreeFactory::getLastSQLStatement(){

    return lastSQLStatement;
}

int ParseTreeFactory::getEqualizer(){

    return mergeCounter;

}

void ParseTreeFactory::merge_pt(ParseTree* subPT){

    ParseTreeBlock ptb;
    int shift = pt.getPTSize();    

    for (int i = 0; i < subPT->getPTSize(); i++){

        ptb = subPT->getParseTreeBlock(i);
        int ref = ptb.getACTReference();
        int ledge = ptb.getLeftEdge();
        int redge = ptb.getRightEdge();

        // Update reference position to the merged ACT
        if(ref >= 0)
            ptb.setACTReference(ref + pt.getACTSize());

        if(ledge >= 0)
            ptb.setLeftEdge(ledge + shift);

        if(redge >= 0)
            ptb.setRightEdge(redge + shift);

        //cout << ptb.getOperation() << ptb.getTable() << endl;
        pt.addParseTreeBlock(ptb);
    }

    for (int i = 0; i < subPT->getACTSize(); i++){
        pt.addACTLine(subPT->getACTLine(i));
    }

    subPT->toString();

    setEqualizer( subPT->getPTSize() );
}


// Sets the necessary internal variables to process the tree junctions later on the code
void ParseTreeFactory::set_join(){

    string t1 = get_table( actLine.getAttribute() );
    string t2 = get_table( actLine.getValue() ); 

    if ( t1 == actLine.getAttribute() ){
        t1 = tables[ tables.size() -1 ];
        if ( t1 == get_table( actLine.getValue() ) ) t1 = tables[ tables.size() -2 ];
        t1.append( "." );
        t1.append( get_table( actLine.getAttribute() ) );
        actLine.setAttribute( t1 );
    }
    if ( t2 == actLine.getValue() ){
        t2 = tables[ tables.size() - 2 ];
        if ( t2 == get_table( actLine.getAttribute() ) ) t2 = tables[ tables.size() -1 ];
        t2.append( "." );
        t2.append( get_table( actLine.getValue() ) );
        actLine.setValue( t2 );
    }

    t1 = get_table( t1 );
    t2 = get_table( t2 );

    // Setting the positions
    ptBlock.setLeftEdge( get_ref( t1 ) );
    ptBlock.setRightEdge( get_ref( t2 ) );

    // Marking the tables as joined ...
    joinedTables[ t1 ] = 1;
    joinedTables[ t2 ] = 1;

}

void ParseTreeFactory::build(){

    for ( int i = 0; i < tables.size(); i++ ){
        bins[ tables[i] ] = i;        
    }

    for ( int i = 0; i < joinMap.size(); i++ ){

        // Updating references
        ParseTreeBlock temp = joinMap[i].first;
        string t1 = get_table ( joinMap[i].second.getAttribute() );
        string t2 = get_table ( joinMap[i].second.getValue() );
        int ref1 = get_ref( t1 );
        int ref2 = get_ref( t2 );
        temp.setLeftEdge( ref1 );
        temp.setRightEdge( ref2 );

        pt.addParseTreeBlock( ( temp ), ( joinMap[i].second ) );        

        int aux = bins[ t2 ];
        for ( int j = 0; j < tables.size(); j++ ){
            if( bins[ tables[j] ] == aux ||
                bins[ tables[j] ] == bins [ t1 ] )

                bins[ tables[j] ] = bins[ t1 ];

        }

    }

}

string ParseTreeFactory::to_lower(string str){
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);

    return str;
}
