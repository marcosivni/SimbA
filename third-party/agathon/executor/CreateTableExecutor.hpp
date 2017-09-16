#ifndef CREATETABLEEXECUTOR_HPP
#define CREATETABLEEXECUTOR_HPP

#include <RegularSQLExecutor.hpp>
#include <CreateTableSemanticStructs.hpp>

class CreateTableExecutor : public RegularSQLExecutor{

    private:
        DataDictionary *dic;
        Constraint currentConstraint;
        CheckValidColumnsAndTypes *checkColumns;
        CheckConstraints *checkConstraints;

    private:
        DataDictionary *dictionary();

        void table_name();
        void column_definition();
        void column_constraint();
        void constraint_name();
        void column_name_list(int op = 0);
        void column_name(int op = 0);
        void metric_name_list();
        void metric_name();
        void column_constraint_list();
        void data_type();
        void table_element_list();
        void table_element();
        void table_constraint();

        string originalCreateTable();
        string additionalCreateTable(string table, string column, string dataType, Constraint metric);
        string additionalInsertIntoComplexAttribute(string column, string table, string dataType);
        vector<string> additionalInsertIntoComplexAttibMetric(Constraint constraint, string column, string table);

    public:
        CreateTableExecutor(string sql, DatabaseManager *dbmg) : RegularSQLExecutor(sql, dbmg){
            dic = new DataDictionary(dbmg);
            checkColumns = new CheckValidColumnsAndTypes(dic);
            checkConstraints = new CheckConstraints(dic);
        }
        ~CreateTableExecutor();

        vector<string> translate();
        bool isExecutable();
};

#endif // CREATETABLEEXECUTOR_HPP
