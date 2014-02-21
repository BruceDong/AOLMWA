#include <iostream>

#include <cstdlib>
#include <ctime>

#include "WordAgent.hpp"
#include "Parameter.hpp"
#include "Evaluation.hpp"

using namespace std;

Environment::Environment(int r, int c, Evaluation * evaluation, Model *model)
		: rows(r), cols(c)
{
	resetAgents();
	eva = evaluation;
	mod = model;
	pAntigenNum = 0;
	pBcellNum = 0;
	agentId = 0;
	_initGrid();
	senID = 0;

	badrow.clear();
	badcol.clear();

	agentNum = 0;
	isSecondResponse = false;
}

int Environment::_calcSub(const pair<int, int> & pos) const
{
	return pos.first * cols + pos.second;
}

bool Environment::addPWordAgent(WordAgent & pWordAgent)
{
        if(pWordAgent.getAgentID() == 0)
        {
                agentId++;
                pWordAgent.setAgentID(agentId);
                pWordAgents[_calcSub(pWordAgent.getPosition())].insert(map<int,WordAgent>::value_type(agentId,pWordAgent));
        }
        else
        {
                pWordAgents[_calcSub(pWordAgent.getPosition())].insert(map<int,WordAgent>::value_type(pWordAgent.getAgentID(),pWordAgent));
        }
	return true;
}

bool Environment::addPWordAgent(WordAgent & pWordAgent, int category)
{
       if(pWordAgent.getAgentID() == 0)
        {
                agentId++;
                pWordAgent.setAgentID(agentId);
                pWordAgent.setCategory(category);
                pWordAgents[_calcSub(pWordAgent.getPosition())].insert(map<int,WordAgent>::value_type(agentId,pWordAgent));
        }
        else
        {
                pWordAgent.setCategory(category);
                pWordAgents[_calcSub(pWordAgent.getPosition())].insert(map<int,WordAgent>::value_type(pWordAgent.getAgentID(),pWordAgent));
        }
	return true;
}

bool Environment::delPWordAgent(WordAgent & pWordAgent)
{
	pWordAgents[_calcSub(pWordAgent.getPosition())].erase(pWordAgent.getAgentID());
	return true;
}

bool Environment::resetAgents()
{
	pWordAgents.clear();
	pWordAgents.resize(rows * cols, map<int,WordAgent>());
	return true;
}

int Environment::agentCount(const pair<int, int> & pos)
{
	return pWordAgents[_calcSub(pos)].size();
}

bool Environment::getNearbyAgents(const int id, const pair<int,int> & pos,
		vector<WordAgent> & neabyAgents) const
{
	neabyAgents.clear();
	const map<int,WordAgent> & near = pWordAgents[_calcSub(pos)];
	std::map<int,WordAgent>::const_iterator it = near.begin();
	for(; it != near.end(); it++){
		if((*it).first != id){
			neabyAgents.push_back((*it).second);
		}
	}
	return true;
}

pair<int, int> Environment::getRandomPosition()
{
        int row = rand() % rows;
        int col = rand() % cols;

	return make_pair(row, col);
}

bool Environment::xInRange(int x)
{
	return x >= 0 && x < rows;
}

bool Environment::yInRange(int y)
{
	return y >= 0 && y < cols;
}

bool Environment::update(WordAgent * pWordAgent)
{
	return true;
}

std::pair<int, double> Environment::gainFeedback(WordAgent * pWordAgent, const Sentence & sentence, const  std::vector<int> & father)
{
                vector<int> fa = father;
		if(father.size() == 0)
		{
			int a;
			cout<<"status is "<<pWordAgent->getStatus()<<endl;
			cout<<"father is null in environment "<<endl;
			vector<int> f = pWordAgent->getFather();
			cout<<"agent's father size is "<<f.size()<<endl;
			vector<int> ag;
			ag = pWordAgent->getAgReceptor();
			cout<<"ag size is "<<ag.size()<<endl;
			cin>>a;
		}
		return eva->calFeedback(sentence,pWordAgent,fa);
}

std::vector<std::map<int, WordAgent> > Environment::getAgents()
{
	return pWordAgents;
}

void Environment::gainSentenceInfor(const Sentence & sentence, const std::vector<int> & father)
{
	//sen = sentence;
	//fa  = father;
}

int Environment::getAntigenNum()
{
        return pAntigenNum;
}

int Environment::getBcellNum()
{
        return pBcellNum;
}

void Environment::_initGrid()
{
        vector<int> v;
        v.resize(COLS);
        for(size_t i = 0; i < ROWS; i++)
        {
                grid.push_back(v);
        }
}

int Environment::getLocalAgentsNum(std::pair<int,int> pos)
{
        return grid[pos.first][pos.second];
}
void Environment::setLocalAgentsNum(std::pair<int,int> pos)
{
        grid[pos.first][pos.second]++;
        if(grid[pos.first][pos.second] == MAXNUMAGENT)
        {
                badrow.push_back(pos.first);
                badcol.push_back(pos.second);
        }
}

std::vector<vector<int> > Environment::getGrid()
{
        return grid;
}

bool Environment::setGrid(bool isIncrease, const std::pair<int,int> & position)
{
        if(isIncrease)
        {
                grid[position.first][position.second]++;
        }
        else
        {
                grid[position.first][position.second]--;
        }
        return true;
}

