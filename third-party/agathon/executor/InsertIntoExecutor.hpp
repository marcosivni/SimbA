#ifndef INSERTINTOEXECUTOR_HPP
#define INSERTINTOEXECUTOR_HPP

#include <RegularSQLExecutor.hpp>
//Manipulate Complex Objects
#include <ComplexObject.hpp>
#include <SirenExtractors.hpp>
#include <IndexManager.hpp>
#include <BasicArrayObject.hpp>


class InsertIntoExecutor : public RegularSQLExecutor{

    private:
        DataDictionary *dic;
        vector<string> columns;
        vector<string> values;
        string tableName;

    private:
        bool checkIfColumnExists(vector<string> columns, string key);

    private:
        DataDictionary *dictionary();

        string additionalInsertIntoStatement(string table, string column, string dataType, string key, string complexDataPath);
        string originalInsertIntoStatement();

        void clear();
        void load_all_columns();
        void insert_column_list();
        void insert_value_list();

    public:
        InsertIntoExecutor(string sql, DatabaseManager *dbmg) : RegularSQLExecutor(sql, dbmg){
            dic = new DataDictionary(dbmg);
        }
        ~InsertIntoExecutor();

        vector<string> translate();
        bool isExecutable();
};

#endif // INSERTINTOEXECUTOR_HPP
