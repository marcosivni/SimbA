#ifndef CREATEMETRICEXECUTOR_HPP
#define CREATEMETRICEXECUTOR_HPP

#include <RegularSQLExecutor.hpp>
#include <CreateMetricSemanticStructs.hpp>

class CreateMetricExecutor : public RegularSQLExecutor{

    private:
        DataDictionary *dic;
        CheckExtractor *checkExtractors;

    private:
        void extractor_name();
        void parameter_reference();
        void association();
        void associations_list();

        DataDictionary *dictionary();
        string additionalInsertIntoMetricStruct(string metricName, string dataType, string distanceCode, string metricCode);
        vector<string> additionalInsertIntoParameterStruct(string metricCode);

    public:
        CreateMetricExecutor(string sql, DatabaseManager *dbmg) : RegularSQLExecutor(sql, dbmg){
            dic = new DataDictionary(dbmg);
            checkExtractors = new CheckExtractor();
        }
        ~CreateMetricExecutor();

        vector<string> translate();
        bool isExecutable();
};

#endif // CREATEMETRICEXECUTOR_HPP
