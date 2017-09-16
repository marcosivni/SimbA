#ifndef PARSETREETOSQL_HPP
#define PARSETREETOSQL_HPP

#include <string>
#include <vector>
#include "ParseTree.hpp"
#include "ACTLine.hpp"
#include "PTBlock.hpp"

using namespace std;

class ParseTreeToSQL
{
	public:
		ParseTreeToSQL();

		string execute(ParseTree pt);

	private:
		vector < string > select;
		vector < string > from;
		vector < string > where;
};

#endif // PARSETREETOSQL_HPP
