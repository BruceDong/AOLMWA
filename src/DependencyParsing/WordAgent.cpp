#include <vector>
#include <iostream>
#include <time.h>
#include <cassert>
#include <ctime>
#include <cstdlib>
#include <math.h>
#include <iomanip>

#include <algorithm>

#include "Parameter.hpp"
#include "WordAgent.hpp"

using namespace std;



WordAgent::WordAgent(int id, Environment * environment,Simulator * simulator,
		const pair<int, int> & pos, int cat, int con){
	ID = id;
	env = environment;
	position = pos;

	category = cat;
	concentration = con;
	status = ACTIVE;

	stimulus = 0.0;
	suppression = 0.0;

	agAffinity = 0.0;

	AgentID = 0;
	simu = simulator;

	mapStatusToBehavior();
}

bool WordAgent::addDomFeature(const vector<int> & feature)
{
	for(size_t i = 0; i < feature.size(); i++){

		domFeature.insert(make_pair(feature[i],0.0));
	}
	return true;
}

bool WordAgent::addRecFeature(const vector<int> & feature)
{
	for(size_t i = 0; i < feature.size(); i++){
		recFeature.push_back(feature[i]);
	}
	return true;
}

int WordAgent::getID()
{
	return ID;
}

bool WordAgent::run()
{
	bool hasRun = false;

        if(orders.size())
        {
                int now = orders.front();
                orders.pop();
                switch(now)
                {
                        case MOVING:
                                _doMove();
                                break;
                        case INTERACTING:
                                _interact();
                                break;
                        case MUTATING:
                                _mutate();
                                break;
                        case SELECTING:
                                _select();
                                break;
                        case CLONING:
                                _clone();
                                break;
                        case REGULATING:
                                _regulate();
                                break;
                        case DYING:
                                _die();
                                break;
                        default:
                                assert(0);
                }
                hasRun = true;
        }
	return hasRun;
}

bool WordAgent::_doMove()
{

        if(status != ACTIVE)
        {
                _mapStatusToBehavior();
                return false;
        }

	static const int dx[] = {0, 1, 1, 1, 0, -1, -1, -1};
	static const int dy[] = {1, 1, 0, -1, -1, -1, 0, 1};
	int min = simu->agentCount(position);
	vector<pair<int, int> > pos;
	for(int k = 0; k < 8; k++){
		int x = position.first + dx[k];
		int y = position.second + dy[k];
		pair<int, int> newPos = make_pair(x, y);
		if(env->xInRange(x) && env->yInRange(y)){
			if(simu->agentCount(newPos) < min){
				min = simu->agentCount(newPos);
				pos.clear();
			}
			if(simu->agentCount(newPos) == min){
				pos.push_back(newPos);
			}
		}
	}

	orders.push(INTERACTING);
	if(min == simu->agentCount(position)){

		return false;
	}

        pair<int, int> oldPos = position;
	int p = rand() % pos.size();
	position = pos[p];

	simu->addWordAgent(*this);
	position = oldPos;
	simu->deleteWordAgent(*this);
	return true;
}

pair<int, int> WordAgent::getPosition() const
{
	return position;
}

bool WordAgent::_mutate()
{
	if(status == MATCH)
	{
	        tmpFeature = domFeature;
		/*multi-point mutation*/
		double mutatedProb = 0.0;
		mutatePosition.clear();

		for(size_t i = 0; i < agFeature.size(); i++)
		{
		    /*producting mutated probability*/
		    int r = rand()%1000;
		    mutatedProb = (double)r/1e6;
		    double mp = exp(-agAffinity*1e6)*MUTATEPRO;

		    if(mutatedProb < mp )
		    {
		            mutatePosition.push_back(agFeature[i]);
		    }
		}

		if((int)mutatePosition.size() > 0)
		{
		        double sum = 0.0;
			int r = rand()%1000000;
			double deta = (double)r/1e-7;

			if(agAffinity == 0.0)
			{
				for(size_t j = 0; j < mutatePosition.size();j++)
				{
					tmpFeature[mutatePosition[j]] = deta;
					sum += deta;
				}
			}
			else
			{
				for(size_t j = 0; j < mutatePosition.size(); j++)
				{
					if(domFeature[mutatePosition[j]] != 0.0)
					{
						deta = domFeature[mutatePosition[j]]/agAffinity;
						tmpFeature[mutatePosition[j]] = domFeature[mutatePosition[j]] + deta;
						sum += deta;
					}
					else
					{
						tmpFeature[mutatePosition[j]] = deta;
						sum += deta;
					}
                                }

			}

                        /*calculating difference of affinity after mutation*/
                        mutatedAffinity = agAffinity + sum;
                        setStatus(MUTATE);
		}
	}

	mapStatusToBehavior();

	return true;
}

