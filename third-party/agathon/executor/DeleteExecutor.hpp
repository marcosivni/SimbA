#ifndef DELETEEXECUTOR_HPP
#define DELETEEXECUTOR_HPP

#include <RegularSQLExecutor.hpp>
#include <IndexManager.hpp>

class DeleteExecutor : public RegularSQLExecutor{

    private:
        DataDictionary *dic;

    private:
        DataDictionary *dictionary();

    public:
        DeleteExecutor(string sql, DatabaseManager *dbmg) : RegularSQLExecutor(sql, dbmg){
            dic = new DataDictionary(dbmg);
        }
        ~DeleteExecutor();

        vector<string> translate();
        bool isExecutable();
};

#endif // DELETEEXECUTOR_HPP
