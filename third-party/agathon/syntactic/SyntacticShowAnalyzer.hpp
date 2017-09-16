#ifndef SYNTACTICSHOWANALYZER_HPP
#define SYNTACTICSHOWANALYZER_HPP

#include <SyntacticRegularSQLAnalyzer.hpp>

class SyntacticShowAnalyzer : public SyntacticRegularSQLAnalyzer{

    private:
        DataDictionary *dic;

    private:
        void show_specification();

        DataDictionary *dictionary();

    public:
        SyntacticShowAnalyzer(string sql, DatabaseManager *dbmg):SyntacticRegularSQLAnalyzer (sql, dbmg){
            dic = new DataDictionary(dbmg);
        }

        bool isValid();

        ~SyntacticShowAnalyzer();
};

#endif // SYNTACTICSHOWANALYZER_HPP
