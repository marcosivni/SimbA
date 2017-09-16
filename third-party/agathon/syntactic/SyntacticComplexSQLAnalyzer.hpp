#ifndef SYNTACTICCOMPLEXSQLANALYZER_HPP
#define SYNTACTICCOMPLEXSQLANALYZER_HPP

#include <SyntacticRegularSQLAnalyzer.hpp>
#include <SyntacticSelectAnalyzer.hpp>
#include <SyntacticUpdateAnalyzer.hpp>
#include <SyntacticDeleteAnalyzer.hpp>
#include <SyntacticInsertIntoAnalyzer.hpp>
#include <SyntacticCreateTableAnalyzer.hpp>
#include <SyntacticCreateDatabaseAnalyzer.hpp>
#include <SyntacticCreateIndexAnalyzer.hpp>
#include <SyntacticCreateMetricAnalyzer.hpp>
#include <SyntacticAlterTableAnalyzer.hpp>
#include <SyntacticDropIndexAnalyzer.hpp>
#include <SyntacticDropMetricAnalyzer.hpp>
#include <SyntacticDropTableAnalyzer.hpp>
#include <SyntacticDropDatabaseAnalyzer.hpp>
#include <SyntacticShowAnalyzer.hpp>
#include <SyntacticDescAnalyzer.hpp>
#include <SyntacticUse.hpp>

class SyntacticComplexSQLAnalyzer : public SyntacticRegularSQLAnalyzer{

    private:
        SyntacticSelectAnalyzer *select;
        SyntacticUpdateAnalyzer *update;
        SyntacticDeleteAnalyzer *deletee;
        SyntacticInsertIntoAnalyzer *insert;
        SyntacticCreateTableAnalyzer *createTable;
        SyntacticCreateDatabaseAnalyzer *createDatabase;
        SyntacticCreateIndexAnalyzer *createIndex;
        SyntacticCreateMetricAnalyzer *createMetric;
        SyntacticAlterTableAnalyzer *alter;
        SyntacticDropIndexAnalyzer *dropIndex;
        SyntacticDropMetricAnalyzer *dropMetric;
        SyntacticDropTableAnalyzer *dropTable;
        SyntacticDropDatabaseAnalyzer *dropDatabase;
        SyntacticShowAnalyzer *show;
        SyntacticDescAnalyzer *desc;
        SyntacticUse *use;

    public:
        SyntacticComplexSQLAnalyzer(string sql, DatabaseManager *dbmg) : SyntacticRegularSQLAnalyzer (sql, dbmg){

            select = new SyntacticSelectAnalyzer(sql, dbmg);
            update = new SyntacticUpdateAnalyzer(sql, dbmg);
            deletee = new  SyntacticDeleteAnalyzer(sql, dbmg);
            insert = new SyntacticInsertIntoAnalyzer(sql, dbmg);
            createTable = new SyntacticCreateTableAnalyzer(sql, dbmg);
            createDatabase = new SyntacticCreateDatabaseAnalyzer(sql, dbmg);
            createIndex = new SyntacticCreateIndexAnalyzer(sql, dbmg);
            createMetric = new SyntacticCreateMetricAnalyzer(sql, dbmg);
            alter = new SyntacticAlterTableAnalyzer(sql, dbmg);
            dropIndex = new SyntacticDropIndexAnalyzer(sql, dbmg);
            dropMetric = new SyntacticDropMetricAnalyzer(sql, dbmg);
            dropTable = new SyntacticDropTableAnalyzer(sql, dbmg);
            dropDatabase = new SyntacticDropDatabaseAnalyzer(sql, dbmg);
            show = new SyntacticShowAnalyzer(sql, dbmg);
            desc = new SyntacticDescAnalyzer(sql, dbmg);
            use = new SyntacticUse(sql, dbmg);
        }

        ~SyntacticComplexSQLAnalyzer();

        bool isValid();

        bool isSelect();
        bool isDML();
        bool isDDL();
        bool isDCL();

        void setSQLExpression(string sqlExpression);
};

#endif // SYNTACTICCOMPLEXSQLANALYZER_HPP
