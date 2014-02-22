#ifndef __ENVIRONMENT__
#define __ENVIRONMENT__

#include <vector>
#include <map>
#include <set>

#include "Sentence.hpp"
#include "Predictor.hpp"
#include "Evaluation.hpp"

class WordAgent;
class Evaluation;
class Information;


class Environment{
private:

	int senID;
	int agentId;
	int agentNum;
	int pBcellNum;
	int rows, cols;
	int pAntigenNum;
	int antigenQuantity;

	bool feedback;
	bool isSecondResponse;
	bool havePositiveFeedback;

        std::vector<int> fa;
	std::vector<int> vAgID;
	std::vector<int> badrow;
	std::vector<int> badcol;
        std::vector<WordAgent *> address;
        std::vector<WordAgent *> delAddress;
		std::vector<std::vector<int> > grid;
        std::vector<std::map<int,WordAgent> > pWordAgents;

        Model * mod;
	Sentence sen;
	Evaluation * eva;
	Information * infor;

public:
	Environment(int r, int c, Evaluation * evaluation, Model *model);

	bool addPWordAgent(WordAgent & pWordAgent);
	bool addPWordAgent(WordAgent & pWordAgent, int category);
	bool delPWordAgent(WordAgent & pWordAgent);

	bool resetAgents();
	bool getNearbyAgents(const int id,const std::pair<int,int> & pos,
		std::vector<WordAgent> & neabyAgents) const;
        std::vector<std::map<int, WordAgent> > getAgents();
	int agentCount(const std::pair<int, int> & pos);
	std::pair<int, int> getRandomPosition();
	bool xInRange(int x);
	bool yInRange(int y);
	bool update(WordAgent * pWordAgent);
	std::map<int, double> getInfor(WordAgent * pWordAgent);
	std::pair<int, double> gainFeedback(WordAgent * pWordAgent, const Sentence & sentence, const  std::vector<int> & father);
	void gainSentenceInfor(const Sentence & sentence,const  std::vector<int> & father);
	int getAntigenNum();
	void setAntigenNum();
	bool initAntigenNum();
	int getBcellNum();
	int getLocalAgentsNum(std::pair<int,int> pos);
	void setLocalAgentsNum(std::pair<int,int> pos);
	std::vector<std::vector<int> > getGrid();
	bool setGrid(bool isIncrease, const std::pair<int,int> & position);
	void testSub(int a);
	void setSentence(const Sentence & sentence);
	void setFather(const std::vector<int> & father);
	std::vector<int> getFather();

	std::vector<double> getFeatureWeights();

	bool updateFeatureWeights(std::map<int, double> & fweight);

        /*state number of cells*/
        bool increaseBcellNum();
        bool decreaseBcellNum();
        bool increaseAntigenNum();
        bool decreaseAntigenNum(int id);

        bool getFeedback();
        bool setFeedback(bool fb);

        int getAntigenQuantity();
        bool setAntigenQuantity(int quantity);

        Sentence getSentence();

        bool getFeedbackFlag();
        bool setFeedbackFlag(bool flag);

        bool initAgID();
        bool addAgID(const int & ID);


        int getSentenceID();
        bool setSentenceID(int id);

        bool setWordAgentStatus(int status, std::pair<int,int> & position,int agentID);
        bool setAntigenID(int id, std::pair<int,int> & position,int agentID);
        bool setWordAgentSentence(const Sentence & sentence, int sentenceID,std::pair<int,int> & position,int agentID);
        bool setWordAgentAgReceptor(std::vector<int> & rec,std::pair<int,int> & position,int agentID);
        bool setWordAgentAffinity(double affinity,std::pair<int,int> & position,int agentID);
        bool removeAntigen();/*remove antigens that can not be killed at this response*/

        int getWordAgentStatus(std::pair<int,int> & position,int agentID);

        bool getSecondResponseFlag();

        bool updateReceptor();

private:
	void _initGrid();
	int _calcSub(const std::pair<int, int> & pos) const;
};



#endif
