#ifndef __TRAINER__
#define __TRAINER__

#include "Model.hpp"
#include "Sentence.hpp"
#include "WordAgent.hpp"
#include "Simulator.hpp"
#include "Environment.hpp"
#include "FeatureTemplate.hpp"


class Trainer{
private:
	int senID;

	fstream f;

	std::map<std::string, int> wordID;

	std::vector<int> wordFreq;
	std::vector<WordAgent> BCells;
	std::vector<WordAgent> Antigens;
	std::vector<std::vector<double> > feaSet;
	std::vector<std::pair<Sentence,std::vector<int> > >  vSen;

	Model * pModel;
	Simulator * simu;
	Evaluation * pEva;
	Environment * pEnv;

public:
	Trainer(Model * pm, Evaluation * eva);
	~Trainer();

	bool rfTrain(const Sentence & sen, const int senID,const std::vector<int> & fa, int iter);

	bool constructBcellNet();
	bool constructBcellNet(int p, int learntimes);
	bool addBCells(int senID, const Sentence & sen, const std::vector<int> & fa);

        bool cloneBCells();
        bool cloneAntigens();

        void testSub();
        void testAgentNum();

        bool saveFeatureWeights();
        bool mergeFeatureWeights();

        void initSentences();
        bool initSentenceID();

        void storeWordAgent(int learnTimes, int parts);

private:
	int _buildBCell(const std::string & word);

	bool _addAntigen();
	bool _addAntigenToSimulator(const Sentence & sen, const std::vector<int> & fa);
	bool _injectAntigen(const Sentence & sen, const std::vector<int> & fa);
	bool _buildAntigen(const Sentence & sen,int child,const std::string & word,
                                int parent,const std::string & fword);

};

#endif

