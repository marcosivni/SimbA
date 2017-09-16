#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

#include <Hermes.hpp>
#include <DataDictionary.hpp>

template <class FeatureVector>
class Evaluator{

private:
    unsigned char types;
    DataDictionary *dic;
    bool metrical;

public:
    enum Distances{
        Euclidean = 1,
        CityBlock = 2,
        Chebyshev = 3,
        MHD = 4,
        Jeffrey = 5,
        Canberra = 6
    };

private:
    DataDictionary *dictionary(){
        return dic;
    }

public:

    Evaluator(){

        dic = NULL;
        types = 0;
        metrical = false;
    }

    Evaluator(string distanceFunction, DataDictionary *dic){

        this->dic = dic;
        if (dictionary()->isDistanceFunctionName(distanceFunction)){
            types = atoi(dictionary()->getDistanceFunctionCode(distanceFunction).c_str());
            metrical = dictionary()->isMetrical(dictionary()->getDistanceFunctionCode(distanceFunction));
        } else {
            throw new std::exception();
        }
    }


    unsigned char getType(){

        return types;
    }

    bool isMetrical(){

        return metrical;
    }

    void setDistanceFunction(unsigned char distanceFunction, DataDictionary *dic) throw (std::exception*){

        this->dic = dic;
        types = distanceFunction;
        char last[21];
        sprintf(last, "%d", types);
        string tmp = last;
        metrical = dictionary()->isMetrical(dictionary()->getDistanceFunctionCode(tmp));
    }

    stDistance getDistance(FeatureVector *obj1, FeatureVector *obj2){

        return GetDistance(obj1, obj2);
    }

    stDistance GetDistance(FeatureVector *obj1, FeatureVector *obj2){

        stDistance answer;

        if (types == Euclidean){
            EuclideanDistance<FeatureVector> *d = new EuclideanDistance<FeatureVector>();
            answer = d->getDistance(obj1, obj2);
            delete(d);
        } else {
            if (types == CityBlock){
                ManhattanDistance<FeatureVector>  *d = new ManhattanDistance<FeatureVector> ();
                answer = d->getDistance(obj1, obj2);
                delete(d);
            } else {
                if (types == Chebyshev){
                    ChebyshevDistance<FeatureVector>  *d = new ChebyshevDistance<FeatureVector> ();
                    answer = d->getDistance(obj1, obj2);
                    delete(d);
                } else {
                    if (types == MHD){
                        MetricHistogramDistance<FeatureVector>  *d = new MetricHistogramDistance<FeatureVector> ();
                        answer = d->getDistance(obj1, obj2);
                        delete(d);
                    } else {
                        if (types == Jeffrey){
                            JeffreyDivergenceDistance<FeatureVector>  *d = new JeffreyDivergenceDistance<FeatureVector> ();
                            answer = d->getDistance(obj1, obj2);
                            delete(d);
                        } else {
                            if (types == Canberra){
                                CanberraDistance<FeatureVector>  *d = new CanberraDistance<FeatureVector> ();
                                answer = d->getDistance(obj1, obj2);
                                delete(d);
                            }
                        }
                    }
                }
            }
        }

        return answer;
    }
};

#endif // EVALUATOR_HPP