double WordAgent::_calAffinity(std::vector<int> & receptor, int & matchSize)
{
	/*calculating affinity*/
	matchSize = 0;
	double sum = 0.0;
        std::map<int, double>::iterator it;
        for(size_t i = 0; i < receptor.size(); i++)
        {
                it = domFeature.find(receptor[i]);
                if(it != domFeature.end())
                {
                        matchSize++;
                        sum += domFeature[receptor[i]];
                }
        }
	return sum;
}

double WordAgent::_calAffinity(std::map<int,double> & domReceptor, int & matchSize)
{
        /*calculating affinity*/
	matchSize = 0;
	double sum = 0.0;
        std::map<int, double>::iterator it;
        for(size_t i = 0; i < recFeature.size(); i++)
        {
                it = domReceptor.find(recFeature[i]);
                if(it != domReceptor.end())
                {
                        matchSize++;
                        sum += domReceptor[recFeature[i]];
                }
        }
	return sum;

}

double WordAgent::calAffinity(const std::vector<int> & receptor, std::map<int,double> & domF,int & matchSize)
{
        /*calculating affinity*/
	matchSize = 0;
	double sum = 0.0;
        std::map<int, double>::iterator it;
        for(size_t i = 0; i < receptor.size(); i++)
        {
                it = domF.find(receptor[i]);
                if(it != domF.end())
                {
                        matchSize++;
                        sum += domF[receptor[i]];
                }
        }
	return sum;
}

double WordAgent::_calStimulusByBcell(std::vector<int> receptor)
{
	double sum = 0.0;
	if(status != DIE)
	{
		std::map<int, double>::iterator it;
		for(size_t i = 0; i < receptor.size(); i++)
		{
			it = domFeature.find(receptor[i]);
			if(it != domFeature.end())
			{
				sum += domFeature[receptor[i]];
			}
		}
	}

	return sum;
}

double WordAgent::_calMutatedAffinity(std::vector<int> receptor)
{
	/*calculating affinity after mutation*/
	double sum = 0.0;
	std::map<int, double>::iterator it;

	for(size_t i = 0; i < receptor.size(); i++)
	{
		it = tmpFeature.find(receptor[i]);
		if(it != tmpFeature.end())
		{
			sum += tmpFeature[receptor[i]];
		}
	}


	return sum;
}

int WordAgent::getStatus()
{
	return status;
}

void WordAgent::setStatus(int s)
{
	status = s;
}

bool WordAgent::_interact()
{
	/*interaction between word-agents:
	(1) Antigens and B cells;
	(2) B cells
	*/
	updateSelf();
	if(status != ACTIVE)
	{
		return false;
	}
	simu->interact(*this);
	return true;
}

bool WordAgent::_clone()
{
	if(status == MATURE)
	{
		if(!_getRegulation())
		{
			std::cerr<<"Gain regulation failed!"<<std::endl;
		}

		int N = 0;
		if(stimulus > suppression)
		{
			N +=1;
		}
		else if (stimulus < suppression)
		{
			double diff = suppression - stimulus;

			if(diff > mutatedAffinity)
				N -=2;
			else
				N -=1;
		}

		if(N > 0)
		{
			WordAgent wa(ID,env,simu,position,MEMORYBCELL,1);
			wa.setAgentID(AgentID);
			wa.setAgReceptor(agFeature);
			wa.setAffinity(mutatedAffinity);
			wa.mapStatusToBehavior();
                        for(int i = 0; i < N; i++)
			{
			        simu->addWordAgent(*this);
			}
		}
		else
		{
			int M = -N;
			if(M > 1)
			simu->regulateByDelete(*this,N);
		}
	}

        setStatus(ACTIVE);
	mapStatusToBehavior();

	return true;
}

