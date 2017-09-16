#include "InsertIntoExecutor.hpp"

InsertIntoExecutor::~InsertIntoExecutor(){

    if (dic != NULL){
        delete (dic);
    }

    clear();
}

DataDictionary * InsertIntoExecutor::dictionary(){

    return dic;
}

bool InsertIntoExecutor::checkIfColumnExists(vector<string> columns, string key){

    for (int x = 0; x < columns.size(); x++){
        if (columns[x] == key){
            return true;
        }
    }
    return false;
}

string InsertIntoExecutor::additionalInsertIntoStatement(string table, string column, string dataType, string key, string complexDataPath){

    try {
        vector<string> sColumns;
        vector<string> sValues;
        Lexical lex;
        vector< pair<string, string> > metrics = dictionary()->getMetrics(table, column);
        vector<Signature *> fVectors;

        string name;
        for (int x = 1; x < complexDataPath.size()-1; x++){
            name += complexDataPath[x];
        }
        ComplexObject *complexObject = new ComplexObject(name);

        //Create insert for original complex data and extracted parameters
        //All as BLOB
        sValues.push_back(key);
        //Stores the integral image or just the name?
        //For now just the name
        sValues.push_back(lex.toBase64(complexObject->stillImage()->getImage()->getFilename()));

        for (int x = 0; x < metrics.size(); x++){

            fVectors.push_back(NULL);

            Evaluator<Signature> *eval = new Evaluator<Signature>(dictionary()->getDistanceName(dictionary()->getDistanceFunction(table, column, dictionary()->getMetricName(metrics[x].first))), dictionary());

            Signature *signatureForMetric = new Signature();

            vector<string> extractors = dictionary()->getExtractorsName(metrics[x].first);

            for (int y = 0; y < extractors.size(); y++){

                vector<string> params = dictionary()->getParametersName(dictionary()->getExtractorCode(extractors[y]));

                for (int z = 0; z < params.size(); z++){

                    SirenExtractors<Signature> *sExtractor = new SirenExtractors<Signature>(dataType, extractors[y], dictionary());

                    Signature *aux = sExtractor->extract(complexObject);

                    for (int k = 0; k < aux->getSize(); k++){
                        double weigth = atof(dictionary()->getWeight(metrics[x].first, params[z]).c_str());
                        signatureForMetric->set((* (double *) aux->get(k))*weigth);
                    }

                    if (!checkIfColumnExists(sColumns, params[z])){
                        sColumns.push_back(params[z]);
                        sValues.push_back(lex.toBase64(aux->serializeToString()));
                    }

                    delete (aux);
                }
            }

            signatureForMetric->setOID(atoi(key.c_str()));
            fVectors[x] = signatureForMetric;

            string indexFileName = dictionary()->getIndexFile(table, column, dictionary()->getMetricName(metrics[x].first));
            IndexManager *idx = new IndexManager(dictionary(), *eval, IndexManager::PAGE_SIZE, indexFileName);

            idx->addItem(fVectors[x]);

            delete (eval);
            delete (idx);
            delete (signatureForMetric);
        }
        fVectors.clear();

        string aux = " ( " + column + "_id, " + column + ", ";
        for (int x = 0; x < sColumns.size(); x++){
            if (x > 0)
                aux += ",";
            aux += " " + sColumns[x] + " ";
        }
        aux += ") VALUES (";


        for (int x = 0; x < sValues.size(); x++){
            if (x > 0)
                aux += ",";
            aux += " '" + sValues[x] + "' ";
        }
        aux += ")";

        for (int x = 0; x < sColumns.size(); x++){
            sColumns[x].clear();
        }
        sColumns.clear();

        for (int x = 0; x < sValues.size(); x++){
            sValues[x].clear();
        }
        sValues.clear();

        delete (complexObject);

        string strSql = "";

        if(dbmg->getSGBD() == DatabaseManager::POSTGRES)
        {
            strSql = "INSERT INTO \"" + dictionary()->getTablePrefix(dataType) + "$" + tableName + "_" + column + "\"" + aux;
        }
        else
        {
            strSql = "INSERT INTO " + dictionary()->getTablePrefix(dataType) + "$" + tableName + "_" + column + aux;
        }

        return strSql;

    } catch (...) {
        throw new std::exception();
    }
}

