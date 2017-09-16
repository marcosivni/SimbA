#include "DeleteExecutor.hpp"

DeleteExecutor::~DeleteExecutor(){

    if (dic != NULL){
        delete (dic);
    }
}

DataDictionary * DeleteExecutor::dictionary(){

    return dic;
}

//@todo Should remove from index also!!
vector<string> DeleteExecutor::translate(){

    vector<string> answer;
    string aux;

    try{

        getParser()->match(getParser()->getCurrentToken());//DELETE
        getParser()->match(getParser()->getCurrentToken());//FROM
        string tableName = currentToken().getLexem();
        getParser()->match(getParser()->getCurrentToken());//tb name

        vector<string> cps = dictionary()->getComplexAttributes(tableName);

        string whereClause;

        if (!endOfStatement()){
           getParser()->match(getParser()->getCurrentToken());
           while (!endOfStatement()){
               whereClause += " " + currentToken().getLexem();
               getParser()->match(getParser()->getCurrentToken());
           }
        }

        for (int x = 0; x < cps.size(); x++){

            vector<string> ids = dictionary()->getComplexReferenceId(tableName, cps[x], whereClause);
            vector< pair<string, string> > metrics = dictionary()->getMetrics(tableName, cps[x]);

            if (ids.size() > 0){
                aux = "DELETE FROM ";
                aux += dictionary()->getTablePrefix(dictionary()->getComplexAttributeReference(tableName, cps[x]));
                aux += "$" + tableName + "_" + cps[x];

                aux += " WHERE " + cps[x] + "_id IN ( ";
                for (int y = 0; y < ids.size(); y++){
                    if (y > 0)
                        aux += ", ";
                    aux += ids[y] + " ";
                }

                aux += ")";
                answer.push_back(aux);

                //Removing from indexes
                for (int k = 0; k < metrics.size(); k++){
                    Evaluator<Signature> eval(dictionary()->getDistanceName(dictionary()->getDistanceFunction(metrics[k].first)), dictionary());
                    IndexManager *idx = new IndexManager(dictionary(), eval, IndexManager::PAGE_SIZE, dictionary()->getIndexFile(tableName, cps[x], dictionary()->getMetricName(metrics[k].first)));

                    for (int y = 0; y < ids.size(); y++){
                        idx->removeItem(atoi(ids[y].c_str()));
                    }

                    delete (idx);
                }
                //
            }
        }

        aux = "DELETE FROM " + tableName;
        if (!whereClause.empty()){
            aux += " WHERE " + whereClause;
        }
        answer.push_back(aux);
        aux.clear();
    } catch (...){
    }

    getParser()->resetParser();

    return answer;
}

bool DeleteExecutor::isExecutable(){

    return (translate().size() > 0);
}

