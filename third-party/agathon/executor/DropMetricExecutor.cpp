#include "DropMetricExecutor.hpp"

DropMetricExecutor::~DropMetricExecutor(){

    if (dic != NULL){
        delete (dic);
    }
}

DataDictionary* DropMetricExecutor::dictionary(){

    return dic;
}

vector<string> DropMetricExecutor::translate(){

    vector<string> answer;
    string metricName = getParser()->getToken(2)->getLexem();

    answer.push_back("DELETE FROM CDD$MetricStruct WHERE MetricCode = " + dictionary()->getMetricCode(metricName));
    answer.push_back("DELETE FROM CDD$ParameterStruct WHERE MetricCode = " + dictionary()->getMetricCode(metricName));
    answer.push_back("DELETE FROM CDD$ComplexAttribMetric WHERE MetricCode = " + dictionary()->getMetricCode(metricName));

    vector< pair < string, string > > tablesAndColumns = dictionary()->getAllTableNameComplexColumn(dictionary()->getMetricCode(metricName));

    for (int x = 0; x < tablesAndColumns.size(); x++){

        //Remove complex index file for metric
        string indexFile = dictionary()->getIndexFile(tablesAndColumns[x].first, tablesAndColumns[x].second, metricName);
        if (!(indexFile == "NULL")){
            remove(("siren_slim_" + indexFile).c_str());
            remove(("siren_dummy_" + indexFile).c_str());
        }

        vector< pair<string, string> > params = dictionary()->getParameters(dictionary()->getMetricCode(metricName));
        for (int k = 0; k < params.size(); k++){
            answer.push_back("ALTER TABLE " + dictionary()->getTablePrefix(dictionary()->getComplexAttributeReference(tablesAndColumns[x].first, tablesAndColumns[x].second)) + "$" + tablesAndColumns[x].first + "_" + tablesAndColumns[x].second + " DROP COLUMN " + params[k].first);
        }

        if (dictionary()->getMetrics(tablesAndColumns[x].first, tablesAndColumns[x].second).size() == 1){
            answer.push_back("DELETE FROM CDD$ComplexAttribute WHERE TableName = '" + tablesAndColumns[x].first + "' AND ComplexAttribName='" + tablesAndColumns[x].second + "'");
            answer.push_back("DROP TABLE " + dictionary()->getTablePrefix(dictionary()->getComplexAttributeReference(tablesAndColumns[x].first, tablesAndColumns[x].second)) + "$" + tablesAndColumns[x].first + "_" + tablesAndColumns[x].second);
            answer.push_back("ALTER TABLE " + tablesAndColumns[x].first + " DROP COLUMN " + tablesAndColumns[x].second);
        }

        tablesAndColumns[x].first.clear();
        tablesAndColumns[x].second.clear();
    }
    tablesAndColumns.clear();

    metricName.clear();

    return answer;
}

bool DropMetricExecutor::isExecutable(){

    return (translate().size() > 0);
}

