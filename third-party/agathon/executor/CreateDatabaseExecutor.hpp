#ifndef CREATEDATABASEEXECUTOR_HPP
#define CREATEDATABASEEXECUTOR_HPP

#include <RegularSQLExecutor.hpp>

class CreateDatabaseExecutor : public RegularSQLExecutor{

    private:
        DataDictionary *dic;

    public:
        CreateDatabaseExecutor(string sql, DatabaseManager *dbmg) : RegularSQLExecutor(sql, dbmg){
            dic = new DataDictionary(dbmg);
        }

        ~CreateDatabaseExecutor();

        vector<string> translate();
        bool isExecutable();
};

#endif // CREATEDATABASEEXECUTOR_HPP
