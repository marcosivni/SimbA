#ifndef COMPLEXSQLEXECUTOR_HPP
#define COMPLEXSQLEXECUTOR_HPP

#include <RegularSQLExecutor.hpp>
//incluir select
#include <UpdateExecutor.hpp>
#include <DeleteExecutor.hpp>
#include <InsertIntoExecutor.hpp>
#include <CreateTableExecutor.hpp>
#include <CreateDatabaseExecutor.hpp>
#include <CreateIndexExecutor.hpp>
#include <CreateMetricExecutor.hpp>
#include <AlterTableExecutor.hpp>
#include <DropIndexExecutor.hpp>
#include <DropMetricExecutor.hpp>
#include <DropTableExecutor.hpp>
#include <DropDatabaseExecutor.hpp>
#include <ShowExecutor.hpp>
#include <DescExecutor.hpp>
#include <UseExecutor.hpp>
#include <SelectExecutor.hpp>

#include <SyntacticComplexSQLAnalyzer.hpp>

class ComplexSQLExecutor : public RegularSQLExecutor{

    private:
        UpdateExecutor* update;
        DeleteExecutor* deletee;
        InsertIntoExecutor* insert;
        CreateTableExecutor* createTable;
        CreateDatabaseExecutor* createDatabase;
        CreateIndexExecutor* createIndex;
        CreateMetricExecutor* createMetric;
        AlterTableExecutor* alter;
        DropIndexExecutor* dropIndex;
        DropMetricExecutor* dropMetric;
        DropTableExecutor* dropTable;
        DropDatabaseExecutor* dropDatabase;
        ShowExecutor* show;
        DescExecutor* desc;
        UseExecutor *use;
        SelectExecutor* sel;
        SyntacticComplexSQLAnalyzer *syn;

    public:
        ComplexSQLExecutor(string sql, DatabaseManager *dbmg) : RegularSQLExecutor(sql, dbmg){

            update = new UpdateExecutor(sql, dbmg);
            deletee = new DeleteExecutor(sql, dbmg);
            insert = new InsertIntoExecutor(sql, dbmg);
            createTable = new CreateTableExecutor(sql, dbmg);
            createDatabase = new CreateDatabaseExecutor(sql, dbmg);
            createIndex = new CreateIndexExecutor(sql, dbmg);
            createMetric = new CreateMetricExecutor(sql, dbmg);
            alter = new AlterTableExecutor(sql, dbmg);
            dropIndex = new DropIndexExecutor(sql, dbmg);
            dropMetric = new DropMetricExecutor(sql, dbmg);
            dropTable = new DropTableExecutor(sql, dbmg);
            dropDatabase = new DropDatabaseExecutor(sql, dbmg);
            show = new ShowExecutor(sql, dbmg);
            desc = new DescExecutor(sql, dbmg);
            use = new UseExecutor(sql, dbmg);
            syn = new SyntacticComplexSQLAnalyzer(sql, dbmg);
            sel = new SelectExecutor(sql, dbmg);
        }
        ~ComplexSQLExecutor();

        vector<string> translate();
        void setSQLExpression(string sqlExpression);
        bool isExecutable();
};

#endif // COMPLEXSQLEXECUTOR_HPP
