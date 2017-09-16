#include "ComplexDMLStatement.hpp"

/**
* This method <b>translates</b> a Complex SQL, into a Regular SQL, that
* can be pure executed by DBMS such as Oracle(R), Postgres(S) or
* MySQL(R). </br>
*
* The implicit input of this method in the Parser attribute of this
* class.
* <b>Important</b>: If the complex sql is a non-dml, it will generate
* an error.
*
* @return A vector with regular SQL to be executed;
*/
vector<string> ComplexDMLStatement::translateComplexSQLToRegularSQL(){

    try{
        string sql;
        vector<Token> aux;
        aux = getParser()->getTokenList();

        for (int x = 0; x < aux.size(); x++){
            if (x != 0){
                sql += " ";
            }
            sql += aux[x].getLexem();
        }
        aux.clear();

        int command = chooseDMLStatement();
        switch (command){
        case ComplexDMLStatement::COMPLEX_INSERT_INTO_STATEMENT:
            commandList = complexInsertIntoStatement();
            break;
        case ComplexDMLStatement::COMPLEX_SELECT_STATEMENT:
            commandList = complexSelectStatement();
            break;
        case ComplexDMLStatement::COMPLEX_UPDATE_STATEMENT:
            commandList.push_back(sql);
            return commandList;
            break;
        case ComplexDMLStatement::COMPLEX_DELETE_STATEMENT:
            commandList.push_back(sql);
            return commandList;
            break;
        default:
            getParser()->addError("Non-valid DML statement.");
        }
    } catch (...) {
        getParser()->addError("Syntax error.");
    }

    vector<string> clean = commandList;
    commandList.clear();

    for (int x = 0; x < clean.size(); x++){
        if (clean[x].size() > 7){
            commandList.push_back(clean[x]);
        }
    }
    clean.clear();

    return commandList;
}

/**
* This methods checks if the sentence is a INSERT INTO statement. If yes process according to the rule definition,
* otherwise a Syntax Error will be generated. The implicit input is the Parser attribute of this class. </br></br>
*
* <b>Rule definition:</b><br>
* <insert_statement> ::= INSERT INTO <table_name> ['(' <insert_column_list> ')'] VALUES '(' <insert_value_list> ')'
*
* </br><b>Important</b>: This method also validates regular SQL INSERT INTO command.
*
* @return The standard SQL for INSERT INTO command.
*/
vector<string> ComplexDMLStatement::complexInsertIntoStatement() {

    // Auxiliary list
    vector<string> pAttribList;
    string tableName;

    // clears the auxiliary lists
    imgAttribList.clear();
    audAttribList.clear();
    insertColList.clear();
    insertValueList.clear();
    imgAttribPosList.clear();
    audAttribPosList.clear();
    imgInsertAuxList.clear();
    audInsertAuxList.clear();
    attribPosition = "0";

    commandList.clear();

    try{

        if (getParser()->getCurrentToken()->toUpperLexem() == "INSERT"){
            getParser()->match(getParser()->getCurrentToken());
        } else {
            getParser()->match("INSERT", Token::TK_IDENTIFIER);
        }

        if (getParser()->getCurrentToken()->toUpperLexem() == "INTO"){
            getParser()->match(getParser()->getCurrentToken());
        } else {
            getParser()->match("INTO", Token::TK_IDENTIFIER);
        }

        tableName = getParser()->getCurrentToken()->getLexem();
        getParser()->match(getParser()->getCurrentToken()->getLexem(), Token::TK_IDENTIFIER);

        //getting possible stillimage attributes.
        imgAttribList = getDictionary()->getStillimageAttribute(tableName);
        //getting possible audio attributes.
        audAttribList = getDictionary()->getAudioAttribute(tableName);
        //getting possible particulate attributes.
        pAttribList = getDictionary()->getParticulateAttribute(tableName);

        // optional TOKEN ( <column list> )
        if (getParser()->getCurrentToken()->getTokenType() == Token::TK_OPEN_BRACE) {
            getParser()->match("(", Token::TK_OPEN_BRACE);

            buildColumnList();

            // inserting the particulate attributes
            for ( int i = 0; i < pAttribList.size(); i++) {
                // adding the ','
                Token *tk = new Token();
                tk->setLexem(",");
                tk->setTokenType(Token::TK_COMMA);
                getParser()->insertToken(tk, getParser()->getCurrentPosition());

                // adding the particulate attribute
                tk = new Token();
                tk->setLexem(pAttribList[i]);
                tk->setTokenType(Token::TK_IDENTIFIER);
                getParser()->insertToken(tk, getParser()->getCurrentPosition()+1);
            }
            getParser()->match(")", Token::TK_CLOSE_BRACE);
        } else { //gets all the attributes of table name
            if (pAttribList.size() > 0){
                getParser()->addError("Particulate attributes are system controlled. It is not allowed to assign values to them.");
            } else {
                insertColList = getDictionary()->getFieldsName(tableName);
            }
        }

        if (!hasError()) {

            for (int x = 0; x < insertColList.size(); x++){

                char auxp[8];
                for (int y = 0; y < imgAttribList.size(); y++){
                    if (imgAttribList[y] == insertColList[x]){
                        sprintf(auxp, "%d", x);
                        imgAttribPosList.push_back(auxp);
                    }
                }
                for (int y = 0; y < audAttribList.size(); y++){
                    if (audAttribList[y] == insertColList[x]){
                        sprintf(auxp, "%d", x);
                        audAttribPosList.push_back(auxp);
                    }
                }
                for (int y = 0; y < pAttribList.size(); y++){
                    if (pAttribList[y] == insertColList[x]){
                        getParser()->addError("The particulate attribute '" + insertColList[x] + "' is system controlled.");
                    }
                }
            }
        }

        if (!hasError()) {
            // inserting the particulate attributes
            for ( int i = 0; i < pAttribList.size(); i++)
                insertColList.push_back(pAttribList[i]);

            if (getParser()->getCurrentToken()->toUpperLexem() == "VALUES"){
                getParser()->match(getParser()->getCurrentToken());
            } else {
                getParser()->match("VALUES", Token::TK_IDENTIFIER);
            }

            getParser()->match("(", Token::TK_OPEN_BRACE);

            int atbp = atoi(attribPosition.c_str());
            buildInsertValueList(&atbp);

            char auxp[8];
            sprintf(auxp, "%d", atbp);
            attribPosition = auxp;

            string partid = "-1";
            if (pAttribList.size() > 0){
                sprintf(auxp, "%d", getDictionary()->runSequence("ParticulateIdSequence"));
                partid = auxp;
            }

            for ( int i = 0; i < pAttribList.size(); i++) {
                // adding the ','
                Token *tk = new Token();
                tk->setLexem(",");
                tk->setTokenType(Token::TK_COMMA);
                getParser()->insertToken(tk, getParser()->getCurrentPosition());

                // adding the particulate attribute
                tk = new Token();
                tk->setLexem(partid);
                tk->setTokenType(Token::TK_INTEGER);
                getParser()->insertToken(tk, getParser()->getCurrentPosition()+1);
            }

            getParser()->match(")", Token::TK_CLOSE_BRACE);

            // - verify if all attributes needed to build the particulate object were informed
            string neededAttribute = syntactic->checkInsertParticles(tableName, insertColList);
            if (!neededAttribute.empty()) {
                getParser()->addError("The attribute '" + neededAttribute + "' needs to be present in the insert statement since there is a particulate referencing it.");
            }

            if (!hasError()) {

                if (pAttribList.size() > 0) {
                    // Insert each particulate object in its related metric trees
                    insertParticulate(tableName, partid, insertColList, insertValueList);
                }

                string tab_name = "";
                string attrib_name = "";
                string f_name = "";
                string id = "0";

                // Execute the other insert statements for stillimage
                int i = 0;
                while (i < imgInsertAuxList.size()) {

                    tab_name = imgInsertAuxList[i];
                    i += 1;
                    attrib_name = imgInsertAuxList[i];
                    i += 1;

                    //Eliminating the "'" chars
                    //for (int x = 0; x < )

                    f_name = imgInsertAuxList[i];
                    i += 1;
                    id = imgInsertAuxList[i];

                    ifstream fstream(f_name.c_str());

                    if (fstream){
                        insertImageIPV(tab_name, attrib_name, f_name, id);
                    } else {
                        getParser()->addError("You must specify a valid image file. File not found: " + f_name + ".");
                    }

                    fstream.clear();

                    i += 1;
                }

                // Execute the other insert statements for audio
                i = 0;
                while (i < audInsertAuxList.size()) {

                    tab_name = audInsertAuxList[i];
                    i += 1;
                    attrib_name = audInsertAuxList[i];
                    i += 1;
                    f_name = audInsertAuxList[i];
                    i += 1;
                    id = audInsertAuxList[i];

                    ifstream fstream(f_name.c_str());

                    if (fstream){
                        insertAudioAPV(tab_name, attrib_name, f_name, id);
                    } else {
                        getParser()->addError("You must specify a valid audio file. File not found: " + f_name + ".");
                    }

                    fstream.clear();

                    i += 1;
                }
            }
        }

        //Build the main INSERT INTO STATEMENT
        string sql;
        for(int y = 0; y < getParser()->countTokens(); y++){
            sql += getParser()->getToken(y)->getLexem() + " ";
        }
        commandList.push_back(sql);

    } catch(...){
        getParser()->addError("Syntax Error.");
    }

    return commandList;
}

// SELECT [DISTINCT | ALL] select_list table_expression
vector<Token> ComplexDMLStatement::rebuildComplexSelectToRegularSelectStatement(){

    try{
        if ((getParser()->getCurrentToken()->toUpperLexem() == "SELECT") || (getParser()->getCurrentToken()->toUpperLexem() == "SUBSELECT")){
            getParser()->match(getParser()->getCurrentToken());
            // optional quantifier
            if ((getParser()->getCurrentToken()->toUpperLexem() == "DISTINCT") || (getParser()->getCurrentToken()->toUpperLexem() == "ALL")){
                getParser()->match(getParser()->getCurrentToken()->getLexem(), Token::TK_IDENTIFIER);
            }

            buildSelectList();
            checkIfTableExpressionIsValid();

            if (!hasError()) {

                //attributes new escopes position
                //Unnecessary
                int highScope = 0;
                int highScopePosition = -1;
                for (int y = 0; y < getParser()->countTokens(); y++){
                    if (getParser()->getToken(y)->getScopePosition() > highScope){
                        highScope = getParser()->getToken(y)->getScopePosition();
                        highScopePosition = y;
                    }
                }

                for (int x = highScopePosition; ((highScope > 0) && (x < getParser()->getCurrentPosition())); x++){
                    getParser()->getToken(x)->setScopePosition(highScope);
                }
                //End - This code can be replaced by getParser()->defineRightScopes()

                bool checkComplexAtt = hasComplexAttributeInTheStatement(getParser()->getTokenList(getParser()->getActiveScope()));
                //Replaces the image id column references of the select list for the appropriate image column references
                if ((checkComplexAtt) && (getParser()->getTokenList(getParser()->getActiveScope()).size() == rebuildSelectAsteriskToSelectColumns(getParser()->getTokenList(getParser()->getActiveScope()), getParser()->getActiveScope()).size())){
                    rebuildSelectComplexColumnsToSelectRegularColumns(getParser()->getTokenList(getParser()->getActiveScope()), getParser()->getActiveScope());
                }
            }
        } else {
            getParser()->addError("Select expected. '"+getParser()->getCurrentToken()->getLexem()+"' found.");
        }
    } catch (...) {
    }

    return getParser()->getTokenList(getParser()->getActiveScope());
}

vector<string> ComplexDMLStatement::complexSelectStatement(){

    try{

        //Parse SELECT and generates
        //the joins for complex attributes

        //Return it as
        //SELECT complex JOIN PV$ ... NEAR/FAR ...
        rebuildComplexSelectToRegularSelectStatement();


        int earlier = getParser()->getActiveScope();

        if (earlier-1 >= 0)
            getParser()->setActiveScope(earlier-1);
        else
            getParser()->setActiveScope(0);

        // Treat the NEAR and FAR predicates
        if ((!hasError()) && (getParser()->getCurrentPosition() != -1)) {
            rebuildSelectKnnOrRangeToSelectInList();
        }

        getParser()->setActiveScope(earlier);
    } catch (...) {
       // getParser()->addError("Syntax Error.");
    }


    string aux;
    commandList.clear();
    //Builds the translation
    for (int x = 0; x < getParser()->countTokens(); x++){
        if (x != 0){
            aux +=  ' ';
        }
        aux += getParser()->getToken(x)->getLexem();
    }
    commandList.push_back(aux);

    return commandList;
}

/**
* Checks if the values to be inserted are singular o complex. </br>
* In the complex case it is turned on integer that references another
* tuple that contais the entire data as a blob. </br>
* The implicit input of this method is the list of values to be inserted.
*
* @param attribPosition: The position of the value on the value list.
* @return True if the values are ok in number and types, false otherwise.
*/
bool ComplexDMLStatement::checkIfInsertValueIsValid(int attribPosition){

    long id = 0;
    string file, nops;

    bool imageFound = false;
    for (int x = 0; x < imgAttribPosList.size(); x++){
        if (atoi(imgAttribPosList[x].c_str()) == attribPosition){
            imageFound = true;
        }
    }

    bool audioFound = false;
    for (int x = 0; x < audAttribPosList.size(); x++){
        if (atoi(audAttribPosList[x].c_str()) == attribPosition){
            audioFound = true;
        }
    }

    // verifies if the insert value is an image
    if (imageFound) {
        // replaces the image for an image id and inserts the image in the appropriate IPV$ table

        nops = getParser()->getCurrentToken()->getLexem();

        for (int x = 1; x < nops.size()-1; x++){
            file += nops[x];
        }

        id = getDictionary()->runSequence("ImageIdSequence");
        // parameters stored in the InsertAuxList: table name, stillimage attribute name, image file, image id
        imgInsertAuxList.push_back(getParser()->getToken(2)->getLexem()); // table name
        imgInsertAuxList.push_back(insertColList[attribPosition]); // stillimage attribute name
        imgInsertAuxList.push_back(file); // image file
        char auxp[8];
        sprintf(auxp, "%ld", id);
        imgInsertAuxList.push_back(auxp); // image id
        getParser()->getCurrentToken()->setLexem(auxp);
        getParser()->getCurrentToken()->setTokenType(Token::TK_INTEGER);
        insertValueList.push_back(getParser()->getCurrentToken()->getLexem());
        getParser()->match(getParser()->getCurrentToken());
    } else {
        // verifies if the insert value is an audio
        if (audioFound) {
            // replaces the audio for an audio id and inserts the audio in the appropriate APV$ table
            file = getParser()->getCurrentToken()->getLexem();
            id = getDictionary()->runSequence("AudioIdSequence");
            // parameters stored in the InsertAuxList: table name, audio attribute name, audio file, audio id
            audInsertAuxList.push_back(getParser()->getToken(2)->getLexem()); // table name
            audInsertAuxList.push_back(insertColList[attribPosition]); // audio attribute name
            audInsertAuxList.push_back(file); // audio file
            char auxp[8];
            sprintf(auxp, "%ld", id);
            audInsertAuxList.push_back(auxp); // audio id
            getParser()->getCurrentToken()->setLexem(auxp);
            getParser()->getCurrentToken()->setTokenType(Token::TK_INTEGER);
            insertValueList.push_back(getParser()->getCurrentToken()->getLexem());
            getParser()->match(getParser()->getCurrentToken());
        } else {
            insertValueList.push_back(getParser()->getCurrentToken()->getLexem());
            getParser()->match(getParser()->getCurrentToken());
        }
    }

    return (!hasError());
}