string InsertIntoExecutor::originalInsertIntoStatement(){

    string aux = " (";
    for (int x = 0; x < columns.size(); x++){
        if (x > 0)
            aux += ",";
        aux += " " + columns[x] + " ";
    }
    aux += ") VALUES (";

    for (int x = 0; x < values.size(); x++){
        if (x > 0)
            aux += ",";
        aux += " " + values[x] + " ";
    }
    aux += ")";

    string strSql = "";

    if(dbmg->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "INSERT INTO \"" + tableName + "\"" + aux;
    }
    else
    {
        strSql = "INSERT INTO " + tableName + aux;
    }

    return strSql;
}

void InsertIntoExecutor::clear(){

    for (int x = 0; x < columns.size(); x++){
        columns[x].clear();
    }
    columns.clear();
    for (int x = 0; x < values.size(); x++){
        values[x].clear();
    }
    values.clear();
    tableName.clear();

}

void InsertIntoExecutor::load_all_columns(){

    vector<string> cols =  dictionary()->getAllColumnsFromTable(tableName);

    for (int x = 0; x < cols.size(); x++){
        columns.push_back(cols[x]);
        cols[x].clear();
    }
    cols.clear();
}


void InsertIntoExecutor::insert_column_list(){

    while (currentToken().getLexem() != ")"){
        if (currentToken().getTokenType() == Token::TK_COMMA){
            getParser()->match(getParser()->getCurrentToken());
        } else {
            columns.push_back(currentToken().getLexem());
            getParser()->match(getParser()->getCurrentToken());
        }
    }
}

void InsertIntoExecutor::insert_value_list(){

    while (currentToken().getLexem() != ")"){
        if (currentToken().getTokenType() == Token::TK_COMMA){
            getParser()->match(getParser()->getCurrentToken());
        } else {
            values.push_back(currentToken().getLexem());
            getParser()->match(getParser()->getCurrentToken());
        }
    }
}

vector<string> InsertIntoExecutor::translate(){

    vector<string> answer;
    try{
        getParser()->match(getParser()->getCurrentToken());
        getParser()->match(getParser()->getCurrentToken());
        tableName = currentToken().getLexem();
        getParser()->match(getParser()->getCurrentToken());
        if (currentToken().getLexem() == "("){
            getParser()->match(getParser()->getCurrentToken());
            insert_column_list();
            getParser()->match(")", Token::TK_CLOSE_BRACE);
        } else {
            load_all_columns();
        }
        getParser()->match(getParser()->getCurrentToken());
        getParser()->match("(", Token::TK_OPEN_BRACE);
        insert_value_list();
        getParser()->match(")", Token::TK_CLOSE_BRACE);

        vector<string> complexTypes = dictionary()->getAllValidDataTypes();
        bool isComplex;
        for (int x = 0; x < columns.size(); x++){

            string dataType = dictionary()->getColumnDataType(tableName, columns[x]);
            isComplex = false;

            for (int y = 0; y < complexTypes.size(); y++){
                if (complexTypes[y] == dataType){
                    isComplex = true;
                }
            }

            if (isComplex){
                int cid = dictionary()->runSequence(dictionary()->getTablePrefix(dataType));
                char last[21];
                sprintf(last, "%d", cid);
                string tmp = last;
                answer.push_back(additionalInsertIntoStatement(tableName, columns[x], dataType, tmp, values[x]));
                //Foreign-key is defined right now!
                values[x] = tmp;
            }
        }

        //Putting the original insert back
        //answer.push_back(originalInsertIntoStatement());
        // @loc: primeira insercao na tabela original, devido a fk na tabela $
        answer.insert(answer.begin(), originalInsertIntoStatement());

        //Restarting
        getParser()->resetParser();
        clear();
    } catch (...){
    }

    return answer;
}

bool InsertIntoExecutor::isExecutable(){

    return translate().size() > 0;
}
