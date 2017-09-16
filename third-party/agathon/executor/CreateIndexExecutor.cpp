#include "CreateIndexExecutor.hpp"

CreateIndexExecutor::~CreateIndexExecutor(){

    if (dic != NULL){
        delete (dic);
    }

    for (int x = 0; x < columnName.size(); x++){
        columnName[x].clear();
    }
    columnName.clear();
}

DataDictionary * CreateIndexExecutor::dictionary(){

    return dic;
}

string CreateIndexExecutor::getColumnName(int pos) throw (std::exception*){

    if (pos < columnName.size())
        return columnName[pos];
    else
        throw (new std::exception());
}

void CreateIndexExecutor::column_name(){

    bool first = true;
    while ((currentToken().getLexem() != ")") || (first)){
        if (currentToken().getTokenType() == Token::TK_COMMA){
            getParser()->match(getParser()->getCurrentToken());
        }
        columnName.push_back(currentToken().getLexem());
        first = false;
        getParser()->match(getParser()->getCurrentToken());
    }
}

vector<string> CreateIndexExecutor::translate(){

    string trans;
    vector<string> answer;

    try{
        getParser()->match(getParser()->getCurrentToken());
        getParser()->match(getParser()->getCurrentToken());
        string indexName = currentToken().getLexem();
        getParser()->match(getParser()->getCurrentToken());
        getParser()->match(getParser()->getCurrentToken());
        getParser()->match(getParser()->getCurrentToken());
        getParser()->match(getParser()->getCurrentToken());

        column_name();

        getParser()->match(getParser()->getCurrentToken());

        trans = "CREATE INDEX " + getParser()->getToken(2)->getLexem() + " ON ";
        trans += getParser()->getToken(4)->getLexem() + " (";
        for (int x = 0; x < columnName.size(); x++){
            if (x > 0)
                trans += ",";
            trans += " " + getColumnName(x) + " ";
        }
        trans += ")";

        if (!endOfStatement()){
            trans += " USING ";
            getParser()->match(getParser()->getCurrentToken());
            if (currentToken().toUpperLexem() == "BTREE"){
                trans += "BTREE";
                getParser()->match(getParser()->getCurrentToken());
            } else {
                if (currentToken().toUpperLexem() == "HASH"){
                    trans += "HASH";
                    getParser()->match(getParser()->getCurrentToken());
                } else {
                    trans.clear();
                    trans = "INSERT INTO CDD$ComplexAttribMetric VALUES ( '";
                    trans += getParser()->getToken(4)->getLexem() + "', '";
                    trans += getParser()->getToken(6)->getLexem() + "', ";
                    trans += dictionary()->getMetricCode(currentToken().getLexem()) + ", ";
                    trans += "'N', '";
                    trans += indexName + "', '";
                    trans += indexName + ".dat' )";

                    indexName += ".dat";

                    //Adding feature vectors to the new index
                    string table = getParser()->getToken(4)->getLexem();
                    string column = getParser()->getToken(6)->getLexem();
                    string metricName = currentToken().getLexem();

                    Lexical lex;
                    vector<Signature *> fVectors;
                    string cpTable = dictionary()->getTablePrefix(dictionary()->getColumnDataType(table, column));
                    vector< pair <string, string> > complexDataAndRowId = dictionary()->getComplexObjectAndRowId(cpTable, column);


                    Evaluator<Signature> *eval = new Evaluator<Signature>(dictionary()->getDistanceName(dictionary()->getDistanceFunction(dictionary()->getMetricCode(metricName))), dictionary());
                    vector<string> extractors = dictionary()->getExtractorsName(dictionary()->getMetricCode(metricName));

                    for (int m = 0; m < complexDataAndRowId.size(); m++){

                        Signature *signatureForMetric = new Signature();
                        ComplexObject *complexObject = new ComplexObject(lex.fromBase64(complexDataAndRowId[m].first));

                        for (int y = 0; y < extractors.size(); y++){
                            vector<string> params = dictionary()->getParametersName(dictionary()->getExtractorCode(extractors[y]));
                            for (int z = 0; z < params.size(); z++){
                                SirenExtractors<Signature> *sExtractor = new SirenExtractors<Signature>(dictionary()->getColumnDataType(table, column), extractors[y], dictionary());
                                Signature *aux = sExtractor->extract(complexObject);
                                for (int k = 0; k < aux->getSize(); k++){
                                    double weigth = atof(dictionary()->getWeight(dictionary()->getMetricCode(metricName), params[z]).c_str());
                                    if (weigth == 0)
                                        weigth = 1;
                                    signatureForMetric->set((* (double *) aux->get(k))*weigth);
                                }
                                delete (aux);
                            }
                        }

                        signatureForMetric->setOID(atoi(complexDataAndRowId[m].second.c_str()));
                        fVectors.push_back(signatureForMetric);
                    }

                    IndexManager *idx = new IndexManager(dictionary(), *eval, IndexManager::PAGE_SIZE, indexName);

                    for (int x = 0; x < fVectors.size(); x++){
                        idx->addItem(fVectors[x]);
                        delete (fVectors[x]);
                    }
                    fVectors.clear();

                    delete (eval);
                    delete (idx);
                    //Closing new filled index
                }
            }
        }

        answer.push_back(trans);
        trans.clear();

        getParser()->resetParser();
    } catch (...){
    }

    return answer;
}

bool CreateIndexExecutor::isExecutable(){

    return (translate().size() > 0);
}
