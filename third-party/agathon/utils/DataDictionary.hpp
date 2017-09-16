#ifndef DATADICTIONARY_HPP
#define DATADICTIONARY_HPP

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>
#include <DatabaseManager.hpp>

using namespace std;

class DataDictionary{

    private:
        DatabaseManager *dbmg;

    public:
        DataDictionary(DatabaseManager *dbmg);
        ~DataDictionary();

        string unSlugify(string text);

        bool isMetrical(string distanceCode);
        bool isPrimaryKey(string tableName, string columnName);
        bool isValidPrimaryKey(string value, string tableName, string columnName);
        bool isValidDatabase(string databaseName);
        bool isValidDataType(string dataType);
        bool isValidComplexDataType(string dataType);
        bool isValidIndex(string tableName, string indexName);
        bool isValidTable(string tableName);
        bool isValidColumn(string tableName, string columnName);
        bool isValidColumn(vector<string> tableName, vector<string> columnName);
        bool isDistanceFunctionName(string distanceName);
        bool isExtractorName(string extractorName);
        bool isValidExtractorForThisType(string dataType, string extractorName);
        bool isExtractorParameter (int extractorCode, string parameterName);
        bool isMetric(string metricName);
        bool isComplexAttributeReference(string tableName, string attributeName);
        bool isComplexAttributeReference(string attributeName);
        bool isValidMetric(string metricName, string tableName, string attributeName);
        bool isComplexAttributeIndex(string caIndex);
        bool isComplexAttributeIndex(string tableName, string caIndex);
        bool isStillimageAttributeReference(string attribName);
        bool isStillimageAttributeReference(string tableName, string imgAttribName);
        bool isParticulateMetric(string metricName);
        bool isComplexAttribMetricTuple(string tableName, string caAttribName, string metricCode);
        int runSequence(string sequenceName);

        string toUpper(string text);
        string useStatement(string databaseName);
        string dropIndexStatement(string table, string indexName);
        string getNewIndexId();
        string getNewMetricId();
        vector<string> getComplexReferenceId(string table, string column, string whereClause = "");
        string getTablePrefix(string complexData);
        string getColumnDataType(string tableName, string columnName);
        string showTables();
        string showDatabases();
        string getAllTraditionalIndexesStatement();
        string descMetricCommand(string metricName);
        string descTableCommand(string tableName);
        string getDistanceName(string distanceCode);
        vector<string> getParametersName(string extractorCode);
        vector<string> getExtractorsName(string metricCode);
        vector<string> getAllColumnsFromTable(string tableName);
        vector< pair <string, string> > getAllTableNameComplexColumn(string metricCode);
        vector< pair <string, bool> > getColumnsAndNullableFromTable(string tableName);
        vector<string> getAllValidDataTypes();
        vector< pair <string, string> > getComplexObjectAndRowId(string tableName, string column);
        string getDefaultDistanceFunctionCode();
        string getDistanceFunctionCode(string distanceName);
        string getComplexDataTypeCode(string dataType);
        string getExtractorCode(string extractorName);
        string getMetricCode(string metricName);
        string getComplexAttributeReference(string tableName, string attributeName);
        string getDbmsDataType(string attribType);
        string getDefaultMetric(string tableName, string attribName);
        string getMetricName(string metricCode);
        string getMetricType(string metricCode);
        vector< pair<string, string> > getMetrics(string tableName, string attribName);
        string getIndexFile(string tableName, string indexName);
        string getIndexFile(string tableName, string attribName, string metricName);
        vector<string> getStillimageAttribute(string tableName);
        vector<string> getComplexAttributes(string tableName);
        vector<string> getAudioAttribute(string tableName);
        vector<string> getParticulateAttribute(string tableName);
        string getDistanceFunction(string metricCode);
        string getDistanceFunction(string tableName, string attribName, string metricName);
        string getParameterType(string parameterName, string extractorCode);
        vector< pair<string, string> > getParameters(string metricCode);
        string getParticleType(string tableName, string particleColName);
        vector<string> getParametersNameForAlterPVTable(string metric, string metricCode);
        int getNumberOfParticulateParams(string tableName, string attribName, string metricName);
        vector<string> getParticulateMetricParamNames(string tableName, string metricName, string particulateName);
        vector<string> getParticulateMetricParamInstances(string tableName, string metricName, string particulateName);
        vector<string> getParticulateMetricParamTypes(string tableName, string metricName, string particulateName);
        vector< vector<string> > getParameterNameAndParameterInstance(string tableName, string metricName, string particulateName);
        vector< vector<string> > descTable(string tableName);
        vector<string> getParameterName(string tableName, string metricName);
        vector<string> getParameterInstance(string tableName);
        vector< vector<string> > getTableNameCompAttNameMetricCodeIdxFile(string indexName);
        string getWeight(string metricCode, string parameterName);
        pair<string, string> getExtractorNameParameterName(string tableName, string complexAttribName, string metricName);
        vector<string> getFieldsName(string query);
        vector<string> dataDictionaryInDDLDefinition();
        int getNumberOfPossibleTuples(string query);
        vector< vector<string> > runSubQuery(string subQuery);
        string getValidMetric(string metricName, string tableName, string attributeName);
        string getClientFolder();
        string getExtractorParameter(int extractorCode, string parameterName);
        string getEntireBlob(string queryId);
        int getDBMS();

        void setDatabaseManager(DatabaseManager *dbmg);
        DatabaseManager* getDatabaseManager();
};

#endif // DATADICTIONARY_HPP