/**
* Build the list value of parameters to be inserted. </br>
*
* @param attribPosition: The current position of the value on the value list.
* @return True if the values of the list are ok in number and types, false otherwise.
*/
bool ComplexDMLStatement::buildInsertValueList(int *attribPosition){

    int auxn = *attribPosition;
    checkIfInsertValueIsValid(auxn);

    // auxiliary variable that stores the position of the current value in the in
    (*attribPosition) = auxn+1;

    if (getParser()->getCurrentToken()->getTokenType() == Token::TK_COMMA) {
        getParser()->match(",", Token::TK_COMMA);
        buildInsertValueList(attribPosition);
    }

    return (!hasError());
}

/**
* Check if the column name is valid. </br>
*
* @return True if the current column name is valid, false otherwise.
*/
bool ComplexDMLStatement::checkIfInsertColumnIsValid(){

    insertColList.push_back(getParser()->getCurrentToken()->getLexem());
    getParser()->match(getParser()->getCurrentToken()->getLexem(), Token::TK_IDENTIFIER);

    return (!hasError());
}

/**
* Build the column name list and validate it. </br>
*
* @return True if the list can be built, false otherwise.
*/
bool ComplexDMLStatement::buildColumnList(){

    checkIfInsertColumnIsValid();

    if (getParser()->getCurrentToken()->getTokenType() == Token::TK_COMMA) {
        getParser()->match(",", Token::TK_COMMA);
        buildColumnList();
    }

    return (!hasError());
}

// far_predicate ::= img_column_reference FAR [similarity_grouping] image_definition
//                      [BY metric_name]
//                      [RANGE val_range]
//                      [STOP AFTER k_nearest [WITH tie_list]]
bool ComplexDMLStatement::checkIfFarPredicateIsValid(){

    if (getParser()->getCurrentToken()->toUpperLexem() == "FAR") {
        getParser()->match(getParser()->getCurrentToken());
    } else {
        getParser()->match("FAR", Token::TK_IDENTIFIER);
    }

    // optional similarity grouping
    if (getParser()->getCurrentToken()->getTokenType() == Token::TK_IDENTIFIER) {
        if (getParser()->getCurrentToken()->toUpperLexem() == "SUM") {
            getParser()->getCurrentToken()->setLexemType(Lexical::LK_SUM_SG);
            getParser()->match(getParser()->getCurrentToken());
        } else {
            if (getParser()->getCurrentToken()->toUpperLexem() == "AVG") {
                getParser()->getCurrentToken()->setLexemType(Lexical::LK_AVG_SG);
                getParser()->match(getParser()->getCurrentToken());
            } else {
                if (getParser()->getCurrentToken()->toUpperLexem() == "ALL") {
                    getParser()->getCurrentToken()->setLexemType(Lexical::LK_ALL_SG);
                    getParser()->match(getParser()->getCurrentToken());
                } else {
                    if (getParser()->getCurrentToken()->toUpperLexem() == "EVERY") {
                        getParser()->getCurrentToken()->setLexemType(Lexical::LK_EVERY_SG);
                        getParser()->match(getParser()->getCurrentToken());
                    } else {
                        if (getParser()->getCurrentToken()->toUpperLexem() == "ANY") {
                            getParser()->getCurrentToken()->setLexemType(Lexical::LK_ANY_SG);
                            getParser()->match(getParser()->getCurrentToken());
                        } else {
                            if (getParser()->getCurrentToken()->toUpperLexem() == "MAX") {
                                getParser()->getCurrentToken()->setLexemType(Lexical::LK_MAX_SG);
                                getParser()->match(getParser()->getCurrentToken());
                            } else {
                                getParser()->addError("Similarity grouping clause expected: "+ getParser()->getCurrentToken()->getLexem() + " found instead.");
                            }
                        }
                    }
                }
            }
        }
    }

    checkIfQueryObjectDefinitionIsValid();

    // optional BY clause
    if ((getParser()->getCurrentPosition() != -1) && (getParser()->getCurrentToken()->toUpperLexem() == "BY")) {
        getParser()->match(getParser()->getCurrentToken());
        checkIfMetricNameIsValid();
    }
    // optional RANGE clause
    if ((getParser()->getCurrentPosition() != -1) && (getParser()->getCurrentToken()->toUpperLexem() == "RANGE")) {
        getParser()->match(getParser()->getCurrentToken());
        checkIfRangeValueIsValid();
    }
    // optional STOP AFTER clause
    if ((getParser()->getCurrentPosition() != -1) && (getParser()->getCurrentToken()->toUpperLexem() == "STOP")) {
        getParser()->match(getParser()->getCurrentToken());
        if (getParser()->getCurrentToken()->toUpperLexem() == "AFTER") {
            getParser()->match(getParser()->getCurrentToken());
            checkIfKNearestValueIsValid();
            // optional WITH clause
            if (getParser()->getCurrentToken()->toUpperLexem() == "WITH") {
                getParser()->match(getParser()->getCurrentToken());
                checkIfTieListIsValid();
            }
        } else {
            getParser()->match("AFTER", Token::TK_IDENTIFIER);
        }
    }

    return (!hasError());
}

// tie_list ::= TIE LIST
bool ComplexDMLStatement::checkIfTieListIsValid(){

    if (getParser()->getCurrentToken()->toUpperLexem() == "TIE") {
        getParser()->match(getParser()->getCurrentToken());
        if (getParser()->getCurrentToken()->toUpperLexem() == "LIST") {
            getParser()->match(getParser()->getCurrentToken());
        } else {
            getParser()->match("LIST", Token::TK_IDENTIFIER);
        }
    } else {
        getParser()->match("TIE", Token::TK_IDENTIFIER);
    }

    return (!hasError());
}

// k_nearest::= TK_INTEGER
bool ComplexDMLStatement::checkIfKNearestValueIsValid(){

    if (getParser()->getCurrentToken()->getTokenType() == Token::TK_INTEGER) {
        getParser()->match(getParser()->getCurrentToken());
    } else {
        getParser()->addError("Integer expected (k-nn value) : Found " + getParser()->getCurrentToken()->getLexem() + " instead.");
    }

    return (!hasError());
}

// val_range ::= TK_FLOATING_POINT | TK_INTEGER
bool ComplexDMLStatement::checkIfRangeValueIsValid() {

    //Must have a atof - like function that guarantees that is a number
    if ((getParser()->getCurrentToken()->getTokenType() == Token::TK_INTEGER) || (getParser()->getCurrentToken()->getTokenType() == Token::TK_FLOATING_POINT)) {
        getParser()->match(getParser()->getCurrentToken());
    } else {
        getParser()->addError("Value expected for range: Found " + getParser()->getCurrentToken()->getLexem() + " instead.");
    }

    return (!hasError());
}

bool ComplexDMLStatement::checkIfMetricNameIsValid(){

    bool answer = true;
    if (getParser()->getCurrentToken()->getTokenType() == Token::TK_IDENTIFIER){
        answer = answer && getParser()->match(getParser()->getCurrentToken());
    }else{
        getParser()->addError("Metric name expected. " + getParser()->getCurrentToken()->getLexem() + " found.");
        answer = false;
    }

    return answer;
}

// parameter_val_assoc: particle_column_val_ref as parameter_name
bool ComplexDMLStatement::checkIfAssociedParameterValueListIsValid(){

    // particle_colum_val_ref (Only integer and floating point values are allowed)
    if (getParser()->getCurrentToken()->getTokenType() == Token::TK_INTEGER){
        getParser()->match(getParser()->getCurrentToken()->getLexem(), Token::TK_INTEGER);
    } else {
        if (getParser()->getCurrentToken()->getTokenType() == Token::TK_FLOATING_POINT){
            getParser()->match(getParser()->getCurrentToken()->getLexem(), Token::TK_FLOATING_POINT);
        } else {
            getParser()->addError("Invalid parameter type found to: " + getParser()->getCurrentToken()->getLexem());
        }
    }

    // token AS
    if (getParser()->getCurrentToken()->toUpperLexem() == "AS") {
        getParser()->match(getParser()->getCurrentToken());
    }

    // parameter_name
    getParser()->match(getParser()->getCurrentToken()->getLexem(), Token::TK_IDENTIFIER);


    return (!hasError());
}

// parameter_val_assoc_list: parameter_val_assoc | parameter_val_assoc ',' parameter_val_assoc_list
bool ComplexDMLStatement::buildAssociedParameterValueList() {

    checkIfAssociedParameterValueListIsValid();
    if (getParser()->getCurrentToken()->getTokenType() == Token::TK_COMMA) {
        getParser()->match(",", Token::TK_COMMA);
        buildAssociedParameterValueList();
    }

    return (!hasError());
}

// query_obj_definition ::= complex_attribute_reference | subquery | particulate_reference | image_location | audio_location
bool ComplexDMLStatement::checkIfQueryObjectDefinitionIsValid() {

    string tableName;

    // complex_attribute_reference
    if (getParser()->getCurrentToken()->getTokenType() == Token::TK_IDENTIFIER) {

        checkIfColumnRefenceIsValid();

        bool check_attr_ref = getDictionary()->isComplexAttributeReference(getParser()->getCurrentToken()->getLexem());
        bool tname_check = false;

        if (check_attr_ref) {
            vector<string> tbNameList = syntactic->getTableNameFromTokenList(getParser()->getTokenList(), getParser()->getCurrentToken()->getLexem());
            if (tbNameList.size() == 0) {
                getParser()->addError("Invalid complex attribute: " + getParser()->getCurrentToken()->getLexem() + ".");
            } else {
                for (int i = 0; i < tbNameList.size(); i++) {
                    tableName = tbNameList[i];
                    if (getParser()->getCurrentToken()->getTokenType() == Token::TK_PERIOD) {
                        if (getParser()->getToken(getParser()->getCurrentPosition()-2)->getLexem() == tableName)
                            tname_check = true;
                    } else {
                        tname_check = true;
                    }
                } if (!tname_check) {
                    getParser()->addError("Invalid complex attribute: " + tableName + "." + getParser()->getCurrentToken()->getLexem() + ".");
                }
            }
        }
    } else { // subquery ::= '(' query_specification ')' or particulate_reference ::=  '(' parameter_val_assoc_list ')'
        if (getParser()->getCurrentToken()->getTokenType() == Token::TK_OPEN_BRACE) {
            getParser()->match("(", Token::TK_OPEN_BRACE);

            if (getParser()->getCurrentToken()->toUpperLexem() == "SELECT") {
                //Here is the BIG trick
                //Turns the internal select into subselect
                //NOTE: Only performKnnOrRangeQuery method
                //can turn SUBSELECT back into SELECT
                getParser()->getCurrentToken()->setLexem("SUBSELECT");
                //Sets a new level of scope --
                //NOTE: getParser()->defineRightScopes() will have the same effect
                //because of '(' and ')'
                getParser()->getCurrentToken()->setScopePosition(getParser()->getNumberOfScopes()+1);
                getParser()->setActiveScope(getParser()->getNumberOfScopes());

                //Recursive call
                //complexSelectStatement();
                rebuildComplexSelectToRegularSelectStatement();

                //Return to back level
                getParser()->setActiveScope(getParser()->getActiveScope()-1);

            } else { // particulate_reference ::=  '(' parameter_val_assoc_list ')'
                buildAssociedParameterValueList();
            }

            if (getParser()->getCurrentPosition() == -1)
                getParser()->match(getParser()->getCurrentToken());//Token::TK_CLOSE_BRACE
        } else { // image_location ::= 'TK_STRING' or audio_location ::= 'TK_STRING'
            if (getParser()->getCurrentToken()->getTokenType() == Token::TK_STRING) {
                string fileName = getParser()->getCurrentToken()->getLexem();
                ifstream file(fileName.c_str());

                if (file.good()) {
                    getParser()->match(getParser()->getCurrentToken()->getLexem(), Token::TK_STRING);
                } else {
                    getParser()->match(getParser()->getCurrentToken()->getLexem(), Token::TK_STRING);
                    getParser()->addError("File " + fileName + " not found.");
                }
            } else {
                getParser()->addError("Invalid complex object (query center) definition.");
            }
        }
    }

    return (!hasError());
}

