#include "DropTableExecutor.hpp"

DropTableExecutor::~DropTableExecutor(){

    if (dic != NULL){
        delete (dic);
    }
}

DataDictionary * DropTableExecutor::dictionary(){

    return dic;
}

vector<string> DropTableExecutor::translate(){

    vector<string> answer;

    try{
        vector<string> complexColumns = dictionary()->getComplexAttributes(getParser()->getToken(2)->getLexem());
        answer.push_back("DELETE FROM CDD$ComplexAttribute WHERE TableName = '" + getParser()->getToken(2)->getLexem() + "'");
        answer.push_back("DELETE FROM CDD$ComplexAttribMetric WHERE TableName = '" + getParser()->getToken(2)->getLexem() + "'");
        for (int x = 0; x < complexColumns.size(); x++){
            vector< pair <string, string> > metrics = dictionary()->getMetrics(getParser()->getToken(2)->getLexem(), complexColumns[x]);

            //Removing all indexes entrances
            for (int k = 0; k < metrics.size(); k++){
                string indexFile = dictionary()->getIndexFile(getParser()->getToken(2)->getLexem(), complexColumns[x], dictionary()->getMetricName(metrics[k].first));
                if (!(indexFile == "NULL")){
                    remove(("siren_slim_" + indexFile).c_str());
                    remove(("siren_dummy_" + indexFile).c_str());
                }
            }

            answer.push_back("DROP TABLE " + dictionary()->getTablePrefix(dictionary()->getColumnDataType(getParser()->getToken(2)->getLexem(), complexColumns[x])) + "$" + getParser()->getToken(2)->getLexem() + "_" + complexColumns[x]);
            complexColumns[x].clear();
        }
        complexColumns.clear();
        answer.push_back("DROP TABLE " + getParser()->getToken(2)->getLexem());
    } catch (...) {
    }

    getParser()->resetParser();

    return answer;
}

bool DropTableExecutor::isExecutable(){

    return (translate().size() > 0);
}

