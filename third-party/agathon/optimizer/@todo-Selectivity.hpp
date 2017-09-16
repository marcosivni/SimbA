#ifndef SELECTIVITY_HPP
#define SELECTIVITY_HPP

#include <iostream>
#include <cmath>
#include <cstdio>
#include <vector>
#include <DataDictionary.hpp>

using namespace std;

class Selectivity{

    private:
        DataDictionary *dic;

    public:
        Selectivity(DataDictionary *dic);

        double getSelectivity(string tableName);
        double getSelectivity(string attribute, string comparison, string value, string table);
        double getSelectivity(string attribute, string comparison, string value, string table, string idxName);
};

#endif // SELECTIVITY_HPP