//near_predicate ::= img_column_reference NEAR [similarity_grouping] image_definition
//                      [BY metric_name]
//                      [RANGE val_range]
//                      [STOP AFTER k_nearest [WITH TIE LIST]]
bool ComplexDMLStatement::checkIfNearPredicateIsValid() {

    if (getParser()->getCurrentToken()->toUpperLexem() == "NEAR") {
        getParser()->match(getParser()->getCurrentToken());
    } else {
        getParser()->match("NEAR", Token::TK_IDENTIFIER);
    }

    // optional similarity grouping
    if (getParser()->getCurrentToken()->getTokenType() == Token::TK_IDENTIFIER) {
        if (getParser()->getCurrentToken()->toUpperLexem() == "SUM") {
            getParser()->getCurrentToken()->setLexemType(Lexical::LK_SUM_SG);
            getParser()->match(getParser()->getCurrentToken());
        } else {
            if (getParser()->getCurrentToken()->toUpperLexem() == "AVG") {
                getParser()->getCurrentToken()->setLexemType(Lexical::LK_AVG_SG);
                getParser()->match(getParser()->getCurrentToken());
            } else {
                if (getParser()->getCurrentToken()->toUpperLexem() == "ALL") {
                    getParser()->getCurrentToken()->setLexemType(Lexical::LK_ALL_SG);
                    getParser()->match(getParser()->getCurrentToken());
                } else {
                    if (getParser()->getCurrentToken()->toUpperLexem() == "EVERY") {
                        getParser()->getCurrentToken()->setLexemType(Lexical::LK_EVERY_SG);
                        getParser()->match(getParser()->getCurrentToken());
                    } else {
                        if (getParser()->getCurrentToken()->toUpperLexem() == "ANY") {
                            getParser()->getCurrentToken()->setLexemType(Lexical::LK_ANY_SG);
                            getParser()->match(getParser()->getCurrentToken());
                        } else {
                            if (getParser()->getCurrentToken()->toUpperLexem() == "MAX") {
                                getParser()->getCurrentToken()->setLexemType(Lexical::LK_MAX_SG);
                                getParser()->match(getParser()->getCurrentToken());
                            } else {
                                getParser()->addError("Similarity grouping clause expected: "+ getParser()->getCurrentToken()->getLexem() + " found instead.");
                            }
                        }
                    }
                }
            }
        }
    }

    checkIfQueryObjectDefinitionIsValid();

    // optional BY clause
    if ((getParser()->getCurrentPosition() != -1) && (getParser()->getCurrentToken()->toUpperLexem() == "BY")) {
        getParser()->match(getParser()->getCurrentToken());
        checkIfMetricNameIsValid();
    }
    // optional RANGE clause
    if ((getParser()->getCurrentPosition() != -1) && (getParser()->getCurrentToken()->toUpperLexem() == "RANGE")) {
        getParser()->match(getParser()->getCurrentToken());
        checkIfRangeValueIsValid();
    }
    // optional STOP AFTER clause
    if ((getParser()->getCurrentPosition() != -1) && (getParser()->getCurrentToken()->toUpperLexem() == "STOP")) {
        getParser()->match(getParser()->getCurrentToken());
        if (getParser()->getCurrentToken()->toUpperLexem() == "AFTER") {
            getParser()->match(getParser()->getCurrentToken());
            checkIfKNearestValueIsValid();
            // optional WITH clause
            if (getParser()->getCurrentToken()->toUpperLexem() == "WITH") {
                getParser()->match(getParser()->getCurrentToken());
                checkIfTieListIsValid();
            }
        } else {
            getParser()->match("AFTER", Token::TK_IDENTIFIER);
        }
    }

    return (!hasError());
}

bool ComplexDMLStatement::checkIfValueExpressionIsValid() {

    // a valid value_expression can be: a string, an integer or a floating point
    if (getParser()->getCurrentToken()->getTokenType() == Token::TK_STRING){
        getParser()->match(getParser()->getCurrentToken()->getLexem(), Token::TK_STRING);
    } else {
        if (getParser()->getCurrentToken()->getTokenType() == Token::TK_INTEGER){
            getParser()->match(getParser()->getCurrentToken()->getLexem(), Token::TK_INTEGER);
        } else {
            if (getParser()->getCurrentToken()->getTokenType() == Token::TK_FLOATING_POINT){
                getParser()->match(getParser()->getCurrentToken()->getLexem(), Token::TK_FLOATING_POINT);
            } else {
                getParser()->addError("Invalid value expression. Expected integer, float or string type for: " + getParser()->getCurrentToken()->getLexem());
            }
        }
    }

    return (!hasError());
}

// predicate ::= comparison_predicate | between_predicate | like_predicate
//                  | test_for_null | in_predicate | all_or_any_predicate
//                  | existence_test | near_predicate | far_predicate
// notes: 1) the operators comparison_predicate, between_predicate, like_predicate,
//       test_for_null, in_predicate, all_or_any_predicate and existence_test
//       only can be used with numeric and string attributes
//       2) the operators near_predicate and far_predicate only can be used with
//       complex attributes (stillimage or particulate)

