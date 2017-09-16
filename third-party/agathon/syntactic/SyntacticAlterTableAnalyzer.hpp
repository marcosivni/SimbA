#ifndef SYNTACTICALTERTABLEANALYZER_HPP
#define SYNTACTICALTERTABLEANALYZER_HPP

#include <SyntacticRegularSQLAnalyzer.hpp>

class SyntacticAlterTableAnalyzer : public SyntacticRegularSQLAnalyzer{

    private:
        DataDictionary *dic;
        string tableName;

    private:
        void table_name();
        void column_name();
        void value_name(int op = 0);
        void command_name();
        void alter_table_specification();

        DataDictionary *dictionary();
        string getTableName();

    public:
        SyntacticAlterTableAnalyzer(string sql, DatabaseManager *dbmg):SyntacticRegularSQLAnalyzer (sql, dbmg){
            dic = new DataDictionary(dbmg);
        }

        ~SyntacticAlterTableAnalyzer();

        bool isValid();
};

#endif // SYNTACTICALTERTABLEANALYZER_HPP