bool WordAgent::_regulate()
{

	/*regulating by idiotype immune network of B cells as equa:
	N = concentration + stimulus - suppression
	*/
	if((category == BCELL))
	{
                if(!_getRegulation())
                {
                        std::cerr<<"Gain regulation failed!"<<std::endl;
                }


                int con = _calConcentration();
                int N = con * (int)(stimulus - suppression);
                if(N > 0)
                {
                        /*cloning (stimulus)*/
                        for(int i = 0; i < N; i++)
                        {
                                /*new agent by cloning: status is ACTIVE*/
                                simu->addWordAgent(*this);
                        }

                }
                else if(N < 0)
                {
                        std::vector<WordAgent> nearAgents;
                        int M = -N;
                        simu->regulateByDelete(*this,M);
                }

                setStatus(ACTIVE);
                mapStatusToBehavior();
	}

	return true;
}

std::vector<int> WordAgent::getRecReceptor() const
{
	return recFeature;
}

bool WordAgent::_select()
{
	simu->select(*this);

	return true;
}

bool WordAgent::_calFeedback(WordAgent & wordAgent)
{
	/*calculating feedback based on mutated receptors*/
	feedback = env->gainFeedback(&wordAgent,wordAgent.getSentence(),wordAgent.getFather());
	if(feedback.first > 0)
	{
	        if((wordAgent.getSentenceID() == env->getSentenceID()) && (feedback.second > ACCURACYTHRESHOLD))
	        {
                        env->setFeedbackFlag(true);

	        }

                return true;
	}

	return false;
}

double WordAgent::getAgAffinity()
{
	return agAffinity;
}

bool WordAgent::setAffinity(double affinity)
{
        agAffinity = affinity;
        return true;
}

bool WordAgent::_die()
{
        if(simu->deleteWordAgent(*this))
        {
                return true;
        }
	return false;
}

double WordAgent::_calSuppressByBcell(std::map<int, double> & receptor)
{

	double sum = 0.0;
	map<int, double>::iterator it;
	for(size_t i = 0; i < recFeature.size(); i++)
	{
		it = receptor.find(recFeature[i]);
		if(it != receptor.end())
		{
			sum += receptor[recFeature[i]];
		}
	}

	return sum;
}

std::map<int, double> WordAgent::getDomReceptor()
{
	return domFeature;
}

std::map<int, double> WordAgent::getTmpReceptor()
{
	return tmpFeature;
}

bool WordAgent::updateSelf()
{
	/*updating information of self receptor including:
	domFeature
	*/
	if(category == BCELL)
	{
                int fid = -1;
                vector<double> fWeight = env->getFeatureWeights();
                for(map<int,double>::iterator it = domFeature.begin(); it != domFeature.end(); it++)
                {
                        fid = it->first;
                        if(fWeight[fid] != it->second)
                        {
                                it->second = fWeight[fid];
                        }
                }
	}

	return true;
}



int WordAgent::getCategory()
{
	return category;
}

void WordAgent::setCategory(int cat)
{
        category = cat;
}


bool WordAgent::_getRegulation()
{
	stimulus = 0.0;
	suppression = 0.0;
	vector<WordAgent> nearAgents;
	if(simu->getNearbyAgents(ID, position,nearAgents))
	{
		double affinity = 0.0;

		for(size_t i = 0; i < nearAgents.size(); i++)
		{
			if(nearAgents[i].getStatus() != DIE)
			{
				if(nearAgents[i].getCategory() == BCELL)
				{
					affinity = _calStimulusByBcell(nearAgents[i].getRecReceptor());
					stimulus += affinity;
					map<int,double> dom = nearAgents[i].getDomReceptor();
					affinity = _calSuppressByBcell(dom);
					suppression += affinity;
				}
			}
		}
	}

	return true;
}

double WordAgent::getMutatedAffinity()
{
	return mutatedAffinity;
}

int WordAgent::getConcentration()
{
	return _calConcentration();
}

