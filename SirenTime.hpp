#ifndef SIRENTIME_HPP
#define SIRENTIME_HPP

#include <cstdio>
#include <ctime>

class SirenTime{

    private:
        double start, end;
        time_t timer;

    public:
        SirenTime();

        void setStart(double start = (double)  clock());
        void setEnd(double end = (double)  clock());

        double getStart();
        double getEnd();

        double stopWatchInMilliseconds();

};

#endif // SIRENTIME_HPP
