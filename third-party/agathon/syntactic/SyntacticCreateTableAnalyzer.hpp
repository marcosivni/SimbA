#ifndef SYNTACTICCREATETABLEANALYZER_HPP
#define SYNTACTICCREATETABLEANALYZER_HPP

#include <SyntacticRegularSQLAnalyzer.hpp>
#include <CreateTableSemanticStructs.hpp>

class SyntacticCreateTableAnalyzer : public SyntacticRegularSQLAnalyzer{

    private:
        DataDictionary *dic;
        Constraint currentConstraint;
        CheckValidColumnsAndTypes *checkColumns;
        CheckConstraints *checkConstraints;

    private:
        DataDictionary *getDictionary();

        //Methods for syntatic analyzer
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
        void create_table_specification();

    public:
        SyntacticCreateTableAnalyzer(string expression, DatabaseManager *dbmg) : SyntacticRegularSQLAnalyzer(expression, dbmg){
            dic = new DataDictionary(dbmg);
            checkColumns = new CheckValidColumnsAndTypes(dic);
            checkConstraints = new CheckConstraints(dic);
        }

        ~SyntacticCreateTableAnalyzer();

        bool isValid();
};

#endif // SYNTACTICCREATETABLEANALYZER_HPP
