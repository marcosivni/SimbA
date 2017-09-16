#include "SirenTime.hpp"

SirenTime::SirenTime(){

    start = end = 0.0;
    time(&timer);
}

void SirenTime::setStart(double start){

    this->start = start;
}

void SirenTime::setEnd(double end){

    this->end = end;
}

double SirenTime::getStart(){

    return start;
}

double SirenTime::getEnd(){

    return end;
}

double SirenTime::stopWatchInMilliseconds(){

    return (end-start)/CLOCKS_PER_SEC;
}
