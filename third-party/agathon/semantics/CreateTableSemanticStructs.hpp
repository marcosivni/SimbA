#ifndef CREATETABLESEMANTICSTRUCTS_HPP
#define CREATETABLESEMANTICSTRUCTS_HPP

#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <DataDictionary.hpp>

using namespace std;

class CheckValidColumnsAndTypes{

    private:
        vector< pair<string, string> > listColumns;
        DataDictionary *dic;
        string error;

    public:
        CheckValidColumnsAndTypes(DataDictionary *dic);
        ~CheckValidColumnsAndTypes();

        void setDataType(int pos, string dataType) throw (std::exception*);
        void addNewColumn(string columnName, string dataType);
        void addNewColumn(string columnName);
        void addNewDataType(string dataType);
        void addError(string error);

        int countColumns(string columnName);

        bool isComplexColumn(string columnName);
        bool isValid();
        bool isUniqueColumnNames();

        string dataType(string columnName);
        DataDictionary *dictionary();

        int getSize();
        pair<string, string> getColumnNameAndDataType(int pos) throw (std::exception*);
        string getError();
        string toString();
};

class Constraint{

    private:
        string name;
        unsigned char type;
        vector<string> attributes;
        string referenceTableName;
        vector< pair <string, bool> > usingMetricName;
        vector<string> referenceAttributes;

    public:
        static const unsigned char PRIMARY_KEY = 0;
        static const unsigned char FOREIGN_KEY = 1;
        static const unsigned char METRIC = 2;
        static const unsigned char UNIQUE = 3;
        static const unsigned char NOT_NULL = 4;
        static const unsigned char NULL_ = 5;

    public:
        Constraint();
        ~Constraint();

        void clear();

        void setName(string name);
        void setType(unsigned char type);
        void addAttribute(string attribute);
        void setReferenceTableName(string referenceTableName);
        void addMetricName(pair<string, bool> usingMetricName);
        void addReferenceAttribute(string referenceAttribute);

        bool isValid();
        bool isEqual(Constraint constraint);

        string getName();
        unsigned char getType();
        vector<string> getAttributes();
        string getReferenceTableName();
        vector< pair<string, bool> > getMetricsNames();
        vector<string> getReferenceAttributes();
};

class CheckConstraints{

    private:
        vector <Constraint> constraintList;
        string error;
        DataDictionary *dic;

    public:
        CheckConstraints(DataDictionary *dic);
        ~CheckConstraints();

        void addNewConstraint(Constraint constraint);
        void addError(string error);

        int countNames(string constraintName);
        int countMetrics(Constraint constraint);
        int countDefaults(Constraint constraint);
        int countPrimaryKey();
        int countForeignKey(Constraint constraint);
        int countUnique(Constraint constraint);

        bool checkAttributes(CheckValidColumnsAndTypes *columnsAndTypes);
        bool isValid(CheckValidColumnsAndTypes *columnsAndTypes);
        bool isConstraints();

        Constraint getConstraint(int pos)  throw (std::exception*);
        int getSize();
        string getError();
        DataDictionary *dictionary();
        string toString();
};

#endif // CREATETABLESEMANTICSTRUCTS_HPP
