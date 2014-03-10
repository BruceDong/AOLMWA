#include "WordAgent.hpp"
#include "Parameter.hpp"
#include "Environment.hpp"
#include "DependencyPaser.hpp"

#include <iostream>

using namespace std;

void testWordAgentInit()
{
	std::pair<int, int> pos;
	pos.first = 3;
	pos.second = 3;
}

void testEvaluation()
{
        DependencyPaser dp;
        dp.evaluate("./result/result.conll","./result/evaluate.conll");
}

void testParsing()
{
        DependencyPaser dp;

        dp.parsing("./data/DataSetForDependencyParsing/train-10","./data/DataSetForDependencyParsing/train-10","./result/result.conll","./result/evaluate.conll");

}
