#ifndef SYNTACTICDROPMETRICANALYZER_HPP
#define SYNTACTICDROPMETRICANALYZER_HPP

#include <SyntacticRegularSQLAnalyzer.hpp>

class SyntacticDropMetricAnalyzer : public SyntacticRegularSQLAnalyzer{

    private:
        DataDictionary* dic;

    private:
        void metric_name();
        void drop_metric_specification();

        DataDictionary *dictionary();

    public:
        SyntacticDropMetricAnalyzer(string sql, DatabaseManager *dbmg):SyntacticRegularSQLAnalyzer (sql, dbmg){
            dic = new DataDictionary(dbmg);
        }

        bool isValid();

        ~SyntacticDropMetricAnalyzer();
};

#endif // SYNTACTICDROPMETRICANALYZER_HPP
