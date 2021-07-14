#include "IndexManager.h"

/**
* Constructor.
*
* @param distanceFunction The employed distance function.
* @param pageSize The page size value. For a page size with a value 0, the system will create a new MAM.
*/
IndexManager::IndexManager(std::string indexName, MetricDistanceFunction distanceFunction, bool createDummyOnly, u_int32_t pageSize){

    setIndexName(indexName);
    setDistanceFunction(distanceFunction);

    slimTree = nullptr;
    dummyTree = nullptr;

    if (createDummyOnly){
        dropDummyTree();
        createDummyTree(pageSize);
    } else {
        if (!fileExists(getSlimFileName())){
            createSlimTree(pageSize);
            createDummyTree(pageSize);
        } else {
            openSlimTree();
            openDummyTree();
        }
    }
}

/**
* Destructor.
*
*/
IndexManager::~IndexManager(){

    if (slimTree != nullptr){
        delete (slimTree);
    }
    if (dummyTree != nullptr){
        delete (dummyTree);
    }
}

/**
* Sets the distance function.
*
* @param df The distance function object.
*/
void IndexManager::setDistanceFunction(MetricDistanceFunction df){

    mdf = df;
}

/**
* Sets the index name.
*
* @param indexName The index name.
*/
void IndexManager::setIndexName(std::string indexName){

    this->indexName = indexName;
}

/**
* Checks if the index file already exists.
*
* @param indexName The name of index file.
* @return True if the file exists, false otherwise.
*/
bool IndexManager::fileExists(std::string indexName){

    if (indexName.empty()){
        std::fstream file(getIndexName(), std::ios_base::in);
        return file.good();
    } else {
        std::fstream file(indexName, std::ios_base::in);
        return file.good();
    }
}

/**
* Return the index name.
*
* @return The index filename.
*/
std::string IndexManager::getIndexName(){

    return indexName;
}

std::string IndexManager::getSlimFileName(){

    return filePath + "_slim_" + getIndexName();
}

std::string IndexManager::getDummyFileName(){

    return filePath + "_dummy_" + getIndexName();
}

/**
* Creates the slim-tree and save it into a file.
*
* @param pageSize The page size.
*/
void IndexManager::createSlimTree(const u_int32_t pageSize){

    closeSlimTree();

    stPlainDiskPageManager *pageManager = new stPlainDiskPageManager(getSlimFileName().c_str(), pageSize);

    slimTree = new SlimTree(pageManager);
    slimTree->SetChooseMethod(SlimTree::cmMINDIST);
    slimTree->SetSplitMethod(SlimTree::smMINMAX);

    //Set the distance function
    slimTree->GetMetricEvaluator()->setDistanceFunction(mdf.distanceType());
}

/**
* Open the slim-tree.
*
*/
void IndexManager::openSlimTree(){

    closeSlimTree();

    stPlainDiskPageManager *pageManager = new stPlainDiskPageManager(getSlimFileName().c_str());

    slimTree = new SlimTree(pageManager);
    slimTree->SetChooseMethod(SlimTree::cmMINDIST);
    slimTree->SetSplitMethod(SlimTree::smMINMAX);

    //Set the distance function
    slimTree->GetMetricEvaluator()->setDistanceFunction(mdf.distanceType());
}

/**
* Closes the slim-tree and free the memory.
*
*/
void IndexManager::closeSlimTree(){

    if (slimTree != nullptr){
        delete (slimTree);
    }
    slimTree = nullptr;
}

/**
* Drops the slim-tree.
*
* @throw std::exception If the file cannot be removed.
*/
void IndexManager::dropSlimTree() throw (std::exception*){

    if (remove(getSlimFileName().c_str()) != 0)
        throw new std::exception();
}

/**
* Creates the dummy-tree and save it into a file.
*
* @param pageSize The page size.
*/
void IndexManager::createDummyTree(const u_int32_t pageSize){

    closeDummyTree();

    stPlainDiskPageManager *pageManager = new stPlainDiskPageManager(getDummyFileName().c_str(), pageSize);

    dummyTree = new DummyTree(pageManager);

    //Set the distance function
    dummyTree->GetMetricEvaluator()->setDistanceFunction(mdf.distanceType());
}

