#ifndef DROPDATABASEEXECUTOR_HPP
#define DROPDATABASEEXECUTOR_HPP

#include <RegularSQLExecutor.hpp>

class DropDatabaseExecutor : public RegularSQLExecutor{

    public:
        DropDatabaseExecutor(string sql, DatabaseManager *dbmg) : RegularSQLExecutor(sql, dbmg){
        }

        vector<string> translate();
        bool isExecutable();
};

#endif // DROPDATABASEEXECUTOR_HPP