void Environment::testSub(int a)
{
        for(int i = 0; i < a; i++)
        {
                for(int j = 0; j < a; j++)
                {
                        cout<<_calcSub(make_pair(i,j))<<" ";
                }
                cout<<endl;
        }
        cout<<endl;
}

void Environment::setAntigenNum()
{
        this->pAntigenNum--;
}

bool Environment::initAntigenNum()
{
        pAntigenNum = 0;
        return true;
}

void Environment::setSentence(const Sentence & sentence)
{
        sen = sentence;
}

void Environment::setFather(const vector<int> & father)
{
        fa = father;
}

std::vector<int> Environment::getFather()
{
        return fa;
}

std::vector<double> Environment::getFeatureWeights()
{
        return mod->getFeatureWeight();
}

bool Environment::updateFeatureWeights(std::map<int, double> & fweight)
{
        mod->updateFeatureWeight(fweight);
        return true;
}

bool Environment::increaseBcellNum()
{
        pBcellNum++;
        return true;
}
bool Environment::decreaseBcellNum()
{
        pBcellNum--;
        return true;
}

bool Environment::increaseAntigenNum()
{
        pAntigenNum++;
        return true;
}
bool Environment::decreaseAntigenNum(int id)
{
        vector<int>::iterator it = vAgID.begin();
        for(; it != vAgID.end(); it++)
        {
                if(*it == id)
                {
                        pAntigenNum--;
                        break;
                }
        }
        return true;
}

bool Environment::getFeedback()
{
        return feedback;
}
bool Environment::setFeedback(bool fb)
{
        feedback = fb;
        return true;
}

int Environment::getAntigenQuantity()
{
        return antigenQuantity;
}

bool Environment::setAntigenQuantity(int quantity)
{
        antigenQuantity = quantity;
        return true;
}

Sentence Environment::getSentence()
{
        return sen;
}

bool Environment::getFeedbackFlag()
{
        return havePositiveFeedback;
}
bool Environment::setFeedbackFlag(bool flag)
{
        havePositiveFeedback = flag;
        return true;
}

bool Environment::addAgID(const int & ID)
{
        vAgID.push_back(ID);
        return true;
}

bool Environment::initAgID()
{
        vAgID.clear();
        return true;
}

int Environment::getSentenceID()
{
        return senID;
}

bool Environment::setSentenceID(int id)
{
        senID = id;
        return true;
}

bool Environment::setWordAgentStatus(int status, pair<int,int> & position, int agentID)
{
        map<int,WordAgent>::iterator it = pWordAgents[_calcSub(position)].find(agentID);
        if(it != pWordAgents[_calcSub(position)].end())
        {
                it->second.setStatus(status);
        }
        return true;
}

int Environment::getWordAgentStatus(pair<int,int> & position,int agentID)
{
        int status = -1;
        map<int,WordAgent>::iterator it = pWordAgents[_calcSub(position)].find(agentID);
        if(it != pWordAgents[_calcSub(position)].end())
        {
                return it->second.getStatus();
        }
        return status;
}

bool Environment::getSecondResponseFlag()
{
        return isSecondResponse;
}

bool Environment::setAntigenID(int id, std::pair<int,int> & position,int agentID)
{
        map<int,WordAgent>::iterator it = pWordAgents[_calcSub(position)].find(agentID);
        if(it != pWordAgents[_calcSub(position)].end())
        {
                it->second.setAGID(id);
        }
        return true;
}
bool Environment::setWordAgentSentence(const Sentence & sentence, int sentenceID,std::pair<int,int> & position,int agentID)
{
        map<int,WordAgent>::iterator it = pWordAgents[_calcSub(position)].find(agentID);
        if(it != pWordAgents[_calcSub(position)].end())
        {
                it->second.setSentence(sentence,sentenceID);
        }
        return true;
}

bool Environment::setWordAgentAgReceptor(std::vector<int> & rec,std::pair<int,int> & position,int agentID)
{
        map<int,WordAgent>::iterator it = pWordAgents[_calcSub(position)].find(agentID);
        if(it != pWordAgents[_calcSub(position)].end())
        {
                it->second.setAgReceptor(rec);
        }
        return true;
}

bool Environment::setWordAgentAffinity(double affinity,std::pair<int,int> & position,int agentID)
{
        map<int,WordAgent>::iterator it = pWordAgents[_calcSub(position)].find(agentID);
        if(it != pWordAgents[_calcSub(position)].end())
        {
                it->second.setAffinity(affinity);
        }
        return true;
}

bool Environment::removeAntigen()
{
        cout<<"removing antigens that are not killed!"<<endl;
        for(size_t i = 0; i < pWordAgents.size(); i++)
        {
                map<int,WordAgent>::iterator it = pWordAgents[i].begin();
                while(it != pWordAgents[i].end())
                {
                        if((it->second.getStatus() != DIE) && (it->second.getCategory() == ANTIGEN))
                        {
                                it->second.setStatus(DIE);
                        }
                        it++;
                }
        }

        cout<<"removing finished!"<<endl;
        return true;
}


bool Environment::updateReceptor()
{
        for(size_t i = 0; i < pWordAgents.size(); i++)
        {
                map<int, WordAgent>::iterator it = pWordAgents[i].begin();
                while(it != pWordAgents[i].end())
                {
                        it->second.updateSelf();

                }
        }

        return true;
}