int WordAgent::_calConcentration()
{
	concentration = simu->calConcentration(this);

	return concentration;
}

void WordAgent::updateConcentration()
{
	/*gain neighbour*/
	int sum = 0;
	vector<WordAgent > nearAgents;
	if(env->getNearbyAgents(AgentID, position, nearAgents))
	{
		for(size_t i = 0; i < nearAgents.size(); i++)
		{
			if(nearAgents[i].getStatus() != DIE)
			{
				if(ID == nearAgents[i].getID())
				{
					sum++;
				}
			}
		}
	}
	concentration = sum;
}

void WordAgent::_mapStatusToBehavior()
{
	if(category == ANTIGEN)
	{
		/*selecting behavior according to status of antigen*/
		switch(status)
		{
			case ACTIVE:
				orders.push(MOVING);
				break;
			case DIE:
				orders.push(DYING);
				break;
			default:
				assert(0);
		}
	}
	else
	{
		/*selecting behavior according to status of B cell*/
		switch(status)
		{
			case ACTIVE:
                                orders.push(MOVING);
				break;
			case MATCH:
				orders.push(MUTATING);
				break;
			case MUTATE:
				orders.push(SELECTING);
				break;
			case MATURE:
				orders.push(CLONING);
				break;
			case DIE:
				orders.push(DYING);
				break;
                        case REGULATE:
				orders.push(REGULATING);
				break;
			default:
				assert(0);
		}
	}
}

void WordAgent::setPosition(std::pair<int,int> p)
{
        position.first = p.first;
        position.second = p.second;
}

bool WordAgent::setDomReceptor(std::map<int, double> & rec)
{


        for(map<int, double>::iterator it = rec.begin(); it != rec.end(); it++)
        {
                domFeature.insert(make_pair(it->first, it->second));
        }
        return true;
}

bool WordAgent::setRecReceptor(std::vector<int> & rec)
{
        for(size_t i = 0; i < rec.size(); i++)
        {
                recFeature.push_back(rec[i]);
        }
        return true;
}

bool WordAgent::initDomReceptor(std::map<int, double> & rec)
{
        domFeature = rec;
        return true;
}
bool WordAgent::initRecReceptor(std::vector<int> & rec)
{
        recFeature = rec;
        return true;
}

int WordAgent::getAGID()
{
        return AGID;
}

bool WordAgent::setAGID(int id)
{
        AGID = id;
        return true;
}

bool WordAgent::setAgentID(const int id)
{
        AgentID = id;
        return true;
}
int WordAgent::getAgentID()
{
        return AgentID;
}

Sentence WordAgent::getSentence()
{
        return sen;
}

bool WordAgent::setSentence(const Sentence & sentence, int sentenceID)
{
        sen.clear();
        sen = sentence;
        senID = sentenceID;
        return true;
}

int WordAgent::getSentenceID()
{
        return senID;
}

std::vector<int> WordAgent::getMemoryBcellReceptor()
{
        return memoryFeature;
}

bool WordAgent::setMemoryBcellReceptor(std::vector<int> & memRec)
{
        memoryFeature = memRec;
        return true;
}

std::vector<int> WordAgent::getAntibodyReceptor()
{
        return antibodyFeature;
}

bool WordAgent::setAntibodyReceptor(std::vector<int> & antibodyRec)
{
        antibodyFeature = antibodyRec;
        return true;
}

bool WordAgent::isSame(std::vector<int> & s, std::vector<int> & d)
{
        if(s.size() != d.size())
        {
                return false;
        }

        for(size_t i = 0; i < s.size(); i++)
        {
                if(s[i] != d[i])
                {
                        return false;
                }
        }
        return true;
}

bool WordAgent::_calFeedback()
{

        /*calculating feedback based on mutated receptors*/
	feedback = env->gainFeedback(this,sen,father);
	if(feedback.first > 0)
	{
	        cout<<"ag number is "<<env->getAntigenNum()<<endl;

	        if((senID == env->getSentenceID()) && (feedback.second > ACCURACYTHRESHOLD))
	        {
                        env->setFeedbackFlag(true);
	        }
	        cout<<"sentence "<<senID<<" positive feedback! "<<endl;
	        vector<map<int, WordAgent> >  agents = env->getAgents();

                return true;
	}

        return false;
}