/**
* Open the dummy-tree.
*
*/
void IndexManager::openDummyTree(){

    closeDummyTree();

    stPlainDiskPageManager *pageManager = new stPlainDiskPageManager(getDummyFileName().c_str());

    dummyTree = new DummyTree(pageManager);

    //Set the distance function
    dummyTree->GetMetricEvaluator()->setDistanceFunction(mdf.distanceType());

}

/**
* Closes the dummy-tree and free the memory.
*
*/
void IndexManager::closeDummyTree(){

    if (dummyTree != nullptr){
        delete (dummyTree);
    }
    dummyTree = NULL;
}

/**
* Drops the dummy-tree.
*
* @throw std::exception If the file cannot be removed.
*/
void IndexManager::dropDummyTree() throw (std::exception*){

    if (fileExists(getDummyFileName())){
        if (remove(getDummyFileName().c_str()) != 0)
            throw new std::exception();
    }
}

/**
* Closes both dummy and slim-tree.
*
*/
void IndexManager::close(){

    closeSlimTree();
    closeDummyTree();
}

/**
* Checks if the index file exists.
* If there is such a file, open the file (index).
*
* @param pageSize The page size value,
*/
void IndexManager::openIndex(const u_int32_t pageSize){


    if (fileExists(indexName)){
        openSlimTree();
        openDummyTree();
    } else {
        createDummyTree(pageSize);
        createSlimTree(pageSize);
    }

}

/**
* Add a new item into the indexes.
*
* @param item The feature vector to be inserted.
*/
void IndexManager::addItem(FeatureVector *item) throw (std::exception*){

    if (!itemExists(item)){
        if (slimTree != nullptr){
            slimTree->Add(item);
            if (dummyTree != nullptr)
                dummyTree->Add(item);
        } else {
            if (dummyTree != nullptr){
                dummyTree->Add(item);
            } else {
                throw new std::exception();
            }
        }
    }
}

/**
* Checks if the item exists.
*
* @param item The feature vector to be compared.
* @return True if the item has been already inserted in the index, false otherwise.
*/
bool IndexManager::itemExists(FeatureVector *item){

    ComplexResult *result = nullptr;
    if (slimTree != nullptr){
        result = slimTree->RangeQuery(item, 0);
    } else {
        if (dummyTree != nullptr){
            result = dummyTree->RangeQuery(item, 0);
        }
    }

    if (result != nullptr){
        if (result->GetNumOfEntries() > 0){
            delete (result);
            return true;
        }
    }
    return false;
}

/**
* Perform a nearest query using a given query element.
*
* @param center The feature vector (query element).
* @param k The number of k elements that should be returned.
* @param tie To catch the elements with the same distance tied as the k-th neighbor.
* Default value is false.
*
* @return The list of the k nearest-neighbors sorted by distance to the query element.
*/
ComplexResultList IndexManager::nearestSelectQuery(FeatureVector* center, u_int32_t k, bool tie){

    ComplexResultList answer;
    ComplexResult *resultSet;

    if (slimTree != nullptr){
        resultSet = slimTree->NearestQuery(center, k, tie);
    } else {
        resultSet = dummyTree->NearestQuery(center, k, tie);
    }

    answer.push_back(resultSet);
    return answer;
}

ComplexResultList IndexManager::nearestAllSelectQuery(FeatureVectorList centers, int centersSize, double k, bool tie){

    ComplexResultList answer;
    ComplexResult* rSet = nullptr;

    if (dummyTree != NULL){
        FeatureVector **sqs = new FeatureVector*[centers.size()];
        for (size_t x = 0; x < centers.size(); x++){
            sqs[x] = new FeatureVector(centers[x]);
        }
        rSet = dummyTree->AllNearestQuery(sqs, centersSize, k, tie);
        for (size_t x = 0; x < centers.size(); x++){
            delete (sqs[x]);
        }
        delete[] (sqs);
    }

    answer.push_back(rSet);
    return answer;
}

