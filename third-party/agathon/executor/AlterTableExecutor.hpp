#ifndef ALTERTABLEEXECUTOR_HPP
#define ALTERTABLEEXECUTOR_HPP

#include <RegularSQLExecutor.hpp>

class AlterTableExecutor : public RegularSQLExecutor{

    private:
        DataDictionary *dic;
        string tableName;

    private:
        DataDictionary* dictionary();

    public:
        AlterTableExecutor(string sql, DatabaseManager *dbmg) : RegularSQLExecutor(sql, dbmg){
             dic = new DataDictionary(dbmg);
        }
        ~AlterTableExecutor();

        vector<string> translate();
        bool isExecutable();
};

#endif // ALTERTABLEEXECUTOR_HPP
