#ifndef INDEXMANAGER_HPP
#define INDEXMANAGER_HPP

#include <arboretum/stPlainDiskPageManager.h>
#include <arboretum/stSlimNode.h>
#include <arboretum/stSlimTree.h>
#include <arboretum/stDummyTree.h>
#include <BasicArrayObject.hpp>

#include <ComplexObject.hpp>
#include <Evaluator.hpp>

#include <iostream>
#include <cstdio>
#include <fstream>

using namespace std;

//Template-structured -> arb code
typedef BasicArrayObject<double> Signature;
typedef stSlimTree < Signature, Evaluator< Signature > > SlimTree;
typedef stDummyTree< Signature, Evaluator< Signature > > DummyTree;
//Results of internal queries
typedef stResult < Signature > ComplexResult; //DynamicResult
typedef stJoinedResult < Signature > ComplexJoinedResult; //DynamicJoinedResult

class IndexManager{

    private:
        SlimTree *slimTree;
        DummyTree *dummyTree;
        Evaluator< Signature > evaluator;
        //Hash and B+-Tree missing?
        string indexName;
        DataDictionary *dic;

    public:
        static const int PAGE_SIZE =  65536;

    private:
        void setIndexName(string indexName);
        bool fileExists(string indexName = "");
        string getIndexName();
        DataDictionary *dictionary();

        void createSlimTree(int pageSize);
        void openSlimTree();
        void closeSlimTree();
        void dropSlimTree() throw (std::exception*);

        void createDummyTree(int pageSize);
        void openDummyTree();
        void closeDummyTree();
        void dropDummyTree() throw (std::exception*);

    public:
        enum Indexes{
            SLIM_TREE = 1,
            DUMMY_TREE = 2
        };

    public:
        IndexManager(DataDictionary *dic, Evaluator< Signature > distanceFunction, int pageSize, string fileName, Indexes indexType = SLIM_TREE);
        ~IndexManager();

        void close();
        void openIndex(Evaluator< Signature > distanceFunction, int pageSize, string fileName, Indexes indexType);

        void addItem(Signature *item) throw (std::exception*);
        void removeItem(long OID) throw (std::exception*);
        bool itemExists(Signature *item);

        ComplexResult* nearestSelectQuery(Signature* center, double k, bool tie);
        ComplexResult* nearestAllSelectQuery(Signature** centers, int centersSize, double k, bool tie);
        ComplexResult* nearestMaxSelectQuery(Signature** centers, int centersSize, double k, bool tie);
        ComplexResult* nearestSumSelectQuery(Signature** centers, int centersSize, double k, bool tie);

        ComplexResult* rangeSelectQuery(Signature* center, double radium);
        ComplexResult* rangeAllSelectQuery(Signature** centers, int centersSize, double radium);
        ComplexResult* rangeMaxSelectQuery(Signature** centers, int centersSize, double radium);
        ComplexResult* rangeSumSelectQuery(Signature** centers, int centersSize, double radium);

        ComplexResult* farthestSelectQuery(Signature* center, double k, bool tie);
        ComplexResult* farthestAllSelectQuery(Signature** centers, int centersSize, double k, bool tie);
        ComplexResult* farthestMaxSelectQuery(Signature** centers, int centersSize, double k, bool tie);
        ComplexResult* farthestSumSelectQuery(Signature** centers, int centersSize, double k, bool tie);

        ComplexResult* reverseRangeSelectQuery(Signature* center, double radium);
        ComplexResult* reverseRangeAllSelectQuery(Signature** centers, int centersSize, double radium);
        ComplexResult* reverseRangeMaxSelectQuery(Signature** centers, int centersSize, double radium);
        ComplexResult* reverseRangeSumSelectQuery(Signature** centers, int centersSize, double radium);

        ComplexResult* rangeJoinQuery(vector<Signature> centers, double k);
        ComplexResult* nearestJoinQuery(vector<Signature> centers, double k);
        ComplexResult* closestJoinQuery(vector<Signature> centers, double k);

        //JOINS will be implemented later
};

#endif // INDEXMANAGER_HPP