bool ComplexDMLStatement::checkIfPredicateIsValid() {

    string tableName;
    string ca;
    checkIfColumnRefenceIsValid();

    if (!hasError()) {
        ca = getParser()->getToken(getParser()->getCurrentPosition()-1)->getLexem();
        bool check_attr_ref = getDictionary()->isComplexAttributeReference(ca);
        bool tname_check = false;

        if (check_attr_ref) {
            vector<string> tbNameList;
            tbNameList = syntactic->getTableNameFromTokenList(getParser()->getTokenList(/*getParser()->getActiveScope()*/), ca);
            if (tbNameList.empty()){
                getParser()->addError("Invalid complex attribute: " + getParser()->getCurrentToken()->getLexem() + ".");
            } else {
                for (int i = 0; i < tbNameList.size(); i++) {
                    tableName = tbNameList[i];
                    if (getParser()->getToken(getParser()->getCurrentPosition()-2)->getTokenType() == Token::TK_PERIOD) {
                        if (getParser()->getToken(getParser()->getCurrentPosition()-3)->getLexem() == tableName)
                            tname_check = true;
                    } else {
                        tname_check = true;
                    }
                }
                if (!tname_check) {
                    getParser()->addError("Invalid complex attribute: " + tableName + "." + getParser()->getCurrentToken()->getLexem() + ".");
                }
            }
        }

        //.............................
        // comparison_predicate ::= column_reference comp_op column_reference
        //                          | column_reference comp_op value_expression
        //.............................

        // equals operator
        if (getParser()->getCurrentToken()->getLexem() == "=") {
            if (!check_attr_ref) {
                getParser()->match("=", Token::TK_EQUAL);
                if (getParser()->getCurrentToken()->getTokenType() == Token::TK_IDENTIFIER) {
                    checkIfColumnRefenceIsValid();
                } else {
                    checkIfValueExpressionIsValid();
                }
            } else {
                getParser()->addError("The operator \"" + getParser()->getCurrentToken()->getLexem() + "\" cannot be used with a complex attribute.");
            }
        } else {// not equals operator
            if (getParser()->getCurrentToken()->getLexem() == "!=") {
                if (!check_attr_ref) {
                    getParser()->match("!=", Token::TK_NE);
                    if (getParser()->getCurrentToken()->getTokenType() == Token::TK_IDENTIFIER){
                        checkIfColumnRefenceIsValid();
                    } else {
                        checkIfValueExpressionIsValid();
                    }
                } else {
                    getParser()->addError("The operator \"" + getParser()->getCurrentToken()->getLexem() + "\" cannot be used with a complex attribute.");
                }
            } else {// like operator
                if (getParser()->getCurrentToken()->toUpperLexem() == "LIKE") {
                    if (!check_attr_ref) {
                        getParser()->match(getParser()->getCurrentToken());
                        checkIfValueExpressionIsValid();
                    } else {
                        getParser()->addError("The operator \"" + getParser()->getCurrentToken()->getLexem() + "\" cannot be used with a complex attribute.");
                    }
                } else {// not equals operator
                    if (getParser()->getCurrentToken()->getLexem() == "<>") {
                        if (!check_attr_ref) {
                            getParser()->match("<>", Token::TK_N2);
                            if (getParser()->getCurrentToken()->getTokenType() == Token::TK_IDENTIFIER){
                                checkIfColumnRefenceIsValid();
                            } else {
                                checkIfValueExpressionIsValid();
                            }
                        } else {
                            getParser()->addError("The operator \"" + getParser()->getCurrentToken()->getLexem() + "\" cannot be used with a complex attribute.");
                        }
                    } else {// less than operator
                        if (getParser()->getCurrentToken()->getLexem() == "<") {
                            if (!check_attr_ref) {
                                getParser()->match("<", Token::TK_LQ);
                                if (getParser()->getCurrentToken()->getTokenType() == Token::TK_IDENTIFIER){
                                    checkIfColumnRefenceIsValid();
                                } else {
                                    checkIfValueExpressionIsValid();
                                }
                            } else {
                                getParser()->addError("The operator \"" + getParser()->getCurrentToken()->getLexem() + "\" cannot be used with a complex attribute.");
                            }
                        } else {// less than or equals operator
                            if (getParser()->getCurrentToken()->getLexem() == "<=") {
                                if (!check_attr_ref) {
                                    getParser()->match("<=", Token::TK_LE);
                                    if (getParser()->getCurrentToken()->getTokenType() == Token::TK_IDENTIFIER){
                                        checkIfColumnRefenceIsValid();
                                    } else {
                                        checkIfValueExpressionIsValid();
                                    }
                                } else {
                                    getParser()->addError("The operator \"" + getParser()->getCurrentToken()->getLexem() + "\" cannot be used with a complex attribute.");
                                }
                            } else {// greater than operator
                                if (getParser()->getCurrentToken()->getLexem() == ">") {
                                    if (!check_attr_ref) {
                                        getParser()->match(">", Token::TK_GQ);
                                        if (getParser()->getCurrentToken()->getTokenType() == Token::TK_IDENTIFIER) {
                                            checkIfColumnRefenceIsValid();
                                        } else {
                                            checkIfValueExpressionIsValid();
                                        }
                                    } else {
                                        getParser()->addError("The operator \"" + getParser()->getCurrentToken()->getLexem() + "\" cannot be used with a complex attribute.");
                                    }
                                } else {// greater than or equals operator
                                    if (getParser()->getCurrentToken()->getLexem() == ">=") {
                                        if (!check_attr_ref) {
                                            getParser()->match(">=", Token::TK_GE);
                                            if (getParser()->getCurrentToken()->getTokenType() == Token::TK_IDENTIFIER){
                                                checkIfColumnRefenceIsValid();
                                            } else {
                                                checkIfValueExpressionIsValid();
                                            }
                                        }
                                    } else {
                                        //.............................
                                        // near_predicate
                                        //.............................
                                        if (getParser()->getCurrentToken()->toUpperLexem() == "NEAR"){
                                            if (check_attr_ref){
                                                checkIfNearPredicateIsValid();
                                            } else {
                                                getParser()->addError("The operator \"" + getParser()->getCurrentToken()->getLexem() + "\" can only be used with a complex attribute.");
                                            }
                                        } else {
                                            //.............................
                                            // far_predicate
                                            //.............................
                                            if (getParser()->getCurrentToken()->toUpperLexem() == "FAR"){
                                                if (check_attr_ref){
                                                    checkIfFarPredicateIsValid();
                                                } else {
                                                    getParser()->addError("The operator \"" + getParser()->getCurrentToken()->getLexem() + "\" only can be used with a complex attribute.");
                                                }
                                            } else {
                                                //.............................
                                                // predicate not implemented!
                                                //.............................
                                                if (getParser()->getCurrentToken()->getLexem() == ""){
                                                    getParser()->addError("Operator expected.");
                                                } else {
                                                    getParser()->addError("Invalid operator \"" + getParser()->getCurrentToken()->getLexem() + "\".");
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

    return (!hasError());
}

// search_condition ::= NOT search_condition | predicate | search_condition AND search_condition
//                      | search_condition OR search_condition
bool ComplexDMLStatement::checkIfSearchConditionIsValid() {

    if (getParser()->getCurrentToken()->toUpperLexem() == "NOT") {
        getParser()->match(getParser()->getCurrentToken());
        if (getParser()->getCurrentToken()->getTokenType() == Token::TK_IDENTIFIER) {
            checkIfSearchConditionIsValid();
        } else {
            getParser()->addError("Identifier expected.");
        }
    }

    checkIfPredicateIsValid();

    if ((getParser()->getCurrentPosition() != -1) && ((getParser()->getCurrentToken()->toUpperLexem() == "AND") || (getParser()->getCurrentToken()->toUpperLexem() == "OR"))){
        getParser()->match(getParser()->getCurrentToken());
        if (getParser()->getCurrentToken()->getTokenType() == Token::TK_IDENTIFIER) {
            checkIfSearchConditionIsValid();
        } else {
            getParser()->addError("Predicate expected.");
        }
    }

    return (!hasError());
}

// having_clause ::= HAVING search_condition
bool ComplexDMLStatement::checkIfHavingClauseIsValid() {

    if (getParser()->getCurrentToken()->toUpperLexem() == "HAVING"){
        getParser()->match(getParser()->getCurrentToken());
    }
    if (getParser()->getCurrentToken()->getTokenType() == Token::TK_IDENTIFIER) {
        checkIfSearchConditionIsValid();
    } else {
        getParser()->addError("Identifier expected.");
    }

    return (!hasError());
}

// groupby_clause ::= GROUP BY column_reference_list()
bool ComplexDMLStatement::checkIfGroupByClauseIsValid() {

    if (getParser()->getCurrentToken()->toUpperLexem() == "GROUP"){
        getParser()->match(getParser()->getCurrentToken());
        if (getParser()->getCurrentToken()->toUpperLexem() == "BY"){
            getParser()->match(getParser()->getCurrentToken());
        } else {
            getParser()->match("BY", Token::TK_IDENTIFIER);
        }
    }

    if (getParser()->getCurrentToken()->getTokenType() == Token::TK_IDENTIFIER){
        buildColumnReferenceList();
    } else {
        getParser()->addError("Identifier expected.");
    }

    return (!hasError());
}

// where_clause ::= WHERE search_condition
bool ComplexDMLStatement::checkIfWhereClauseIsValid() {


    if (getParser()->getCurrentToken()->toUpperLexem() == "WHERE"){
        getParser()->match(getParser()->getCurrentToken());
    }
    if (getParser()->getCurrentToken()->getTokenType() == Token::TK_IDENTIFIER){
        checkIfSearchConditionIsValid();
    } else {
        getParser()->addError("Identifier expected.");
    }

    return (!hasError());
}

// table_reference_list ::= table_name | table_name ',' table_name_list
bool ComplexDMLStatement::buildTableReferenceList() {

    getParser()->getCurrentToken()->setLexemType(Lexical::LK_TABLE_NAME);
    getParser()->match(getParser()->getCurrentToken()->getLexem(), Token::TK_IDENTIFIER);

    if (getParser()->getCurrentToken()->getTokenType() == Token::TK_COMMA){
        getParser()->match(",", Token::TK_COMMA);

        if (getParser()->getCurrentToken()->getTokenType() == Token::TK_IDENTIFIER){
            buildTableReferenceList();
        } else {
            getParser()->addError("Identifier expected.");
        }
    }

    return (!hasError());
}

//from_clause ::= FROM table_reference_list
bool ComplexDMLStatement::checkIfFromClauseIsValid(){

    if (getParser()->getCurrentToken()->toUpperLexem() == "FROM"){
        getParser()->match(getParser()->getCurrentToken());
    }
    if (getParser()->getCurrentToken()->getTokenType() == Token::TK_IDENTIFIER){
        buildTableReferenceList();
    } else {
        getParser()->addError("Identifier expected.");
    }

    return (!hasError());
}
// table_expression ::= from_clause | where_clause | group_by_clause | having_clause
bool ComplexDMLStatement::checkIfTableExpressionIsValid(){

    if (getParser()->getCurrentToken()->toUpperLexem() == "FROM") {
        checkIfFromClauseIsValid();

        // optional WHERE clause
        if ((getParser()->getCurrentToken()->getTokenType() == Token::TK_IDENTIFIER) && (getParser()->getCurrentToken()->toUpperLexem() == "WHERE")){
            checkIfWhereClauseIsValid();
        }

        // optional GROUP BY clause
        if ((getParser()->getCurrentPosition() != -1) && ((getParser()->getCurrentToken()->getTokenType() == Token::TK_IDENTIFIER) && (getParser()->getCurrentToken()->toUpperLexem() == "GROUP"))){
            checkIfGroupByClauseIsValid();
        }

        // optional HAVING clause
        if ((getParser()->getCurrentPosition() != -1) && (getParser()->getCurrentToken()->getTokenType() == Token::TK_IDENTIFIER) && (getParser()->getCurrentToken()->toUpperLexem() == "HAVING")){
            checkIfHavingClauseIsValid();
        }

        // if its not any of above, then its an error
        if ((getParser()->getCurrentPosition() != -1) && ((getParser()->getCurrentToken()->getTokenType() == Token::TK_IDENTIFIER) && (getParser()->getCurrentToken()->toUpperLexem() != "WHERE") && (getParser()->getCurrentToken()->toUpperLexem() != "GROUP") && (getParser()->getCurrentToken()->toUpperLexem() != "HAVING"))){
            getParser()->addError("Invalid clause: \"" + getParser()->getCurrentToken()->getLexem() + "\".");
        }
    } else {
        if (getParser()->getCurrentPosition() != -1)
            getParser()->addError("Expected FROM clause.");
    }

    return (!hasError());
}

// as_clause ::= AS identifier
bool ComplexDMLStatement::checkIfAsClauseIsValid(){

    if (getParser()->getCurrentToken()->toUpperLexem() == "AS"){
        getParser()->match(getParser()->getCurrentToken());
    }

    if (getParser()->getCurrentToken()->getTokenType() == Token::TK_IDENTIFIER){
        getParser()->match(getParser()->getCurrentToken()->getLexem(), Token::TK_IDENTIFIER);
    } else {
        getParser()->addError("Identifier expected.");
    }

    return (!hasError());
}

bool ComplexDMLStatement::checkIfColumnRefenceIsValid(){

    // supporting grouping functions - just for single data?
    if ((getParser()->getCurrentToken()->toUpperLexem() == "COUNT") || (getParser()->getCurrentToken()->toUpperLexem() == "MIN") || (getParser()->getCurrentToken()->toUpperLexem() == "MAX") || (getParser()->getCurrentToken()->toUpperLexem() == "SUM") || (getParser()->getCurrentToken()->toUpperLexem() == "AVG")) {
        getParser()->match(getParser()->getCurrentToken()->getLexem(), Token::TK_IDENTIFIER);
        getParser()->match("(", Token::TK_OPEN_BRACE);
        getParser()->match(getParser()->getCurrentToken());
        getParser()->match(")", Token::TK_CLOSE_BRACE);
    } else {
        // column_reference ::= column_name['.'extractor_name'.'extractor_parameter]
        //                            | table_name'.'column_name['.'extractor_name'.'extractor_parameter]
        getParser()->match(getParser()->getCurrentToken()->getLexem(), Token::TK_IDENTIFIER);

        // if the token is a TK_PERIOD then the next token must be a identifier
        if ((getParser()->getCurrentPosition() != -1) && (getParser()->getCurrentToken()->getTokenType() == Token::TK_PERIOD)) {
            getParser()->match(".", Token::TK_PERIOD);
            if (getParser()->getCurrentToken()->getTokenType() == Token::TK_IDENTIFIER){
                checkIfColumnRefenceIsValid();
            } else {
                getParser()->addError("Identifier expected.");
            }
        }
    }

    return (!hasError());
}

// column_reference_list ::= column_reference [ as_clause ] | column_reference [ as_clause ] ',' column_reference_list
bool ComplexDMLStatement::buildColumnReferenceList(){

    checkIfColumnRefenceIsValid();

    // optional AS clause
    if (getParser()->getCurrentToken()->toUpperLexem() == "AS"){
        checkIfAsClauseIsValid();
    }

    if (getParser()->getCurrentToken()->getTokenType() == Token::TK_COMMA){
        getParser()->match(",", Token::TK_COMMA);
        if (getParser()->getCurrentToken()->getTokenType() == Token::TK_IDENTIFIER){
            buildColumnReferenceList();
        } else {
            getParser()->addError("Identifier expected.");
        }
    }

    return (!hasError());
}

// select_list ::= '*' | column_reference_list
bool ComplexDMLStatement::buildSelectList() {

    if (getParser()->getCurrentToken()->getLexem() == "*"){
        getParser()->match("*", Token::TK_MUL);
    } else {
        if (getParser()->getCurrentToken()->getTokenType() == Token::TK_IDENTIFIER){
            buildColumnReferenceList();
        } else {
            getParser()->addError("Invalid column reference.");
        }
    }
    return (!hasError());
}

bool ComplexDMLStatement::hasComplexAttributeInTheStatement(vector<Token> tokenList){

    string tableName;
    int tk = 0;
    int sum = 0;

    while (tk < tokenList.size()){
        if (tokenList[tk].getLexemType() == Lexical::LK_TABLE_NAME){
            // verifies if the table has image attributes
            tableName = tokenList[tk].getLexem();
            sum += getDictionary()->getStillimageAttribute(tableName).size();
            sum += getDictionary()->getAudioAttribute(tableName).size();
        }
        tk++;
    }

    return (sum > 0);
}


bool ComplexDMLStatement::insertParticulate(string tableName, string particulateId, vector<string> inserColList, vector<string> insertValueList) {

    // for each particulate attribute in the table tablename
    string sql;
    vector<string> complexAttribNames;
    vector< vector<string> > answer;

    sql =  "SELECT DISTINCTt im.ComplexAttribName ";
    sql += "FROM CDD$ComplexAttribMetric im, CDD$MetricStruct ms ";
    sql += "WHERE im.MetricCode = ms.MetricCode ";
    sql += "AND im.TableName = '" + tableName + "' ";

    answer = getDictionary()->runSubQuery(sql);

    for (int x = 0; x < answer.size(); x++){
        complexAttribNames.push_back(answer[x][0]);
    }

    for (int i = 0; i < complexAttribNames.size(); i++){
        //IndexParticulateAttribute(tablename, partid, ComplexAttribNames->Strings[i]//particulatename, InsertColList, InsertValueList, commandlist);

        sql =  "SELECT DISTINCT ms.MetricName, im.ComplexAttribName, im.IndexFile ";
        sql += "FROM CDD$ComplexAttribMetric im, CDD$MetricStruct ms ";
        sql += "WHERE im.MetricCode = ms.MetricCode ";
        sql += "AND im.TableName = '" + tableName + "' ";
        sql += "AND im.ComplexAttribName = '" + complexAttribNames[i] + "' ";

        vector<string> metricName;
        vector<string> indexFile;
        vector<string> metricParamNames;
        vector<string> metricParamTypes;
        vector<string> userParamList;

        answer = getDictionary()->runSubQuery(sql);

        for (int x = 0; x < answer.size(); x++){
            metricName.push_back(answer[x][0]);
            indexFile.push_back(answer[x][2]);
        }

        // for each metric associated with this table attribute
        for (int x = 0; x < metricName.size(); x++) {

            // getting the metric params names, instances and types
            metricParamNames = getDictionary()->getParticulateMetricParamNames(tableName, metricName[x], complexAttribNames[i]);
            metricParamTypes = getDictionary()->getParticulateMetricParamTypes(tableName, metricName[x], complexAttribNames[i]);
            userParamList = syntactic->convertAttributesToMetricParams(tableName, metricName[x], complexAttribNames[i], inserColList);

            // getting the metric
            string distanceCode = getDictionary()->getDistanceFunction(tableName, complexAttribNames[i], metricName[x]);
            // opening index
            IndexManager *idx = new IndexManager();
            idx->openSlimTree(indexFile[x], atoi(distanceCode.c_str()));
            idx->openDummyTree(indexFile[x], atoi(distanceCode.c_str()));
            //DynamicSlimTree *slimtree = treemanager->OpenSlimTree(IndexFile->Strings[i], LpP);
            //DynamicDummyTree *dummytree = treemanager->OpenDummyTree(IndexFile->Strings[i], LpP);

            // setting the weights
            //SetWeights(slimtree->GetMetricEvaluator(), tablename, particulatename, MetricName->Strings[i]);
            //SetWeights(dummytree->GetMetricEvaluator(), tablename, particulatename, MetricName->Strings[i]);

            // inserting in the metric tree
            BasicArrayObject<double> obj = syntactic->buildParticulateObject(atoi(particulateId.c_str()), userParamList, insertValueList, metricParamNames, metricParamTypes);
            idx->getSlimTree()->Add(&obj);
            idx->getDummyTree()->Add(&obj);
        }
    }

    return (!hasError());
}

/**
* Choose what kind of DML statement the current one could be.
*
* @return The kind of the current statement.
*/
int ComplexDMLStatement::chooseDMLStatement(){

    //1 - INSERT INTO
    //2 - SELECT
    //3 - DELETE
    //4 - UPDATE

    int result = -1;

    try {
        if (getParser()->getToken(0)->toUpperLexem() == "INSERT") {
            if (getParser()->getToken(1)->toUpperLexem() == "INTO") {
                result = ComplexDMLStatement::COMPLEX_INSERT_INTO_STATEMENT;
            } else {
                getParser()->addError("INTO clause expected after INSERT.");
            }
        } else {
            if (getParser()->getToken(0)->toUpperLexem() == "SELECT") {
                result = ComplexDMLStatement::COMPLEX_SELECT_STATEMENT;
            } else {
                if (getParser()->getToken(0)->toUpperLexem() == "DELETE") {
                    result = ComplexDMLStatement::COMPLEX_DELETE_STATEMENT;
                } else {
                    if (getParser()->getToken(0)->toUpperLexem() == "UPDATE") {
                        result = ComplexDMLStatement::COMPLEX_UPDATE_STATEMENT;
                    }
                }
            }
        }
    } catch (std::exception *ex){
        getParser()->addError("Invalid Command.");
    }catch (...) {
        getParser()->addError("Invalid Command.");
    }

    return result;
}

/**
* Generates the auxiliary INSERT INTO statements. </br>
* Insert the respectiv values on the metric trees. </br>
*
* @return true If the auxiliary INSERT INTO can be built, false otherwise.
*/
bool ComplexDMLStatement::insertImageIPV(string tableName, string stillImageName, string imageFile, string imageId){

    string sql;

    vector< vector<string> > answer;
    vector<string> metricName, indexFile;
    string distanceFunctionCode;

    sql =  "SELECT DISTINCT ms.MetricName, im.IndexFile ";
    sql += "FROM CDD$ComplexAttribMetric im, CDD$MetricStruct ms ";
    sql += "WHERE im.MetricCode = ms.MetricCode ";
    sql += "AND im.TableName = '" + tableName + "' ";
    sql += "AND im.ComplexAttribName = '" + stillImageName + "' ";

    answer = getDictionary()->runSubQuery(sql);

    for (int x = 0; x < answer.size(); x++){
        metricName.push_back(answer[x][0]);
        indexFile.push_back(answer[x][1]);
    }
    answer.clear();

    SirenComplexObject<double> *complexObject = new SirenComplexObject<double>(imageFile);

    Lexical lex;

    if (!complexObject->isImage()){
        getParser()->addError("Type help to see the supported image file formats.");
        getParser()->addError(imageFile + " does not exists or the extension " + complexObject->getExtension() + " is not supported.");
    } else {
        sql =  "INSERT INTO IPV$" + tableName + "_" + stillImageName + "( image_id , image ) ";
        sql += "VALUES ( \'" + imageId + "\' , \'" + lex.toHexaDecimal(complexObject->getImage()->serializeToString()) + "\' )";

        for (int i = 0; i < metricName.size(); i++){

            distanceFunctionCode = getDictionary()->getDistanceFunction(tableName, stillImageName, metricName[i]);

            IndexManager *idx = new IndexManager();
            SlimTree *slimTree = idx->openSlimTree(indexFile[i], atoi(distanceFunctionCode.c_str()));
            DummyTree *dummyTree = idx->openDummyTree(indexFile[i], atoi(distanceFunctionCode.c_str()));

            // setting the weights
            //SetWeights(slimtree->GetMetricEvaluator(), tablename, stillimagename, MetricName->Strings[i]);
            //SetWeights(dummytree->GetMetricEvaluator(), tablename, stillimagename, MetricName->Strings[i]);

            commandList.push_back(syntactic->updateIPVTableSetCharacteristic(tableName, stillImageName, metricName[i], complexObject, imageId));

            BasicArrayObject<double> *signature = new BasicArrayObject<double>();

            // When we invoke updateIPVTable...() method, internally one extractor is attributed
            // for our SirenComplexObject.
            // If we needyou can recovery through dictionary functions
            signature = complexObject->extractFeatures(complexObject->getExtractor()->getExtractorClass());
            signature->setOID(atol(imageId.c_str()));

            slimTree->Add(signature);
            dummyTree->Add(signature);
        }

        commandList.push_back(sql);
    }

    sql.clear();
    metricName.clear();
    indexFile.clear();

    delete (complexObject);

    return (!hasError());
}

// WARNING:
//  Apollo must be fully integrated!
bool ComplexDMLStatement::insertAudioAPV(string tableName, string audioImageName, string audioFile, string audioId) {

    /*string sql;
    bool ret = true;

    vector< vector<string> > answer;
    vector<string> metricName, indexFile;
    string distanceFunctionCode;

    sql =  "SELECT DISTINCT ms.MetricName, im.IndexFile ";
    sql += "FROM CDD$ComplexAttribMetric im, CDD$MetricStruct ms ";
    sql += "WHERE im.MetricCode = ms.MetricCode ";
    sql += "AND im.TableName = '" + tableName + "' ";
    sql += "AND im.ComplexAttribName = '" + audioName + "' ";

    answer = getDictionary()->runSubQuery(sql);

    SirenComplexObject<double> *complexObject = new SirenComplexObject<double>(imageFile);

    if (!complexObject->isAudio()){
        getParser()->addError(audioFile + " is not a valid name for an audio object or cannot be open.");
        ret = false;
    } else {
        sql =  "INSERT INTO APV$" + tableName + "_" +audioName + "( audio_id , image ) ";
        sql += "VALUES ( " + audioId + " , " + complexObject->getImage()->serializeDataToString() + " )";//getAudio()

        for (int i = 0; i < metricName.size(); i++){

            distanceFunctionCode = getDictionary()->getDistanceFunction(tableName, audioName, metricName[i]);

            IndexManager *idx = new IndexManager();
            SlimTree *slimTree = idx->openSlimTree(indexFile[i], atoi(distanceFunctionCode.c_str()));
            DummyTree *dummyTree = idx->openDummyTree(indexFile[i], atoi(distanceFunctionCode.c_str()));

            // setting the weights
            //SetWeights(slimtree->GetMetricEvaluator(), tablename, stillimagename, MetricName->Strings[i]);
            //SetWeights(dummytree->GetMetricEvaluator(), tablename, stillimagename, MetricName->Strings[i]);

            //commandList.push_back(syntactic->updateAPVTableSetCharacteristic(tableName, stillImageName, metricName[i], complexObject, imageId));

            BasicArrayObject<double> *signature = new BasicArrayObject<double>();

            // When we invoke updateAPVTable...() method, internally one extractor is attributed
            // for our SirenComplexObject.
            signature = complexObject->extractFeatures(complexObject->getAssociatedExtractor()->getExtractorClass());
            signature->setOID(atoi(imageId.c_str()));

            slimTree->Add(signature);
            dummyTree->Add(signature);

            //commandList.push_back(sql);
        }
    }

    sql.clear();
    metricName.clear();
    indexFile.clear();

    delete (complexObject);

    return (!hasError());*/
}

vector<Token> ComplexDMLStatement::rebuildSelectComplexColumnsToSelectRegularColumns(vector<Token> tokenList, int scope){

    Lexical lex;
    vector<Token> newSelectJoinConditionsList;
    vector<Token> newSelectStatement;
    string tableName;
    int tk = 0;
    int joinPosition = 0;
    vector< Token> auxAsteriskReplaceList;
    vector<string> tableImgAttribList;
    vector<string> tableAudAttribList;
    vector<string> tables;
    vector<string> complexAttribute;

    bool firstTable = false;
    bool modified = false;

    Token *aux;
    while (tk < tokenList.size()){

        if (tokenList[tk].getLexemType() == Lexical::LK_TABLE_NAME){
            // verifies if the table has image attributes
            tableName = tokenList[tk].getLexem();

            bool alreadyTranslated = false;
            for (int k = 0; k < tables.size(); k++){
                if (lex.toUpper(tableName) == lex.toUpper(tables[k]))
                    alreadyTranslated = true;
            }
            tables.push_back(tableName);

            tableImgAttribList.clear();
            tableAudAttribList.clear();

            if (!alreadyTranslated){
                tableImgAttribList = getDictionary()->getStillimageAttribute(tableName);
                tableAudAttribList = getDictionary()->getAudioAttribute(tableName);
            }

            if ((tableImgAttribList.size() > 0) || (tableAudAttribList.size() > 0)){
                joinPosition = tk;
                newSelectJoinConditionsList.clear();

                if (firstTable){
                    aux = new Token(",", Token::TK_COMMA, Lexical::LK_UNDEFINED);
                    aux->setScopePosition(scope);
                    auxAsteriskReplaceList.push_back(*aux);
                }

                for (int i = 0; i < tableImgAttribList.size(); i++){
                    complexAttribute.push_back(tableImgAttribList[i]);
                    // *** Adds the table references and join conditions in the tokenlist appropriate place ***
                    // SELECT * FROM table1 JOIN table2 ON table1.attr1 = table2.Image_Id
                    aux = new Token("JOIN", Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                    newSelectJoinConditionsList.push_back(*aux);
                    aux->setScopePosition(scope);
                    string refTable = "IPV$" + tableName;
                    aux = new Token(refTable+"_" + tableImgAttribList[i], Token::TK_IDENTIFIER, Lexical::LK_TABLE_NAME);
                    aux->setScopePosition(scope);
                    newSelectJoinConditionsList.push_back(*aux);
                    aux = new Token("ON", Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                    aux->setScopePosition(scope);
                    newSelectJoinConditionsList.push_back(*aux);
                    aux = new Token(tableName, Token::TK_IDENTIFIER, Lexical::LK_TABLE_NAME_REF);
                    aux->setScopePosition(scope);
                    newSelectJoinConditionsList.push_back(*aux);
                    aux = new Token(".", Token::TK_PERIOD, Lexical::LK_UNDEFINED);
                    aux->setScopePosition(scope);
                    newSelectJoinConditionsList.push_back(*aux);
                    aux = new Token(tableImgAttribList[i], Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                    aux->setScopePosition(scope);
                    newSelectJoinConditionsList.push_back(*aux);
                    aux = new Token("=", Token::TK_EQUAL, Lexical::LK_UNDEFINED);
                    aux->setScopePosition(scope);
                    newSelectJoinConditionsList.push_back(*aux);
                    aux = new Token(refTable+"_" + tableImgAttribList[i], Token::TK_IDENTIFIER, Lexical::LK_TABLE_NAME_REF);
                    aux->setScopePosition(scope);
                    newSelectJoinConditionsList.push_back(*aux);
                    aux = new Token(".", Token::TK_PERIOD, Lexical::LK_UNDEFINED);
                    aux->setScopePosition(scope);
                    newSelectJoinConditionsList.push_back(*aux);
                    aux = new Token("image_id", Token::TK_EQUAL, Lexical::LK_UNDEFINED);
                    aux->setScopePosition(scope);
                    newSelectJoinConditionsList.push_back(*aux);
                    aux = new Token(refTable + "_" + tableImgAttribList[i], Token::TK_IDENTIFIER, Lexical::LK_TABLE_NAME_REF);
                    aux->setScopePosition(scope);
                    auxAsteriskReplaceList.push_back(*aux);
                    aux = new Token(".", Token::TK_PERIOD, Lexical::LK_UNDEFINED);
                    aux->setScopePosition(scope);
                    auxAsteriskReplaceList.push_back(*aux);
                    //if (scope == 0){
                        aux = new Token("image", Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                        aux->setScopePosition(scope);
                        auxAsteriskReplaceList.push_back(*aux);
                    /*} else {
                        aux = new Token("*", Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                        aux->setScopePosition(scope);
                        auxAsteriskReplaceList.push_back(*aux);
                    }*/
                    if (scope == 0) {
                        // adding "as"
                        aux = new Token("AS", Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                        aux->setScopePosition(scope);
                        auxAsteriskReplaceList.push_back(*aux);
                        aux = new Token(tableImgAttribList[i], Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                        aux->setScopePosition(scope);
                        auxAsteriskReplaceList.push_back(*aux);
                    }
                    // adding ","
                    if (i+1 != tableImgAttribList.size()){
                        aux = new Token(",", Token::TK_COMMA, Lexical::LK_UNDEFINED);
                        aux->setScopePosition(scope);
                        auxAsteriskReplaceList.push_back(*aux);
                    }
                }

                if ((tableImgAttribList.size() > 0) && (tableAudAttribList.size() > 0)){
                    aux = new Token(",", Token::TK_COMMA, Lexical::LK_UNDEFINED);
                    aux->setScopePosition(scope);
                    auxAsteriskReplaceList.push_back(*aux);
                }

                //Updates the tokenlist with the join conditions and adds
                //the audio attributes references in an auxiliary tokenlist
                for (int i = 0; i < tableAudAttribList.size(); i++) {
                    complexAttribute.push_back(tableAudAttribList[i]);
                    // *** Adds the table references and join conditions in the tokenlist appropriate place ***
                    // SELECT * FROM table1 JOIN table2 ON table1.attr1 = table2.Audio_Id
                    aux = new Token("JOIN", Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                    aux->setScopePosition(scope);
                    newSelectJoinConditionsList.push_back(*aux);
                    string refTable = "APV$" + tableName + "_" + tableAudAttribList[i];
                    aux->setScopePosition(scope);
                    aux = new Token(refTable, Token::TK_IDENTIFIER, Lexical::LK_TABLE_NAME);
                    aux->setScopePosition(scope);
                    newSelectJoinConditionsList.push_back(*aux);
                    aux = new Token("ON", Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                    aux->setScopePosition(scope);
                    newSelectJoinConditionsList.push_back(*aux);
                    aux = new Token(tableName, Token::TK_IDENTIFIER, Lexical::LK_TABLE_NAME_REF);
                    aux->setScopePosition(scope);
                    newSelectJoinConditionsList.push_back(*aux);
                    aux = new Token(".", Token::TK_PERIOD, Lexical::LK_UNDEFINED);
                    aux->setScopePosition(scope);
                    newSelectJoinConditionsList.push_back(*aux);
                    aux = new Token(tableAudAttribList[i], Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                    aux->setScopePosition(scope);
                    newSelectJoinConditionsList.push_back(*aux);
                    aux = new Token("=", Token::TK_EQUAL, Lexical::LK_UNDEFINED);
                    aux->setScopePosition(scope);
                    newSelectJoinConditionsList.push_back(*aux);
                    aux = new Token(refTable, Token::TK_IDENTIFIER, Lexical::LK_TABLE_NAME_REF);
                    aux->setScopePosition(scope);
                    newSelectJoinConditionsList.push_back(*aux);
                    aux = new Token(".", Token::TK_PERIOD, Lexical::LK_UNDEFINED);
                    aux->setScopePosition(scope);
                    newSelectJoinConditionsList.push_back(*aux);
                    aux = new Token("audio_id", Token::TK_EQUAL, Lexical::LK_UNDEFINED);
                    aux->setScopePosition(scope);
                    newSelectJoinConditionsList.push_back(*aux);
                    aux = new Token(refTable + "_" + tableAudAttribList[i], Token::TK_IDENTIFIER, Lexical::LK_TABLE_NAME_REF);
                    aux->setScopePosition(scope);
                    auxAsteriskReplaceList.push_back(*aux);
                    aux = new Token(".", Token::TK_PERIOD, Lexical::LK_UNDEFINED);
                    aux->setScopePosition(scope);
                    auxAsteriskReplaceList.push_back(*aux);
                    //if (scope == 0){
                        aux = new Token("audio", Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                        aux->setScopePosition(scope);
                        auxAsteriskReplaceList.push_back(*aux);
                    /*} else {
                        aux =new Token("*", Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                        aux->setScopePosition(scope);
                        auxAsteriskReplaceList.push_back(*aux);
                    }*/
                    if (scope == 0) {
                        aux = new Token("AS", Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                        aux->setScopePosition(scope);
                        auxAsteriskReplaceList.push_back(*aux);
                        aux = new Token(tableAudAttribList[i], Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                        aux->setScopePosition(scope);
                        auxAsteriskReplaceList.push_back(*aux);
                    }
                    if (i+1 != tableAudAttribList.size()){
                        aux = new Token(",", Token::TK_COMMA, Lexical::LK_UNDEFINED);
                        aux->setScopePosition(scope);
                        auxAsteriskReplaceList.push_back(*aux);
                    }
                }
                firstTable = true;
            }

            if (joinPosition > 0){
                newSelectStatement.clear();

                for (int x = 0; x < joinPosition+1; x++){
                    newSelectStatement.push_back(tokenList[x]);
                }
                for (int x = 0; x < newSelectJoinConditionsList.size(); x++){
                    newSelectStatement.push_back(newSelectJoinConditionsList[x]);
                }
                for (int x = joinPosition+1; x < tokenList.size(); x++){
                    newSelectStatement.push_back(tokenList[x]);
                }
                tokenList.clear();
                tokenList = newSelectStatement;

                joinPosition = 0;
                tk = tk + newSelectJoinConditionsList.size();

                newSelectJoinConditionsList.clear();
                modified = true;
            }
        }
        tk++;
    }

    vector<Token> attrList;
    int pFrom = 0;
    if (modified){
        for (pFrom = 1; ((pFrom < tokenList.size()) && (tokenList[pFrom].toUpperLexem() != "FROM")); pFrom++){
            if (tokenList[pFrom].getLexem() == ","){
                bool isReplaced = false;
                for (int m = 0; m < complexAttribute.size(); m++){
                    if (lex.toUpper(tokenList[pFrom+1].getLexem()) == lex.toUpper(complexAttribute[m]))
                        isReplaced = true;
                }
                if (!isReplaced){
                    attrList.push_back(tokenList[pFrom]);
                } else {
                    pFrom++;
                }
            } else {
                bool isReplaced = false;
                for (int m = 0; m < complexAttribute.size(); m++){
                    if (lex.toUpper(tokenList[pFrom].getLexem()) == lex.toUpper(complexAttribute[m]))
                        isReplaced = true;
                }
                if (!isReplaced){
                    attrList.push_back(tokenList[pFrom]);
                }
            }
        }
    }

    for (int x = 0; x < auxAsteriskReplaceList.size(); x++){
        if ((x == 0) && (attrList.size() > 0)){
            aux = new Token(",", Token::TK_COMMA, Lexical::LK_UNDEFINED);
            aux->setScopePosition(scope);
            attrList.push_back(*aux);
        }
        attrList.push_back(auxAsteriskReplaceList[x]);
    }

    newSelectStatement.clear();
    // If has complex elements...
    newSelectStatement.push_back(tokenList[0]);
    for (int x = 0; x < attrList.size(); x++){
        newSelectStatement.push_back(attrList[x]);
    }
    for (int x = pFrom; x < tokenList.size(); x++){
        newSelectStatement.push_back(tokenList[x]);
    }

    tokenList.clear();
    tokenList = newSelectStatement;

    auxAsteriskReplaceList.clear();
    tableImgAttribList.clear();
    tableAudAttribList.clear();
    newSelectJoinConditionsList.clear();
    attrList.clear();
    tables.clear();
    complexAttribute.clear();

    vector<Token *> set;
    for (int x = 0; x < tokenList.size(); x++){
        tokenList[x].setScopePosition(scope);
        set.push_back(&tokenList[x]);
    }

    getParser()->setTokenList(set, scope);

    set.clear();
    return tokenList;
}

vector<Token> ComplexDMLStatement::rebuildSelectAsteriskToSelectColumns(vector<Token> tokenList, int scope){

    if (tokenList[1].getLexem() != "*"){
        return tokenList;
    }

    vector<Token> newSelectJoinConditionsList;
    vector<Token> newSelectStatement;
    string tableName;
    int tk = 0;
    int joinPosition = 0;
    vector< Token> auxAsteriskReplaceList;
    vector<string> tableImgAttribList;
    vector<string> tableAudAttribList;
    bool firstTable = false;

    while (tk < tokenList.size()){

        if (tokenList[tk].getLexemType() == Lexical::LK_TABLE_NAME){
            // verifies if the table has image attributes
            tableName = tokenList[tk].getLexem();
            tableImgAttribList = getDictionary()->getStillimageAttribute(tableName);
            tableAudAttribList = getDictionary()->getAudioAttribute(tableName);

            if ((tableImgAttribList.size() > 0) || (tableAudAttribList.size() > 0)){
                joinPosition = tk;
                newSelectJoinConditionsList.clear();

                Token *aux;

                if (firstTable){
                    aux = new Token(",", Token::TK_COMMA, Lexical::LK_UNDEFINED);
                    auxAsteriskReplaceList.push_back(*aux);
                }

                for (int i = 0; i < tableImgAttribList.size(); i++){

                    // *** Adds the table references and join conditions in the tokenlist appropriate place ***
                    // SELECT * FROM table1 JOIN table2 ON table1.attr1 = table2.Image_Id
                    aux = new Token("JOIN", Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                    newSelectJoinConditionsList.push_back(*aux);
                    string refTable = "IPV$" + tableName;
                    aux = new Token(refTable+"_" + tableImgAttribList[i], Token::TK_IDENTIFIER, Lexical::LK_TABLE_NAME);
                    newSelectJoinConditionsList.push_back(*aux);
                    aux = new Token("ON", Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                    newSelectJoinConditionsList.push_back(*aux);
                    aux = new Token(tableName, Token::TK_IDENTIFIER, Lexical::LK_TABLE_NAME_REF);
                    newSelectJoinConditionsList.push_back(*aux);
                    aux = new Token(".", Token::TK_PERIOD, Lexical::LK_UNDEFINED);
                    newSelectJoinConditionsList.push_back(*aux);
                    aux = new Token(tableImgAttribList[i], Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                    newSelectJoinConditionsList.push_back(*aux);
                    aux = new Token("=", Token::TK_EQUAL, Lexical::LK_UNDEFINED);
                    newSelectJoinConditionsList.push_back(*aux);
                    aux = new Token(refTable+"_" + tableImgAttribList[i], Token::TK_IDENTIFIER, Lexical::LK_TABLE_NAME_REF);
                    newSelectJoinConditionsList.push_back(*aux);
                    aux = new Token(".", Token::TK_PERIOD, Lexical::LK_UNDEFINED);
                    newSelectJoinConditionsList.push_back(*aux);
                    aux = new Token("image_id", Token::TK_EQUAL, Lexical::LK_UNDEFINED);
                    newSelectJoinConditionsList.push_back(*aux);
                    aux = new Token(refTable + "_" + tableImgAttribList[i], Token::TK_IDENTIFIER, Lexical::LK_TABLE_NAME_REF);
                    auxAsteriskReplaceList.push_back(*aux);
                    aux = new Token(".", Token::TK_PERIOD, Lexical::LK_UNDEFINED);
                    auxAsteriskReplaceList.push_back(*aux);
                    if (scope == 0){
                        aux = new Token("image", Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                        auxAsteriskReplaceList.push_back(*aux);
                    } else {
                        aux = new Token("*", Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                        auxAsteriskReplaceList.push_back(*aux);
                    }

                    if (scope == 0) {
                        // adding "as"
                        aux = new Token("AS", Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                        auxAsteriskReplaceList.push_back(*aux);
                        aux = new Token(tableImgAttribList[i], Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                        auxAsteriskReplaceList.push_back(*aux);
                    }

                    // adding ","
                    if (i+1 != tableImgAttribList.size()){
                        aux = new Token(",", Token::TK_COMMA, Lexical::LK_UNDEFINED);
                        auxAsteriskReplaceList.push_back(*aux);
                    }
                }

                if ((tableImgAttribList.size() > 0) && (tableAudAttribList.size() > 0)){
                    aux = new Token(",", Token::TK_COMMA, Lexical::LK_UNDEFINED);
                    auxAsteriskReplaceList.push_back(*aux);
                }

                //Updates the tokenlist with the join conditions and adds
                //the audio attributes references in an auxiliary tokenlist
                for (int i = 0; i < tableAudAttribList.size(); i++) {

                    // *** Adds the table references and join conditions in the tokenlist appropriate place ***
                    // SELECT * FROM table1 JOIN table2 ON table1.attr1 = table2.Audio_Id
                    aux = new Token("JOIN", Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                    newSelectJoinConditionsList.push_back(*aux);
                    string refTable = "APV$" + tableName + "_" + tableAudAttribList[i];
                    aux = new Token(refTable, Token::TK_IDENTIFIER, Lexical::LK_TABLE_NAME);
                    newSelectJoinConditionsList.push_back(*aux);
                    aux = new Token("ON", Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                    newSelectJoinConditionsList.push_back(*aux);
                    aux = new Token(tableName, Token::TK_IDENTIFIER, Lexical::LK_TABLE_NAME_REF);
                    newSelectJoinConditionsList.push_back(*aux);
                    aux = new Token(".", Token::TK_PERIOD, Lexical::LK_UNDEFINED);
                    newSelectJoinConditionsList.push_back(*aux);
                    aux = new Token(tableAudAttribList[i], Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                    newSelectJoinConditionsList.push_back(*aux);
                    aux = new Token("=", Token::TK_EQUAL, Lexical::LK_UNDEFINED);
                    newSelectJoinConditionsList.push_back(*aux);
                    aux = new Token(refTable, Token::TK_IDENTIFIER, Lexical::LK_TABLE_NAME_REF);
                    newSelectJoinConditionsList.push_back(*aux);
                    aux = new Token(".", Token::TK_PERIOD, Lexical::LK_UNDEFINED);
                    newSelectJoinConditionsList.push_back(*aux);
                    aux = new Token("audio_id", Token::TK_EQUAL, Lexical::LK_UNDEFINED);
                    newSelectJoinConditionsList.push_back(*aux);
                    aux = new Token(refTable + "_" + tableAudAttribList[i], Token::TK_IDENTIFIER, Lexical::LK_TABLE_NAME_REF);
                    auxAsteriskReplaceList.push_back(*aux);
                    aux = new Token(".", Token::TK_PERIOD, Lexical::LK_UNDEFINED);
                    auxAsteriskReplaceList.push_back(*aux);
                    if (scope == 0){
                        aux = new Token("audio", Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                        auxAsteriskReplaceList.push_back(*aux);
                    } else {
                        aux =new Token("*", Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                        auxAsteriskReplaceList.push_back(*aux);
                    }
                    if (scope == 0) {
                        aux = new Token("AS", Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                        auxAsteriskReplaceList.push_back(*aux);
                        aux = new Token(tableAudAttribList[i], Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                        auxAsteriskReplaceList.push_back(*aux);
                    }
                    if (i+1 != tableAudAttribList.size()){
                        aux = new Token(",", Token::TK_COMMA, Lexical::LK_UNDEFINED);
                        auxAsteriskReplaceList.push_back(*aux);
                    }
                }

                vector<string> attribList = syntactic->getNonComplexAttributes(tableName);
                // Verifies if is needed to add an "," in the selectlist
                if (attribList.size() > 0){
                    aux = new Token(",", Token::TK_COMMA, Lexical::LK_UNDEFINED);
                    auxAsteriskReplaceList.push_back(*aux);
                }
                for (int i = 0; i < attribList.size(); i++){
                    aux = new Token(tableName, Token::TK_IDENTIFIER, Lexical::LK_TABLE_NAME_REF);
                    auxAsteriskReplaceList.push_back(*aux);
                    aux = new Token(".", Token::TK_PERIOD, Lexical::LK_UNDEFINED);
                    auxAsteriskReplaceList.push_back(*aux);
                    aux = new Token(attribList[i], Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                    auxAsteriskReplaceList.push_back(*aux);
                    if (i+1 != attribList.size()){
                        aux = new Token(",", Token::TK_COMMA, Lexical::LK_UNDEFINED);
                        auxAsteriskReplaceList.push_back(*aux);
                    }
                }
                attribList.clear();

                firstTable = true;
            }

            if (joinPosition > 0){
                newSelectStatement.clear();
                for (int x = 0; x < joinPosition+1; x++){
                    newSelectStatement.push_back(tokenList[x]);
                }
                for (int x = 0; x < newSelectJoinConditionsList.size(); x++){
                    newSelectStatement.push_back(newSelectJoinConditionsList[x]);
                }
                for (int x = joinPosition+1; x < tokenList.size(); x++){
                    newSelectStatement.push_back(tokenList[x]);
                }
                tokenList.clear();
                tokenList = newSelectStatement;

                joinPosition = 0;
                tk = tk + newSelectJoinConditionsList.size();

                newSelectJoinConditionsList.clear();
            }
        }
        tk++;
    }

    newSelectStatement.clear();
    // If has complex elements...
    // Removes the "*" from the select clause and includes the attributes of the selectlist
    newSelectStatement.push_back(tokenList[0]);
    for (int x = 0; x < auxAsteriskReplaceList.size(); x++){
        newSelectStatement.push_back(auxAsteriskReplaceList[x]);
    }
    for (int x = 2; x < tokenList.size(); x++){
        newSelectStatement.push_back(tokenList[x]);
    }

    tokenList.clear();
    tokenList = newSelectStatement;

    auxAsteriskReplaceList.clear();
    tableImgAttribList.clear();
    tableAudAttribList.clear();
    newSelectJoinConditionsList.clear();

    vector<Token *> set;
    for (int x = 0; x < tokenList.size(); x++){
        tokenList[x].setScopePosition(scope);
        set.push_back(&tokenList[x]);
    }

    getParser()->setTokenList(set, scope);

    set.clear();
    return tokenList;
}




// *** EXAMPLE ***
// --> statement defined by the user:
// SELECT ...
// FROM ...
// WHERE complex_obj_reference NEAR/FAR [similarity_grouping]
//       complex_obj_definition [BY metric_name]
//       [RANGE value]
//       [STOP AFTER k]
// --> statement that will be executed by SIREN:
// SELECT ...
// FROM ...
// WHERE complex_obj_id_reference IN (complex_obj_id_reference_list)
vector<Token> ComplexDMLStatement::rebuildSelectKnnOrRangeToSelectInList(){

    Semantic *semantics = new Semantic(getDictionary());
    //Instance to use semantics methods.

    string predicate;                     //Identifies token NEAR or FAR
    int fileQueryObject;                  //Pointer to query center name on token list
    string metricName;                    //Stores metric name
    vector<string> tbNameList;            //Stores table that contains complex data
    string tableName;                     //Store current table that should have complex data
    string tableName2;                    //Store second table (comp data) to eventual join
    string attribName, attribName2;       //Names of the complex attributes on table
    string attribType;                    //Type of current complex attribute (e.g.: STILLIMAGE)
    string sgType;                        //Possible similarity grouping (e.g.: SUM, AVG, etc)
    vector<string> attrList;              //Auxiliar table to particulate
    vector<string> parameterValueList;    //-Local processing- parameter value list
    vector<string> parameterList;         //-Local processing- parameter list
    vector<string> parameterTypeList;     //-Local processing- parameter type list
    int parameterCount = 0;               //-Local processing- for particulate parameter
    string imageFileName;                 //Stores eventual file name on the query
    string subQuery;                      //Stores eventual sub query
    string knn;                           //Stores eventual knn value
    bool tieList = false;                 //Defines the tie list
    string range;                         //Stores the eventual range value
    BasicArrayObject<double> **queryObjects = NULL; //Query center(s) object(s)
    int queryObjectsCount = 0;            //Quantity of query center(s)
    //Note: We can't use vector<> to store query centers in order to maintain
    //compatibility with arboretum. Of course, pointers are faster.
    vector< vector<string> > resultSubQuery; //-Local process - subquery
    vector<string> fieldsName;               //-Local process - subquery

    //Gets the current token list to process.
    //Defined by scope.
    vector<Token> list = getParser()->getTokenList(getParser()->getActiveScope());
    int plist = 0;
    int activeList = 0;
    plist++;

    while ((plist < list.size()) && (plist > 0)) {

        if ((list[plist].toUpperLexem() != "NEAR") && (list[plist].toUpperLexem() != "FAR")) {
            activeList++;
        } else {
            tableName2.clear();
            attribName2.clear();
            metricName.clear();

            predicate = list[plist].getLexem();
            attribName = list[plist-1].getLexem();

            //Gets table name
            if (list[plist-2].getLexem() == "."){
                tableName = list[plist-3].getLexem();
            } else {
                //In this case the tbNameList will have only one item
                vector<string> tbNameList = syntactic->getTableNameFromTokenList(getParser()->getTokenList(getParser()->getActiveScope()), attribName);
                tableName = tbNameList[tbNameList.size()-1];
                tbNameList.clear();
            }

            attribType = getDictionary()->getComplexAttributeReference(tableName, attribName);

            plist++;

            if (list[plist].getLexemType() == Lexical::LK_SUM_SG) {
                sgType = "SUM";
                plist++;
            } else {
                if (list[plist].getLexemType() == Lexical::LK_AVG_SG) {
                    sgType = "AVG";
                    plist++;
                } else {
                    if (list[plist].getLexemType() == Lexical::LK_ALL_SG) {
                        sgType = "ALL";
                        plist++;
                    } else {
                        if (list[plist].getLexemType() == Lexical::LK_EVERY_SG) {
                            sgType = "EVERY";
                            plist++;
                        } else {
                            if (list[plist].getLexemType() == Lexical::LK_ANY_SG) {
                                sgType = "ANY";
                                plist++;
                            } else {
                                if (list[plist].getLexemType() == Lexical::LK_MAX_SG) {
                                    sgType = "MAX";
                                    plist++;
                                }
                            }
                        }
                    }
                }
            }

            //Treats ( < SUBSELECT > ) or ( < PARTICULATE_REFERENCE > )
            if (list[plist].getTokenType() == Token::TK_OPEN_BRACE) {
                //Skipping token '('
                plist++;

                //SUBSELECT
                if (list[plist].toUpperLexem() == "SUBSELECT") {
                    activeList++;
                    fileQueryObject = activeList-1;

                    //Getting scope
                    int scope = list[plist].getScopePosition();
                    int earlier = getParser()->getActiveScope();
                    getParser()->setActiveScope(scope);

                    //Recursive call
                    //Turning subselect back into select
                    for (int x = 0; x < getParser()->countTokens(); x++){
                        if (getParser()->getToken(x)->getScopePosition() == getParser()->getActiveScope()){
                            if (getParser()->getToken(x)->toUpperLexem() == "SUBSELECT"){
                                getParser()->getToken(x)->setLexem("SELECT");
                            }
                        }
                    }

                    rebuildSelectKnnOrRangeToSelectInList();
                    getParser()->setActiveScope(earlier);

                    //Retorno backtracking
                    list = getParser()->getTokenList(getParser()->getActiveScope());

                    subQuery.clear();
                    int closeBrace = list.size()-1;
                    while ((closeBrace >= 0) && (list[closeBrace].getLexem() != ")")){
                        closeBrace--;
                    }
                    for (int x = plist; x < closeBrace; x++){
                        subQuery += list[x].getLexem();
                        subQuery += " ";
                    }

                    int auxCount = getDictionary()->getNumberOfPossibleTuples(subQuery);

                    if (auxCount == -1) {
                        getParser()->addError("Execution stoped.");
                        getParser()->addError("You must inform a valid one (or multiple) query center.");
                        return list;
                    }

                    plist = getParser()->getCurrentPosition();

                    resultSubQuery.clear();
                    fieldsName.clear();

                    resultSubQuery = getDictionary()->runSubQuery(subQuery);
                    fieldsName = getDictionary()->getFieldsName(subQuery);

                    if (attribType == "PARTICULATE"){
                        string paramType;
                        if (sgType.empty()){
                            if (auxCount == 1) {
                                if (resultSubQuery.size() > 0){
                                    parameterCount = resultSubQuery[0].size();
                                } else{
                                    parameterCount = 0;
                                }
                                for (int i = 0; i < parameterCount; i++) {
                                    parameterList.push_back(fieldsName[i]);
                                    paramType = getDictionary()->getParticleType(tableName, attrList[i]);
                                    if (paramType == ""){
                                        getParser()->addError("Invalid parameter reference" + parameterList[i] + ".");
                                    } else {
                                        parameterTypeList.push_back(paramType);
                                    }
                                    parameterValueList.push_back(fieldsName[i]);
                                }
                            } else {
                                getParser()->addError("After " + predicate + " it is expected only one valid particulate reference.");
                            }
                        } else {
                            // otherwise if the query has a similarity grouping clause,
                            // then the subquery can return more than one particulate object
                            for (int x = 0; x < resultSubQuery.size(); x++) {
                                if (resultSubQuery.size() > 0){
                                    parameterCount = resultSubQuery[0].size();
                                } else{
                                    parameterCount = 0;
                                }
                                for (int i = 0; i < parameterCount; i++) {
                                    parameterList.push_back(fieldsName[i]);
                                    paramType = getDictionary()->getParticleType(tableName, attrList[i]);
                                    if (paramType.empty()){
                                        getParser()->addError("Invalid parameter reference " + parameterList[i] + ".");
                                    } else {
                                        parameterTypeList.push_back(paramType);
                                    }
                                    parameterValueList.push_back(fieldsName[i]);
                                }
                            }
                        }
                    }

                    closeBrace = list.size()-1;
                    while ((closeBrace >= 0) && (list[closeBrace].getLexem() != ")")){
                        closeBrace--;
                    }
                    //Skipping token 'subselect'
                    plist = closeBrace;
                    //Skipping token 'subselect'
                } else { //END SUBSELECT
                    activeList++;
                    fileQueryObject = activeList-1;

                    while(list[plist].getTokenType() != Token::TK_CLOSE_BRACE) {

                        parameterValueList.push_back(list[plist].getLexem());
                        if (list[plist].getTokenType() == Token::TK_INTEGER) {
                            parameterTypeList.push_back(getDictionary()->getDbmsDataType("integer"));
                        }
                        else if (list[plist].getTokenType() == Token::TK_FLOATING_POINT) {
                            parameterTypeList.push_back(getDictionary()->getDbmsDataType("float"));
                        }

                        //Skipping the parameter value
                        plist++;
                        //Skipping the lexem "as"
                        plist++;
                        parameterList.push_back(list[plist].getLexem());

                        //Skipping the parameter name
                        plist++;
                        if (list[plist].getTokenType() == Token::TK_COMMA){
                            plist++;
                        }

                        //Computing parameter
                        parameterCount++;
                    }
                }
                //Skipping token ')'
                plist++;
            } else { //Else open brace
                //Treats file name
                if (list[plist].getTokenType() == Token::TK_STRING) {

                    if (attribType == "STILLIMAGE"){
                        activeList++;
                        fileQueryObject = activeList-1;
                        imageFileName = list[plist].getLexem();
                        plist++;
                    }
                    //Treats audio
                    //Treats financial time series
                } else {
                    //Treats complex attribute name reference (join)
                    if (list[plist].getTokenType() == Token::TK_IDENTIFIER) {
                        //Getting the second table name and the second attribute name
                        if ((plist + 1 < list.size()) && (list[plist+1].getLexem() == ".")) {
                            tableName2 = list[plist].getLexem();
                            plist += 2;
                            attribName2 = list[plist].getLexem();
                        } else {
                            //In this case the tbNameList will have only one item
                            attribName2 = list[plist].getLexem();
                            tbNameList.clear();
                            tbNameList = syntactic->getTableNameFromTokenList(getParser()->getTokenList(getParser()->getActiveScope()), attribName2);
                            tableName2 = tbNameList[tbNameList.size()-1];
                        }
                        plist++;
                    }
                }
            }

            if (plist < list.size()) {

                if (list[plist].toUpperLexem() == "BY") {
                    plist++;
                    metricName = list[plist].getLexem();
                    plist++;

                    if ((attribType == "STILLIMAGE") || (attribType == "AUDIO")) {
                        //Verify if the metricName is valid for this tableName and attribName
                        if (!getDictionary()->isValidMetric(metricName, tableName, attribName)){
                            getParser()->addError("Invalid metric. '" + metricName + "' is not a previous defined metric.");
                        }
                    } else { // particulate
                        int metricCode = atoi(getDictionary()->getValidMetric(metricName, tableName, attribName).c_str());
                        if (metricCode > 0) {
                            // ParamList is a list that stores the pairs [parameter,type]
                            // of a metric created for a particulate attribute
                            vector< pair<string, string> > paramAuxList;
                            paramAuxList = getDictionary()->getParameters(getDictionary()->getValidMetric(metricName, tableName, attribName));

                            if (paramAuxList.size() == parameterList.size()) {
                                for (int j = 0; j < paramAuxList.size(); j++) {

                                    //Get ParticulateParam position
                                    int ix = -1;
                                    for (int y = 0; y < paramAuxList.size(); y++){
                                        if (paramAuxList[j].first == parameterList[y]){
                                            ix = y;
                                        }
                                    }

                                    if (ix == -1) {
                                        getParser()->addError("Missing parameter: '" + paramAuxList[j].first + "'");
                                    } else {
                                        if (paramAuxList[j].second != parameterTypeList[ix])
                                            getParser()->addError("Invalid parameter type. Expected '" + paramAuxList[j].second + "' found '" + parameterTypeList[ix] + "' instead.");
                                    }
                                }
                            } else {
                                getParser()->addError("Missing or extra particulate metric parameter(s).");
                            }
                        } else {
                            getParser()->addError("Invalid metric '" + metricName + "'.");
                        }
                    }
                }

                if (list[plist].toUpperLexem() == "STOP") {
                    plist++; // stop
                    plist++; // after
                    knn = list[plist].getLexem(); // k
                    // optional tie list
                    if ((plist + 1 < list.size()) && (list[plist+1].toUpperLexem() == "WITH")) {
                        plist++; // with
                        plist++; // tie
                        plist++; // list
                        tieList = true;
                    }
                }

                if ((plist + 1 < list.size()) && (list[plist].toUpperLexem() == "RANGE")) {
                    plist++; // range
                    range = list[plist].getLexem(); // range radius
                }

                if (!hasError()) {
                    if (attribName2.empty()) {
                        //Get query center
                        queryObjects = semantics->buildQueryCenter(tableName, attribName, metricName, imageFileName, subQuery, attribType, parameterList, parameterValueList, parameterCount, &queryObjectsCount);
                        if (queryObjects == NULL){
                            getParser()->addError("Complex query aborted.");
                            getParser()->addError("Or there isn't a default metric defined as: " + metricName);
                            getParser()->addError("The value informed for query(ies) center(s) " + list[fileQueryObject].getLexem() + " is not a valid center.");
                        }
                    }
                }

                if (!hasError()) {

                    activeList = activeList-1;
                    if (attribName2.empty()) {
                        performKnnOrRangeQuery(getParser()->getTokenList(getParser()->getActiveScope()), &activeList, fileQueryObject, predicate, tableName, attribName, sgType, metricName, knn, range, tieList, queryObjects, queryObjectsCount);
                    } else {
                        performKnnOrRangeJoinQuery(getParser()->getTokenList(getParser()->getActiveScope()),&activeList,predicate, tableName, tableName2, attribName, attribName2, sgType, metricName, knn, range, tieList, 0);
                    }
                }
                parameterList.clear();
            } else {
                getParser()->addError("Missing 'STOP AFTER' and/or 'RANGE' clause.");
            }
        }
        plist++;
    }

    //Delete query center
    if (queryObjectsCount > 0) {
        for (int i = 0; i < queryObjectsCount; i++)
            delete queryObjects[i];
        delete queryObjects;
    }

    delete (semantics);

    return getParser()->getTokenList(getParser()->getActiveScope());
}

vector<Token> ComplexDMLStatement::performKnnOrRangeQuery(vector<Token> tokenList, int *activeListTail, int fileQueryObject, string predicate,string tableName, string attribName, string sgType, string metricName, string knn, string range, bool tieList, BasicArrayObject<double> **obj, int numberOfObjects){

    IndexManager *treeManager = new IndexManager();
    Lexical lexical;

    //if the user didn't specified the metric name, get the default
    if (metricName.empty()) {
        metricName = getDictionary()->getDefaultMetric(tableName, attribName);
        if ((metricName.empty()) || (metricName == "NULL")) {
            getParser()->addError("There isn't a default metric defined for " + tableName + "." + attribName + ".");
        }
    }

    //If there are errors, stop
    if (hasError())
        return tokenList;

    //Getting the distance function, better applied
    int dc = atoi(getDictionary()->getDistanceFunction(tableName, attribName, metricName).c_str());

    //Why all the methods are not implemented on Slim Tree (arboretum) ??
    //SlimTree *slimTree = treeManager->openSlimTree(getDictionary()->getIndexFile(tableName, attribName, metricName), dc);
    DummyTree *dummyTree = treeManager->openDummyTree(getDictionary()->getIndexFile(tableName, attribName, metricName), dc);

    //SetWeights(slimtree->GetMetricEvaluator(), tableName, attribName, metricName);
    //SetWeights(dummyTree->GetMetricEvaluator(), tableName, attribName, metricName);

    //Inserting a result and a query object - is a BasicArrayObject<DType>
    ComplexResult *result;

    //knn select query or range select query or similarity grouping queries
    if (!sgType.empty()) {
        //Similarity grouping queries
        //knn
        if (!knn.empty()) {
            if (sgType == "SUM") {
                //TDateTime tmp1 = Now();
                if (lexical.toUpper(predicate) == "NEAR"){
                    result = dummyTree->SumNearestQuery(obj, numberOfObjects, atoi(knn.c_str()), tieList);
                } else {
                    getParser()->addError("The FAR predicate needs implementation!");
                }
                //TDateTime tmp2 = Now();
                //MetricTreeTotalTime += tmp2 - tmp1;
            } else {
                if (sgType == "AVG") {
                    getParser()->addError("The AVG similarity grouping needs implementation!");
                } else {
                    if (sgType == "ALL") {
                        //TDateTime tmp1 = Now();
                        if (lexical.toUpper(predicate) == "NEAR"){
                            result = dummyTree->AllNearestQuery(obj, numberOfObjects, atoi(knn.c_str()), tieList);
                        } else {
                            getParser()->addError("The FAR predicate needs implementation!");
                        }
                        //TDateTime tmp2 = Now();
                        //MetricTreeTotalTime += tmp2 - tmp1;
                    } else {
                        if (sgType == "EVERY") {
                            getParser()->addError("The EVERY similarity grouping needs implementation!");
                        } else {
                            if (sgType == "ANY") {
                                getParser()->addError("The ANY similarity grouping needs implementation!");
                            } else {
                                if (sgType == "MAX") {
                                    //TDateTime tmp1 = Now();
                                    if (lexical.toUpper(predicate) == "NEAR") {
                                        result = dummyTree->MaxNearestQuery(obj, numberOfObjects, atoi(knn.c_str()), tieList);
                                    } else {
                                        getParser()->addError("The FAR predicate needs implementation!");
                                    }
                                    //TDateTime tmp2 = Now();
                                    //MetricTreeTotalTime += tmp2 - tmp1;
                                }
                            }
                        }
                    }
                }
            }
        } else {
            if (range.empty()){
                range = "0.0";
            }

            if (sgType == "SUM") {
                //TDateTime tmp1 = Now();
                if (lexical.toUpper(predicate) == "NEAR") {
                    result = dummyTree->SumRangeQuery(obj, numberOfObjects, atof(range.c_str()));
                } else {
                    getParser()->addError("The FAR predicate needs implementation!");
                    //TDateTime tmp2 = Now();
                    //MetricTreeTotalTime += tmp2 - tmp1;
                }
            } else {
                if (sgType == "AVG") {
                    getParser()->addError("The AVG similarity grouping needs implementation!");
                } else {
                    if (sgType == "ALL") {
                        //TDateTime tmp1 = Now();
                        if (lexical.toUpper(predicate) == lexical.toUpper("near")) {
                            result = dummyTree->AllRangeQuery(obj, numberOfObjects, atof(range.c_str()));
                        } else {
                            getParser()->addError("The FAR predicate needs implementation!");
                        }
                        //TDateTime tmp2 = Now();
                        //MetricTreeTotalTime += tmp2 - tmp1;
                    } else {
                        if (sgType == "EVERY") {
                            getParser()->addError("The EVERY similarity grouping needs implementation!");
                        } else {
                            if (sgType == "ANY") {
                                getParser()->addError("The ANY similarity grouping needs implementation!");
                            } else {
                                if (sgType == "MAX") {
                                    //TDateTime tmp1 = Now();
                                    if (lexical.toUpper(predicate) == "NEAR") {
                                        result = dummyTree->MaxRangeQuery(obj, numberOfObjects, atof(range.c_str()));
                                    } else {
                                        getParser()->addError("The FAR predicate needs implementation!");
                                    }
                                    //TDateTime tmp2 = Now();
                                    //MetricTreeTotalTime += tmp2 - tmp1;
                                }
                            }
                        }
                    }
                }
            }
        }
    } else {
        //knn select query
        if (!knn.empty()) {
            //TDateTime tmp1 = Now();
            if (lexical.toUpper(predicate) == "NEAR"){
                //result = slimtree->NearestQuery(obj[0], StrToInt(knn), tieList);
                result = dummyTree->NearestQuery(obj[0], atoi(knn.c_str()), tieList);
            } else {
                getParser()->addError("The FAR predicate needs implementation!");
            }
            //TDateTime tmp2 = Now();
            //MetricTreeTotalTime += tmp2 - tmp1;
        } else {
            //Range select query
            if (!range.empty()) {
                //TDateTime tmp1 = Now();
                if (lexical.toUpper(predicate) == "NEAR") {
                    //result = slimtree->RangeQuery(obj[0], StrToFloat(range));
                    result = dummyTree->RangeQuery(obj[0], atof(range.c_str()));
                } else {
                    getParser()->addError("The FAR predicate needs implementation!");
                }
                //TDateTime tmp2 = Now();
                //MetricTreeTotalTime += tmp2 - tmp1;
            } else {
                //If no query type informed, do a Range 0
                //TDateTime tmp1 = Now();
                if (lexical.toUpper(predicate) == "NEAR") {
                    //result = slimtree->RangeQuery(obj[0], 0.0);
                    result = dummyTree->RangeQuery(obj[0], 0.0);
                } else {
                    getParser()->addError("The FAR predicate needs implementation!");
                }
                //TDateTime tmp2 = Now();
                //MetricTreeTotalTime += tmp2 - tmp1;
            }
        }
    }

    //If the query returned no records
    if (!result->GetNumOfEntries()){
        // It's not an error
        // It's a warning, but should be returned as error
        // to avoid mess int subselect
        getParser()->addError("Execution stopped.");
        getParser()->addError("The " + predicate + " has returned no records.");
    } else { // else, include the query result in The token list
        // storing prior
        int prior = fileQueryObject-1;
        vector<Token> auxTokenList;

        auxTokenList.clear();

        for (int x = 0; x < prior; x++){
            auxTokenList.push_back( tokenList[x]);
        }

        Token *newToken = new Token("WHERE", Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
        newToken->setScopePosition(getParser()->getActiveScope());
        auxTokenList.push_back( *newToken );

        newToken = new Token(tableName, Token::TK_IDENTIFIER, Lexical::LK_TABLE_NAME);
        newToken->setScopePosition(getParser()->getActiveScope());
        auxTokenList.push_back( *newToken );

        newToken = new Token(".", Token::TK_PERIOD, Lexical::LK_UNDEFINED);
        newToken->setScopePosition(getParser()->getActiveScope());
        auxTokenList.push_back( *newToken );

        newToken = new Token(tokenList[fileQueryObject].getLexem(), Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
        newToken->setScopePosition(getParser()->getActiveScope());
        auxTokenList.push_back( *newToken );

        newToken = new Token("IN", Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
        newToken->setScopePosition(getParser()->getActiveScope());
        auxTokenList.push_back( *newToken );

        newToken = new Token("(", Token::TK_OPEN_BRACE, Lexical::LK_UNDEFINED);
        newToken->setScopePosition(getParser()->getActiveScope());
        auxTokenList.push_back( *newToken );

        //Inserting the objects OID int the 'IN' clause
        for (int i = 0; i < result->GetNumOfEntries(); i++){
           BasicArrayObject<double> *qq;
           qq = (BasicArrayObject<double> *)  ((*result)[i]->GetObject());
           char buf[8];
           sprintf(buf, "%ld", qq->GetOID());
           string tmpBuf = buf;
           newToken = new Token(tmpBuf, Token::TK_INTEGER, Lexical::LK_UNDEFINED);
           newToken->setScopePosition(getParser()->getActiveScope());
           auxTokenList.push_back( *newToken );
           if (i+1 < result->GetNumOfEntries()){
               newToken = new Token(",", Token::TK_COMMA, Lexical::LK_UNDEFINED);
               newToken->setScopePosition(getParser()->getActiveScope());
               auxTokenList.push_back( *newToken );
           }
        }
        newToken = new Token(")", Token::TK_CLOSE_BRACE, Lexical::LK_UNDEFINED);
        newToken->setScopePosition(getParser()->getActiveScope());
        auxTokenList.push_back( *newToken );

        *activeListTail = auxTokenList.size()-1;
        tokenList.clear();
        tokenList = auxTokenList;
        auxTokenList.clear();

        //Updates the current token list into active scope
        //Current token list = internal parser token list
        getParser()->setTokenList(tokenList, getParser()->getActiveScope());
    }



    return tokenList;
}

vector<Token> ComplexDMLStatement::performKnnOrRangeJoinQuery(vector<Token> activeListTail, int *activeList, string predicate, string tableName, string tableName2,string attribName, string attribName2, string sgType, string metricName, string knn, string range, bool tieList, int *tempJoinTableScope){

    IndexManager *treeManager = new IndexManager();
    Lexical lexical;

    if (metricName.empty()) {
        metricName = getDictionary()->getDefaultMetric(tableName, attribName);
        if ((metricName.empty()) || (metricName == "NULL")) {
            getParser()->addError("There isn't a default metric defined for " + tableName + "." + attribName + ".");
        }
    }

    //If there are errors, stop
    if (hasError())
        return activeListTail;

    //Getting the distance function
    int dc = atoi(getDictionary()->getDistanceFunction(tableName, attribName, metricName).c_str());

    //Opening The index - Slim does not support join = ok
    //SlimTree *slimTree = treeManager->openSlimTree(dictionary->getIndexFile(tableName, attribName, metricName), dc);
    DummyTree *dummyTree = treeManager->openDummyTree(getDictionary()->getIndexFile(tableName, attribName, metricName), dc);

    //SetWeights(slimtree->GetMetricEvaluator(), tablename, attribname, metricName);
    //SetWeights(dummyTree->GetMetricEvaluator(), tablename, attribname, metricName);

    // inserting a result and a query object
    ComplexJoinedResult *result;

    //Nearest join or closest join or range join
    if (!knn.empty()) {
        // closest join
        if (sgType == "ANY") {
            string idx = getDictionary()->getIndexFile(tableName2, attribName2, metricName);
            if (idx == "NULL") {
                getParser()->addError("The attribute " + tableName2 + "." + attribName2 + " not associated with " + metricName);
            } else {
                // opening The join tree
                IndexManager *joinManagerDummy = new IndexManager(getDictionary()->getDatabaseManager()->getUsername());
                DummyTree *joinDummyTree = joinManagerDummy->openDummyTree(idx, dc);

                //SetWeights(joinDummyTree->GetMetricEvaluator(), tablename2, attribname2, metricName);

                //Computing join
                //TDateTime tmp1 = Now();
                if (lexical.toUpper(predicate) == "NEAR"){
                    result = dummyTree->ClosestJoinQuery(joinDummyTree , atoi(knn.c_str()), tieList);
                } else {
                    getParser()->addError("The FAR predicate needs implementation!");
                }
                //TDateTime tmp2 = Now();
                //MetricTreeTotalTime += tmp2 - tmp1;

                // unloading The joined tree
                delete joinManagerDummy;
            }
        } else {
            // nearest join
            string idx = getDictionary()->getIndexFile(tableName2, attribName2, metricName);
            if (idx == "NULL") {
                getParser()->addError("The attribute " + tableName2 + "." + attribName2 + " not associated with " + metricName);
            } else {
                // opening The join tree
                IndexManager *joinManagerDummy = new IndexManager(getDictionary()->getDatabaseManager()->getUsername());
                DummyTree *joinDummyTree = joinManagerDummy->openDummyTree(idx, dc);

                //SetWeights(joinDummyTree->GetMetricEvaluator(), tablename2, attribname2, metricName);

                //Computing join
                //TDateTime tmp1 = Now();
                if (lexical.toUpper(predicate) == "NEAR")
                    result = dummyTree->NearestJoinQuery(joinDummyTree , atoi(knn.c_str()), tieList);
                else
                    getParser()->addError("The FAR predicate needs implementation!");
                //TDateTime tmp2 = Now();
                //MetricTreeTotalTime += tmp2 - tmp1;

                //Unloading the joined tree
                delete joinManagerDummy;
            }
        }
    } else { // range join

        if (range.empty()){
            range = "0.0";
        }

        string idx = getDictionary()->getIndexFile(tableName2, attribName2, metricName);
        if (idx == "NULL") {
            getParser()->addError("The attribute " + tableName2 + "." + attribName2 + " not associated with " + metricName);
        } else {
            // opening The join tree
            IndexManager *joinManagerDummy = new IndexManager(getDictionary()->getDatabaseManager()->getUsername());
            DummyTree *joinDummyTree = joinManagerDummy->openDummyTree(idx, dc);

            // setting The weights
            //SetWeights(joinDummyTree->GetMetricEvaluator(), tablename2, attribname2, metricName);

            // computing join
            //TDateTime tmp1 = Now();
            if (lexical.toUpper(predicate) == "NEAR"){
                result = dummyTree->RangeJoinQuery(joinDummyTree , atof(range.c_str()));
            } else {
                getParser()->addError("The FAR predicate needs implementation!");
            }
            //TDateTime tmp2 = Now();
            //MetricTreeTotalTime += tmp2 - tmp1;

            //Unloading the joined tree
            delete joinManagerDummy;
        }
    }

    if (hasError()){
        return activeListTail;
    }

    //Inserting the tmpjoin table before the token "where"
    int newTokenPosition = 0;
    vector<Token> auxNewTokenList;
    for (int x = activeListTail.size()-1; x >= 0; x--){
        if (lexical.toUpper(activeListTail[x].getLexem()) == "WHERE")
            newTokenPosition = x;
    }

    char buf[8];
    sprintf(buf, "%d", *tempJoinTableScope);
    string tmpBuf = buf;
    string tmpTable = "TMP$tmpjoin" + tmpBuf;
    *tempJoinTableScope++;

    newTokenPosition--;
    for (int x = 0; x < newTokenPosition; x++)
        auxNewTokenList.push_back(activeListTail[x]);

    Token *tk;
    tk = new Token(",", Token::TK_COMMA, Lexical::LK_UNDEFINED);
    auxNewTokenList.push_back(*tk);
    tk = new Token(tmpTable, Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
    auxNewTokenList.push_back(*tk);

    //Insert The join conditions in The where clause
    tk = new Token("=", Token::TK_EQUAL, Lexical::LK_UNDEFINED);
    auxNewTokenList.push_back(*tk);
    tk = new Token(tmpTable, Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
    auxNewTokenList.push_back(*tk);
    tk = new Token(".", Token::TK_PERIOD, Lexical::LK_UNDEFINED);
    auxNewTokenList.push_back(*tk);
    tk = new Token("col1", Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
    auxNewTokenList.push_back(*tk);
    tk = new Token("and", Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
    auxNewTokenList.push_back(*tk);
    tk = new Token(tableName2, Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
    auxNewTokenList.push_back(*tk);
    tk = new Token(".", Token::TK_PERIOD, Lexical::LK_UNDEFINED);
    auxNewTokenList.push_back(*tk);
    tk = new Token(attribName2, Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
    auxNewTokenList.push_back(*tk);
    tk = new Token("=", Token::TK_EQUAL, Lexical::LK_UNDEFINED);
    auxNewTokenList.push_back(*tk);
    tk = new Token(tmpTable, Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
    auxNewTokenList.push_back(*tk);
    tk = new Token(".", Token::TK_PERIOD, Lexical::LK_UNDEFINED);
    auxNewTokenList.push_back(*tk);
    tk = new Token("col2", Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
    auxNewTokenList.push_back(*tk);

    for (int x = newTokenPosition; x < activeListTail.size(); x++)
        auxNewTokenList.push_back(activeListTail[x]);

    *activeList = auxNewTokenList.size()-1;

    activeListTail.clear();
    activeListTail = auxNewTokenList;

    //Updates the current token list into active scope
    //Current token list = internal parser token list
    getParser()->setTokenList(activeListTail, getParser()->getActiveScope());

    //Deleting all records on TMP$tmpjoinX
    getDictionary()->getDatabaseManager()->runSelect("DELETE FROM " + tmpTable);

    //Insert the join result into table TMP$tmpjoin
    //TDateTime tmp1 = Now();
    for (int i = 0; i < result->GetNumOfEntries(); i++){
        string query = "INSERT INTO " + tmpTable + " VALUES (";
        BasicArrayObject<double> *tmp1;
        tmp1 = (BasicArrayObject<double> *)  ((*result)[i]->GetObject());
        BasicArrayObject<double> *tmp2 = (BasicArrayObject<double> *)(*result)[i]->GetJoinedObject();
        sprintf(buf, "%d", tmp1->GetOID());
        tmpBuf = buf;
        query += tmpBuf;
        sprintf(buf, "%d", tmp2->GetOID());
        query += tmpBuf;
        query += " )";
        getDictionary()->getDatabaseManager()->runSelect(query);
    }
    //TDateTime tmp2 = Now();
    //TempJoinInsertsTotalTime += tmp2 - tmp1;

    // clean up memory
    delete result;

    return activeListTail;
}