std::vector<int> WordAgent::getAgReceptor()
{
   return agFeature;
}

bool WordAgent::setAgReceptor(std::vector<int> & rec)
{
        agFeature.resize(rec.size());
        agFeature = rec;
        return true;
}

double WordAgent::calAffinity(const std::vector<int> & agRec, int &matchSize)
{
        /*calculating affinity*/
	matchSize = 0;
	double sum = 0.0;
        std::map<int, double>::iterator it;
        for(size_t i = 0; i < agRec.size(); i++)
        {
                it = domFeature.find(agRec[i]);
                if(it != domFeature.end())
                {
                        matchSize++;
                        sum += domFeature[agRec[i]];
                }
        }
	return sum;
}
double WordAgent::calAffinity( std::map<int,double> & bRec, int & matchSize)
{
      /*calculating affinity*/
	matchSize = 0;
	double sum = 0.0;
        std::map<int, double>::iterator it;
        for(size_t i = 0; i < recFeature.size(); i++)
        {
                it = bRec.find(recFeature[i]);
                if(it != bRec.end())
                {
                        matchSize++;
                        sum += bRec[recFeature[i]];
                }
        }
	return sum;
}

void    WordAgent::mapStatusToBehavior()
{
        while(orders.size())
        {
                orders.pop();
        }

        if(category == ANTIGEN)
	{
		/*selecting behavior according to status of antigen*/
		switch(status)
		{
			case ACTIVE:
				orders.push(MOVING);
				break;
			case DIE:
				orders.push(DYING);
				break;
			default:
				assert(0);
		}
	}
	else
	{
		/*selecting behavior according to status of B cell*/
		switch(status)
		{
			case ACTIVE:
                                orders.push(MOVING);
				break;
			case MATCH:
				orders.push(MUTATING);
				break;
			case MUTATE:
				orders.push(SELECTING);
				break;
			case MATURE:
				orders.push(CLONING);
				break;
			case DIE:
				orders.push(DYING);
				break;
                        case REGULATE:
				orders.push(REGULATING);
				break;
			default:
				assert(0);
		}
	}
}

bool WordAgent::calFeedback()
{
        feedback = env->gainFeedback(this,sen,father);
	if(feedback.first > 0)
	{
                if((senID == env->getSentenceID()) && (feedback.second > ACCURACYTHRESHOLD))
	        {
                        env->setFeedbackFlag(true);
	        }

                return true;
	}

        return false;
}


std::vector<int> WordAgent::getMutatePosition()
{
        return mutatePosition;
}

void WordAgent::setFather(std::vector<int> & fa)
{

        father = fa;
	if(father.size() == 0)
	{
		cout<<"father is null in agent"<<endl;
		int s;
		cin>>s;
	}

}
std::vector<int> WordAgent::getFather()
{
        return father;
}

void WordAgent::printReceptor()
{
        map<int,double>::iterator it = domFeature.begin();
        for(; it != domFeature.end();it++)
        {
                if(it->second > 0.1)
                {
                        cout<<"("<<it->first<<","<<it->second<<")"<<" ";
                }
        }
        cout<<"*";
}

bool WordAgent::insertLocalAgents(int agentID)
{
        localInteractFlag.push_back(agentID);
        return true;
}

bool WordAgent::setLocalAgents(int agentID)
{
        return true;
}

bool WordAgent::resetLocalAgents()
{
        localInteractFlag.clear();
        return true;
}

bool WordAgent::haveInteracted(int agentID)
{

        if(localInteractFlag.size() == 0)
        {
                localInteractFlag.push_back(agentID);
                return false;
        }

        vector<int>::iterator it = find(localInteractFlag.begin(),localInteractFlag.end(), agentID);

        if(it == localInteractFlag.end())
        {
                localInteractFlag.push_back(agentID);
                return false;
        }

        return true;
}

bool WordAgent::isMemory()
{
	return isMem;
}

void WordAgent::setMemory(bool f)
{
	isMem = f;
}
