#include "ParseTreeToSQL.hpp"

ParseTreeToSQL::ParseTreeToSQL()
{
}

string ParseTreeToSQL::execute(ParseTree pt)
{
	int size = pt.getPTSize();

	for (int i = 0; i < size; i++)
	{
		PTBlock pb = pt.getPTBlock(i);

		int operation = pb.getOperation();
		ACTLine ac;
		string cond;

		switch(operation)
		{
			case PTBlock::READ:
				from.push_back(pb.getTable());
				break;
			case PTBlock::SELECT:
				ac = pt.getACTLine(pb.getActLinesReferences()[0]);
				cond = "" + ac.getAttribute() + " " + ac.getOperattor() + " " + ac.getValue();
				where.push_back(cond);
				break;
			case PTBlock::SIMILARITY_SELECT:
				ac = pt.getACTLine(pb.getActLinesReferences()[0]);
				cond = "" + ac.getAttribute() + " NEAR " + ac.getValue() + " ";
				if(ac.getOperattor() == "RQ")
				{
					cond += "RANGE " + ac.getParameter("r");
				}
				else if(ac.getOperattor() == "KNN")
				{
					cond += "STOP AFTER " + ac.getParameter("k");
				}
				where.push_back(cond);
				break;
			case PTBlock::PROJECTION:
				ac = pt.getACTLine(pb.getActLinesReferences()[0]);
				select.push_back(ac.getAttribute());
				break;
		}
	}

	string sql = "SELECT ";
	for (int i = 0; i < select.size(); i++)
	{
		sql += select[i];
	}
	sql += " FROM ";
	for (int i = 0; i < from.size(); i++)
	{
		sql += from[i];
	}
	sql += " WHERE ";
	for (int i = 0; i < where.size(); i++)
	{
		sql += where[i] + " AND ";
	}
	sql += "1 = 1;";

	return sql;
}