ComplexResultList IndexManager::nearestMaxSelectQuery(FeatureVectorList centers, int centersSize, double k, bool tie){

    ComplexResultList answer ;
    ComplexResult* rSet = nullptr;

    if (dummyTree != NULL){
        FeatureVector **sqs = new FeatureVector*[centers.size()];
        for (size_t x = 0; x < centers.size(); x++){
            sqs[x] = new FeatureVector(centers[x]);
        }
        rSet = dummyTree->MaxNearestQuery(sqs, centersSize, k, tie);
        for (size_t x = 0; x < centers.size(); x++){
            delete (sqs[x]);
        }
        delete[] (sqs);
    }

    answer.push_back(rSet);
    return answer;
}

ComplexResultList IndexManager::nearestSumSelectQuery(FeatureVectorList centers, int centersSize, double k, bool tie){

    ComplexResultList answer ;
    ComplexResult* rSet = nullptr;

    if (dummyTree != NULL){
        FeatureVector **sqs = new FeatureVector*[centers.size()];
        for (size_t x = 0; x < centers.size(); x++){
            sqs[x] = new FeatureVector(centers[x]);
        }
        rSet = dummyTree->SumNearestQuery(sqs, centersSize, k, tie);
        for (size_t x = 0; x < centers.size(); x++){
            delete (sqs[x]);
        }
        delete[] (sqs);
    }

    answer.push_back(rSet);
    return answer;
}

/**
* Perform a range query using a given query element.
*
* @param center The feature vector (query element).
* @param radium The maximum allowed distance.
*
* @return The list of neighbor elements sorted by distance to the query element
*/
ComplexResultList IndexManager::rangeSelectQuery(FeatureVector* center, double radius){

    ComplexResultList answer ;
    ComplexResult *resultSet;

    if (slimTree != nullptr){
        resultSet = slimTree->RangeQuery(center, radius);
    } else {
        resultSet = dummyTree->RangeQuery(center, radius);
    }

    answer[0] = resultSet;
    return answer;
}


ComplexResultList IndexManager::rangeAllSelectQuery(FeatureVectorList centers, int centersSize, double radius){

    ComplexResultList answer ;
    ComplexResult* rSet = nullptr;

    if (dummyTree != NULL){
        FeatureVector **sqs = new FeatureVector*[centers.size()];
        for (size_t x = 0; x < centers.size(); x++){
            sqs[x] = new FeatureVector(centers[x]);
        }
        rSet = dummyTree->AllRangeQuery(sqs, centersSize, radius);
        for (size_t x = 0; x < centers.size(); x++){
            delete (sqs[x]);
        }
        delete[] (sqs);
    }

    answer.push_back(rSet);
    return answer;
}

ComplexResultList IndexManager::rangeMaxSelectQuery(FeatureVectorList centers, int centersSize, double radius){

    ComplexResultList answer ;
    ComplexResult* rSet = nullptr;

    if (dummyTree != NULL){
        FeatureVector **sqs = new FeatureVector*[centers.size()];
        for (size_t x = 0; x < centers.size(); x++){
            sqs[x] = new FeatureVector(centers[x]);
        }
        rSet  = dummyTree->MaxRangeQuery(sqs, centersSize, radius);
        for (size_t x = 0; x < centers.size(); x++){
            delete (sqs[x]);
        }
        delete[] (sqs);
    }

    answer.push_back(rSet);
    return answer;
}

ComplexResultList IndexManager::rangeSumSelectQuery(FeatureVectorList centers, int centersSize, double radius){

    ComplexResultList answer ;
    ComplexResult* rSet = nullptr;

    if (dummyTree != NULL){
        FeatureVector **sqs = new FeatureVector*[centers.size()];
        for (size_t x = 0; x < centers.size(); x++){
            sqs[x] = new FeatureVector(centers[x]);
        }
        rSet = dummyTree->SumRangeQuery(sqs, centersSize, radius);
        for (size_t x = 0; x < centers.size(); x++){
            delete (sqs[x]);
        }
        delete[] (sqs);
    }

    answer.push_back(rSet);
    return answer;
}

ComplexResultList IndexManager::farthestSelectQuery(FeatureVector* center, double k, bool tie){

    ComplexResultList answer ;
    ComplexResult* rSet = nullptr;

    if (dummyTree != nullptr){
        rSet = dummyTree->FarthestQuery(center, k, tie);
    }

    answer.push_back(rSet);
    return answer;
}

