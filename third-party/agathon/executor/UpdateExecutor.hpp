#ifndef UPDATEEXECUTOR_HPP
#define UPDATEEXECUTOR_HPP

#include <RegularSQLExecutor.hpp>

class UpdateExecutor : public RegularSQLExecutor{

    public:
        UpdateExecutor(string sql, DatabaseManager *dbmg) : RegularSQLExecutor(sql, dbmg){
        }

        vector<string> translate();
        bool isExecutable();
};

#endif // UPDATEEXECUTOR_HPP
