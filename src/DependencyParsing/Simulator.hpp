#ifndef __SIMULATOR__
#define __SIMULATOR__

#include <vector>

#include "Sentence.hpp"
#include "WordAgent.hpp"
#include "Evaluation.hpp"
#include "Environment.hpp"

class WordAgent;
class Evaluation;
class Environment;

class Simulator{
private:

	int times;
	int agNum;
	int agentId;
	int rows, cols;
	int learnTimes;

	std::vector<double> tmpFW;
	std::vector<double> historyAccuracy;
	std::vector<std::map<int,WordAgent> > vWordAgents;
	std::vector<std::vector<std::map<int,WordAgent> > > globalWordAgent;

	Environment * env;
public:
        Evaluation * eva;

	Simulator(Environment * environment,Evaluation * evaluation);

	void init();
	int getAgNum();
	void getLearnTime(int times);

	bool resetAgents();
	bool select(WordAgent & wa);
	bool interact(WordAgent & wa);
	bool addWordAgent(WordAgent & pWordAgent);
	bool regulateByDelete(WordAgent & wa,int N);
	bool deleteWordAgent(WordAgent & pWordAgent);
	bool run(const Sentence & sen, const std::vector<int> & fa);

	void initAccuracy();
	void recordAccuracy(double & acc);
	std::vector<double> getAccuracy();

	bool initLocalAgentsFlags();
	bool initLocalAgents(WordAgent * wa);
	int  calConcentration(WordAgent * wa);
	bool getNearbyAgents(const int id, const std::pair<int,int> &pos, std::vector<WordAgent> & nearbyAgents);

	int agentCount(std::pair<int,int> & position);
        void setAgNum(int num);

        void storeLocalWordAgent(int learnTimes, int parts);
        void getLocalWordAgent(int parts);
        void resetWordAgents(int parts);

        int getRegions();

private:
        int _getAgNum();
        void _updateAgNum();

        int _calcSub(const std::pair<int, int> & pos) const;

        bool _release();
        bool _removeAg();
        bool _resetStatus();
        bool resetInteratObjects();
        bool _isSame(const std::vector<int> & s, const std::vector<int> & d);
};

#endif
