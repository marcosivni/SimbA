#ifndef SYNTACTICUPDATEANALYZER_HPP
#define SYNTACTICUPDATEANALYZER_HPP

#include<SyntacticRegularSQLAnalyzer.hpp>

class SyntacticUpdateAnalyzer : public SyntacticRegularSQLAnalyzer{

    private:
        DataDictionary *dic;
        string tableName;

    private:
        void table_name();
        void column_name();
        void value_name();
        void update_reference();
        void update_list();
        void where_clause();
        void column_where_list();
        void column_where();
        void update_specification();

        DataDictionary* dictionary();
        string getTableName();

    public:
        SyntacticUpdateAnalyzer(string sql, DatabaseManager *dbmg) : SyntacticRegularSQLAnalyzer (sql, dbmg){

            dic = new DataDictionary(dbmg);
        }

        ~SyntacticUpdateAnalyzer();

        bool isValid();
};

#endif // SYNTACTICUPDATEANALYZER_HPP
