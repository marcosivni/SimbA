#ifndef SYNTACTICCREATEMETRICANALYZER_HPP
#define SYNTACTICCREATEMETRICANALYZER_HPP

#include <SyntacticRegularSQLAnalyzer.hpp>
#include <CreateMetricSemanticStructs.hpp>

class SyntacticCreateMetricAnalyzer : public SyntacticRegularSQLAnalyzer{

    private:
        DataDictionary *dic;
        CheckExtractor *checkExtractors;

    private:
        void metric_name();
        void distance_name();
        void datatype_name();
        void extractor_name();
        void parameter_name();
        void parameter_reference();
        void association();
        void associations_list();
        void create_metric_specification();

        DataDictionary* getDictionary();

    public:
        SyntacticCreateMetricAnalyzer(string sql, DatabaseManager *dbmg) : SyntacticRegularSQLAnalyzer (sql, dbmg){
            dic = new DataDictionary(dbmg);
            checkExtractors = new CheckExtractor();
        }

        ~SyntacticCreateMetricAnalyzer();

        bool isValid();
};

#endif // SYNTACTICCREATEMETRICANALYZER_HPP
