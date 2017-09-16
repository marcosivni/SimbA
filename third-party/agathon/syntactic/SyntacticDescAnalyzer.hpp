#ifndef SYNTACTICDESCANALYZER_HPP
#define SYNTACTICDESCANALYZER_HPP

#include <SyntacticRegularSQLAnalyzer.hpp>

class SyntacticDescAnalyzer: public SyntacticRegularSQLAnalyzer{

    private:
        DataDictionary *dic;

    private:
        void table_name();
        void metric_name();
        void desc_specification();

        DataDictionary *dictionary();

    public:
        SyntacticDescAnalyzer(string sql, DatabaseManager *dbmg):SyntacticRegularSQLAnalyzer (sql, dbmg){
            dic = new DataDictionary(dbmg);
        }

        bool isValid();

        ~SyntacticDescAnalyzer();
};

#endif // SYNTACTICDESCANALYZER_HPP
