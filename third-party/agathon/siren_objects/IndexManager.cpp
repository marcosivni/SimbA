#include "IndexManager.hpp"

IndexManager::IndexManager(DataDictionary *dic, Evaluator< Signature > distanceFunction, int pageSize, string fileName, Indexes indexType){

    slimTree = NULL;
    dummyTree = NULL;
    this->dic = dic;
    openIndex(distanceFunction, pageSize, fileName, indexType);
}

IndexManager::~IndexManager(){

    if (slimTree != NULL){
        delete (slimTree);
    }
    if (dummyTree != NULL){
        delete (dummyTree);
    }
}

void IndexManager::setIndexName(string indexName){

    this->indexName = indexName;
}

bool IndexManager::fileExists(string indexName){

    if (indexName.empty()){
        fstream file(getIndexName().c_str(), ios_base::in);
        return file.good();
    } else {
        fstream file(indexName.c_str(), ios_base::in);
        return file.good();
    }
}

string IndexManager::getIndexName(){

    return indexName;
}

DataDictionary *IndexManager::dictionary(){

    return dic;
}

void IndexManager::createSlimTree(int pageSize){

    close();

    stPlainDiskPageManager *pageManager = new stPlainDiskPageManager(("siren_slim_" + getIndexName()).c_str(), pageSize);

    slimTree = new SlimTree(pageManager);
    //As it's template a way to set how distance function to use
    Evaluator<Signature>* gambiarra = slimTree->GetMetricEvaluator();
    gambiarra = &evaluator;
}

void IndexManager::openSlimTree(){

    close();

    stPlainDiskPageManager *pageManager = new stPlainDiskPageManager(("siren_slim_" + getIndexName()).c_str());

    slimTree = new SlimTree(pageManager);
    //As it's template a way to set how distance function to use
    Evaluator<Signature>* gambiarra = slimTree->GetMetricEvaluator();
    gambiarra = &evaluator;
}

void IndexManager::closeSlimTree(){

    if (slimTree != NULL){
        delete (slimTree);
    }
    slimTree = NULL;
}

void IndexManager::dropSlimTree() throw (std::exception*){

    if (remove(("siren_slim_" + getIndexName()).c_str()) != 0)
        throw new std::exception();
}

void IndexManager::createDummyTree(int pageSize){

    close();

    stPlainDiskPageManager *pageManager = new stPlainDiskPageManager(("siren_dummy_" + getIndexName()).c_str(), pageSize);

    dummyTree = new DummyTree(pageManager);
    //As it's template a way to set how distance function to use
    Evaluator<Signature>* gambiarra = dummyTree->GetMetricEvaluator();
    gambiarra = &evaluator;
}

void IndexManager::openDummyTree(){

    close();

    stPlainDiskPageManager *pageManager = new stPlainDiskPageManager(("siren_dummy_" + getIndexName()).c_str());

    dummyTree = new DummyTree(pageManager);

    //As it's template a way to set how distance function to use
    Evaluator<Signature>* gambiarra = dummyTree->GetMetricEvaluator();
    gambiarra = &evaluator;
}

void IndexManager::closeDummyTree(){

    if (dummyTree != NULL){
        delete (dummyTree);
    }
    dummyTree = NULL;
}

void IndexManager::dropDummyTree() throw (std::exception*){

    if (remove(("siren_dummy_" + getIndexName()).c_str()) != 0)
        throw new std::exception();
}

void IndexManager::close(){

    closeSlimTree();
    closeDummyTree();
}

void IndexManager::openIndex(Evaluator< Signature > distanceFunction, int pageSize, string fileName, Indexes indexType){

    evaluator = distanceFunction;
    setIndexName(fileName);

    if ((evaluator.isMetrical()) && (indexType == SLIM_TREE)){
        if (fileExists(("siren_slim_" + fileName))){

            openSlimTree();
            openDummyTree();
        } else {
            createDummyTree(pageSize);
            createSlimTree(pageSize);
        }
    } else {
        if (fileExists(("siren_dummy_" + fileName))){
            openDummyTree();
        } else {
            createDummyTree(pageSize);
        }
    }
}

void IndexManager::removeItem(long OID) throw (std::exception*){

    //@todo
}

void IndexManager::addItem(Signature *item) throw (std::exception*){

    //    if (!itemExists(item)){
    if (slimTree != NULL){
        slimTree->Add(item);
        if (dummyTree != NULL)
            dummyTree->Add(item);
    } else {
        if (dummyTree != NULL){
            dummyTree->Add(item);
        } else {
            throw new std::exception();
        }
    }
    //    }
}

