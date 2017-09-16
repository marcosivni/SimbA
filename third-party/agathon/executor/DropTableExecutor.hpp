#ifndef DROPTABLEEXECUTOR_HPP
#define DROPTABLEEXECUTOR_HPP

#include <RegularSQLExecutor.hpp>

class DropTableExecutor : public RegularSQLExecutor{

    private:
        DataDictionary *dic;

    private:
        DataDictionary *dictionary();

    public:
        DropTableExecutor(string sql, DatabaseManager *dbmg) : RegularSQLExecutor(sql, dbmg){
            dic = new DataDictionary(dbmg);
        }
        ~DropTableExecutor();

        vector<string> translate();
        bool isExecutable();
};


#endif // DROPTABLEEXECUTOR_HPP