ComplexResultList IndexManager::farthestAllSelectQuery(FeatureVectorList centers, int centersSize, double k, bool tie){

    ComplexResultList answer ;
    ComplexResult* rSet = nullptr;

    if (dummyTree != nullptr && centers.size() > 0){
        rSet = dummyTree->FarthestQuery(&centers[0], k, tie);
    }

    answer.push_back(rSet);
    return answer;
}

ComplexResultList IndexManager::farthestMaxSelectQuery(FeatureVectorList centers, int centersSize, double k, bool tie){

    ComplexResultList answer ;
    ComplexResult* rSet = nullptr;

    if (dummyTree != nullptr && centers.size() > 0){
        rSet =  dummyTree->FarthestQuery(&centers[0], k, tie);
    }

    answer.push_back(rSet);
    return answer;
}

ComplexResultList IndexManager::farthestSumSelectQuery(FeatureVectorList centers, int centersSize, double k, bool tie){

    ComplexResultList answer ;
    ComplexResult* rSet = nullptr;

    if (dummyTree != nullptr && centers.size() > 0){
        rSet =  dummyTree->FarthestQuery(&centers[0], k, tie);
    }

    answer.push_back(rSet);
    return answer;
}

ComplexResultList IndexManager::reverseRangeSelectQuery(FeatureVector* center, double radius){

    ComplexResultList answer;
    ComplexResult* rSet = nullptr;

    if (dummyTree != nullptr){
        rSet = dummyTree->ReversedRangeQuery(center, radius);
    }

    answer.push_back(rSet);
    return answer;
}

ComplexResultList IndexManager::reverseRangeAllSelectQuery(FeatureVectorList centers, int centersSize, double radius){

    ComplexResultList answer;
    ComplexResult* rSet = nullptr;

    if (dummyTree != nullptr && centers.size() > 0){
        rSet = dummyTree->ReversedRangeQuery(&centers[0], radius);
    }

    answer.push_back(rSet);
    return answer;
}

ComplexResultList IndexManager::reverseRangeMaxSelectQuery(FeatureVectorList centers, int centersSize, double radius){

    ComplexResultList answer;
    ComplexResult* rSet = nullptr;

    if (dummyTree != nullptr && centers.size() > 0){
        rSet = dummyTree->ReversedRangeQuery(&centers[0], radius);
    }

    answer.push_back(rSet);
    return answer;
}

ComplexResultList IndexManager::reverseRangeSumSelectQuery(FeatureVectorList centers, int centersSize, double radius){

    ComplexResultList answer;
    ComplexResult* rSet = nullptr;

    if (dummyTree != nullptr && centers.size() > 0){
        rSet = dummyTree->ReversedRangeQuery(&centers[0], radius);
    }

    answer.push_back(rSet);
    return answer;
}

/**
* Gets the tree height of the current index.
*
* @return The tree height.
*/
u_int16_t IndexManager::getTreeHeight(){

    if (slimTree != nullptr){
        return slimTree->GetHeight();
    }
    return 0;
}

/**
* Gets the employed distance function.
*
* @return The distance function employed in the index.
*/
MetricDistanceFunction * IndexManager::getMetricDistanceFunction(){

    return &mdf;
}

/**
* Gets the number of elements in the current index.
*
* @return The number of elements inserted on the index.
*/
u_int32_t IndexManager::getNumberOfElements(){

    if (dummyTree != NULL){
        return dummyTree->GetNumberOfObjects();
    }
    return 0;
}

/**
* Performs a diversity query using a given query element.
*
* @param center The feature vector (query element).
* @param k1 The maximum number of influential elements that should be returned.
* @param k2 The maximum number of influentied elements that should be returned.
*
* @return A sorted list (to the query element) of groups, in which the head is the influential elements and the
* queue contains the influentied elements sorted by distance to the head.
*/
ComplexResultList IndexManager::diverseNearestQuery(FeatureVector* center, u_int32_t k1, u_int32_t k2){

    ComplexResultList resultSet;
    ComplexResult *tmpResult;

    resultSet = dummyTree->diverseBRIDGEQuery(center, k1);
    for (size_t x = 0; x < resultSet.size(); x++){
        tmpResult = resultSet[x];
        while (tmpResult->GetNumOfEntries() > k2){
            tmpResult->RemoveLast();
        }
    }

    return resultSet;
}
