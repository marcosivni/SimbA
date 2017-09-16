#ifndef SYNTACTICDROPTABLEANALYZER_HPP
#define SYNTACTICDROPTABLEANALYZER_HPP

#include <SyntacticRegularSQLAnalyzer.hpp>

class SyntacticDropTableAnalyzer : public SyntacticRegularSQLAnalyzer{

    private:
        DataDictionary* dic;

    private:
        void table_name();
        void drop_table_specification();

        DataDictionary *dictionary();

    public:
        SyntacticDropTableAnalyzer(string sql, DatabaseManager *dbmg):SyntacticRegularSQLAnalyzer (sql, dbmg){
            dic = new DataDictionary(dbmg);
        }

        bool isValid();

        ~SyntacticDropTableAnalyzer();
};

#endif // SYNTACTICDROPTABLEANALYZER_HPP