bool IndexManager::itemExists(Signature *item){

    ComplexResult *result = NULL;
    if (slimTree != NULL){
        result = slimTree->RangeQuery(item, 0);
    } else {
        if (dummyTree != NULL){
            result = dummyTree->RangeQuery(item, 0);
        }
    }

    if (result != NULL){
        if (result->GetNumOfEntries() > 0){
            delete (result);
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

ComplexResult* IndexManager::nearestSelectQuery(Signature* center, double k, bool tie){

    if (dummyTree != NULL){
        return dummyTree->NearestQuery(center, k, tie);
    } else {
        if (slimTree != NULL){
            return slimTree->NearestQuery(center, k, tie);
        } else {
            return NULL;
        }
    }
}


ComplexResult* IndexManager::nearestAllSelectQuery(Signature** centers, int centersSize, double k, bool tie){

    if (dummyTree != NULL){
        return dummyTree->AllNearestQuery(centers, centersSize, k, tie);
    } else {
        return NULL;
    }
}

ComplexResult* IndexManager::nearestMaxSelectQuery(Signature** centers, int centersSize, double k, bool tie){

    if (dummyTree != NULL){
        return dummyTree->MaxNearestQuery(centers, centersSize, k, tie);
    } else {
        return NULL;
    }
}

ComplexResult* IndexManager::nearestSumSelectQuery(Signature** centers, int centersSize, double k, bool tie){

    if (dummyTree != NULL){
        return dummyTree->SumNearestQuery(centers, centersSize, k, tie);
    } else {
        return NULL;
    }
}

ComplexResult* IndexManager::rangeSelectQuery(Signature* center, double radium){

    if (dummyTree != NULL){
        return dummyTree->RangeQuery(center, radium);
    } else {
        if (slimTree != NULL){
            return slimTree->RangeQuery(center, radium);
        } else {
            return NULL;
        }
    }
}

ComplexResult* IndexManager::rangeAllSelectQuery(Signature** centers, int centersSize, double radium){

    if (dummyTree != NULL){
        return dummyTree->AllRangeQuery(centers, centersSize, radium);
    } else {
        return NULL;
    }
}

ComplexResult* IndexManager::rangeMaxSelectQuery(Signature** centers, int centersSize, double radium){

    if (dummyTree != NULL){
        return dummyTree->MaxRangeQuery(centers, centersSize, radium);
    } else {
        return NULL;
    }
}

ComplexResult* IndexManager::rangeSumSelectQuery(Signature** centers, int centersSize, double radium){

    if (dummyTree != NULL){
        return dummyTree->SumRangeQuery(centers, centersSize, radium);
    } else {
        return NULL;
    }
}

ComplexResult* IndexManager::farthestSelectQuery(Signature* center, double k, bool tie){

    if (dummyTree != NULL){
        return dummyTree->FarthestQuery(center, k, tie);
    } else {
        return NULL;
    }
}

ComplexResult* IndexManager::farthestAllSelectQuery(Signature** centers, int centersSize, double k, bool tie){

    if (dummyTree != NULL){
        return dummyTree->FarthestQuery(centers[0], k, tie);
    } else {
        return NULL;
    }
}

ComplexResult* IndexManager::farthestMaxSelectQuery(Signature** centers, int centersSize, double k, bool tie){

    if (dummyTree != NULL){
        return dummyTree->FarthestQuery(centers[0], k, tie);
    } else {
        return NULL;
    }
}

ComplexResult* IndexManager::farthestSumSelectQuery(Signature** centers, int centersSize, double k, bool tie){

    if (dummyTree != NULL){
        return dummyTree->FarthestQuery(centers[0], k, tie);
    } else {
        return NULL;
    }
}

ComplexResult* IndexManager::reverseRangeSelectQuery(Signature* center, double radium){

    if (dummyTree != NULL){
        return dummyTree->ReversedRangeQuery(center, radium);
    } else {
        return NULL;
    }
}

ComplexResult* IndexManager::reverseRangeAllSelectQuery(Signature** centers, int centersSize, double radium){

    if (dummyTree != NULL){
        return dummyTree->ReversedRangeQuery(centers[0], radium);
    } else {
        return NULL;
    }
}

ComplexResult* IndexManager::reverseRangeMaxSelectQuery(Signature** centers, int centersSize, double radium){

    if (dummyTree != NULL){
        return dummyTree->ReversedRangeQuery(centers[0], radium);
    } else {
        return NULL;
    }
}

ComplexResult* IndexManager::reverseRangeSumSelectQuery(Signature** centers, int centersSize, double radium){

    if (dummyTree != NULL){
        return dummyTree->ReversedRangeQuery(centers[0], radium);
    } else {
        return NULL;
    }
}

ComplexResult* IndexManager::rangeJoinQuery(vector<Signature> centers, double k){
}

ComplexResult* IndexManager::nearestJoinQuery(vector<Signature> centers, double k){
}

ComplexResult* IndexManager::closestJoinQuery(vector<Signature> centers, double k){
}

