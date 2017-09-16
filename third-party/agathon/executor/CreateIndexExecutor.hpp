#ifndef CREATEINDEXEXECUTOR_HPP
#define CREATEINDEXEXECUTOR_HPP

#include <RegularSQLExecutor.hpp>
#include <IndexManager.hpp>
#include <SirenExtractors.hpp>
#include <ComplexObject.hpp>

class CreateIndexExecutor : public RegularSQLExecutor{

    private:
        DataDictionary *dic;
        vector<string> columnName;

    private:
        void column_name();

        DataDictionary *dictionary();
        string getColumnName(int pos) throw (std::exception*);

    public:
        CreateIndexExecutor(string sql, DatabaseManager *dbmg) : RegularSQLExecutor(sql, dbmg){
            dic = new DataDictionary(dbmg);
        }
        ~CreateIndexExecutor();

        vector<string> translate();
        bool isExecutable();
};


#endif // CREATEINDEXEXECUTOR_HPP
