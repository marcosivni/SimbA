#ifndef SYNTACTICINSERTINTOANALYZER_HPP
#define SYNTACTICINSERTINTOANALYZER_HPP

#include <SyntacticRegularSQLAnalyzer.hpp>
#include <fstream>
#include <ComplexObject.hpp>

using namespace std;

class SyntacticInsertIntoAnalyzer : public SyntacticRegularSQLAnalyzer{

    private:
        DataDictionary *dic;
        string tableName;
        vector<string> columns;
        vector<string> values;

    private:
        //Syntactic Methods
        void table_name();
        void insert_column();
        void insert_column_list();
        void insert_value();
        void insert_value_list();
        void load_all_columns();
        void insert_specification();

        //Semantic Methods
        string getTableName();
        void checkColumnsAndValues();

        DataDictionary *dictionary();

    public:
        SyntacticInsertIntoAnalyzer(string sql, DatabaseManager *dbmg) : SyntacticRegularSQLAnalyzer (sql, dbmg){

            dic = new DataDictionary(dbmg);
        }

        ~SyntacticInsertIntoAnalyzer();

        bool isValid();
};

#endif // SYNTACTICINSERTINTOANALYZER_HPP
