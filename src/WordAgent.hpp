#ifndef __WORD_AGENT__
#define __WORD_AGENT__

#include <set>
#include <map>
#include <queue>
#include <string>
#include <vector>

#include "Simulator.hpp"
#include "Environment.hpp"

class Simulator;

class WordAgent{
private:
        int ID;
	int AGID;
	int senID;
	int status;
	int AgentID;
	int category;
	int concentration;

	bool isMem;
	bool isInteractedWithAntigen;

	double stimulus;
	double suppression;
	double agAffinity;
	double domAffinity;
	double recAffinity;
	double mutatedAffinity;

	std::vector<int> localInteractFlag;

	std::queue<int> orders;

	std::pair<int, int> position;
	std::pair<int, double> feedback;

	std::map<int, double> domFeature;
	std::map<int, double> tmpFeature;

	std::vector<int> father;
	std::vector<int> recFeature;
	std::vector<int> agFeature;
	std::vector<int> mutatePosition;
	std::vector<int> memoryFeature;
	std::vector<int> antibodyFeature;

	Sentence sen;
	Simulator * simu;
	Environment * env;

public:
	WordAgent(int id,
			Environment * environment,
			Simulator * simulator,
			const std::pair<int, int> & pos, int cat, int con);
        /*running agents*/
	bool    run();
	bool    calFeedback();
        void    mapStatusToBehavior();

	/*operation on id*/
	int     getID();
	int     getAGID();
	bool    setAGID(int id);
	int     getAgentID();
	bool    setAgentID(const int id);

	/*adding features(attributes of word agents)*/
	bool    addDomFeature(const std::vector<int> & feature);
	bool    addRecFeature(const std::vector<int> & feature);

	/*getting category*/
	int getCategory();
	void setCategory(int cat);

	/*operation on status*/
	int     getStatus();
	void    setStatus(int s);

	/*getting affinity*/
	double getAgAffinity();
	bool setAffinity(double affinity);
	double getMutatedAffinity();

	/*operation on position*/
	std::pair<int, int> getPosition() const;
	void setPosition(std::pair<int,int> p);

	/*operation on receptor*/
	void printReceptor();
	bool initDomReceptor(std::map<int, double> & rec);
	bool setDomReceptor(std::map<int, double> & rec);
	bool initRecReceptor(std::vector<int> & rec);
	bool setRecReceptor(std::vector<int> & rec);
	bool setAgReceptor(std::vector<int> & rec);

        std::vector<int>        getRecReceptor() const;
        std::vector<int>        getAgReceptor();
        std::map<int, double>   getDomReceptor();
	std::map<int, double>   getTmpReceptor();

	/*operation on concentration*/
	int     getConcentration();
	void    updateConcentration();

	/*operation on sentence*/
	Sentence getSentence();
	int getSentenceID();
	bool setSentence(const Sentence & sentence, int sentenceID);

	/*operation on memory B cell recepors*/
	std::vector<int> getMemoryBcellReceptor();
	bool setMemoryBcellReceptor(std::vector<int> & memRec);

	/*operation on Antibody recepors*/
	std::vector<int> getAntibodyReceptor();
	bool setAntibodyReceptor(std::vector<int> & antibodyRec);

	/*comparing two vectors*/
	bool isSame(std::vector<int> & s, std::vector<int> & d);

        /*updating self receptors*/
	bool    updateSelf();

	/*calculating affinity*/
	std::vector<int> getMutatePosition();
	double calAffinity(const std::vector<int> & agReceptor, int &matchSize);
	double calAffinity(std::map<int,double> & bReceptor, int &matchSize);
        double calAffinity(const std::vector<int> & receptor,
	                  std::map<int,double> & domF,int & matchSize);

	/*operating on fathers of words*/
	std::vector<int> getFather();
	void setFather(std::vector<int> & fa);

        /*operating on agents*/
	bool isMemory();
	void setMemory(bool f);
	bool resetLocalAgents();
	bool setLocalAgents(int agentID);
	bool haveInteracted(int agentID);
	bool insertLocalAgents(int agentID);

private:
        /*Behaviors*/
	bool _doMove();
	bool _interact();
	bool _mutate();
	bool _select();
	bool _clone();
	bool _regulate();
	bool _die();

        /*stating concentration*/
	int     _calConcentration();

	/*getting information on regulation*/
	bool    _getRegulation();

	/*calculating feedback*/
	bool    _calFeedback();
	bool    _calFeedback(WordAgent & wordAgent);

	/*mapping status to behaviors*/
	void    _mapStatusToBehavior();

	/*calculating affinity*/
	double  _calAffinity(std::vector<int> & receptor, int & matchSize);
	double  _calAffinity(std::map<int,double> & domReceptor, int & matchSize);
	double  _calMutatedAffinity(std::vector<int> receptor);
	double  _calSuppressByBcell(std::map<int, double> & receptor);
	double  _calStimulusByBcell(std::vector<int> receptor);

};

#endif
