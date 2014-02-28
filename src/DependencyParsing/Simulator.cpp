#include <vector>
#include <iostream>
#include <time.h>

#include "Simulator.hpp"
#include "Parameter.hpp"

using namespace std;

Simulator::Simulator(Environment * environment,Evaluation * evaluation)
{
        rows = ROWS;
        cols = COLS;
        agentId = 0;
	env = environment;
	times = 1;
	resetAgents();
	agNum = 0;
	eva = evaluation;

}
bool Simulator::resetAgents()
{
        vWordAgents.clear();
	vWordAgents.resize(rows * cols, map<int,WordAgent>());
	return true;
}
int Simulator::_calcSub(const pair<int, int> & pos) const
{
	return pos.first * cols + pos.second;
}

bool Simulator::_isSame(const std::vector<int> & s, const std::vector<int> & d)
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

bool Simulator::addWordAgent(WordAgent & pWordAgent)
{
        if(pWordAgent.getAgentID() == 0)
        {
                if(pWordAgent.getCategory() == ANTIGEN)
                {
                        agNum++;
                }
                agentId++;
                pWordAgent.setAgentID(agentId);
                vWordAgents[_calcSub(pWordAgent.getPosition())].insert(map<int,WordAgent>::value_type(agentId,pWordAgent));
        }
        else
        {
		agentId++;

               bool f = vWordAgents[_calcSub(pWordAgent.getPosition())].insert(map<int,WordAgent>::value_type(agentId,pWordAgent)).second;

		return f;
        }
	return true;
}

bool Simulator::deleteWordAgent(WordAgent & pWordAgent)
{
        pWordAgent.setStatus(DIE);

        return true;
}

bool Simulator::interact(WordAgent & wa)
{
        int cate = wa.getCategory();
        int posi = _calcSub(wa.getPosition());

        if(cate == BCELL)
        {
                /*interact with Antigen agents*/
                map<int,WordAgent>::iterator it = vWordAgents[posi].begin();
                while(it != vWordAgents[posi].end())
                {
                        if((it->second.getCategory()== ANTIGEN) && (it->second.getStatus() == ACTIVE))
                        {
                                int matchSize = 0;
                                vector<int> ag = it->second.getRecReceptor();
                                double affinity = wa.calAffinity(ag,matchSize);
                                if((matchSize > 0) && (matchSize == (int)ag.size()))
                                {
                                        /*set status*/
                                        wa.setStatus(MATCH);
                                        it->second.setStatus(DIE);
                                        _updateAgNum();

                                        /*mapping behavior*/
                                        it->second.mapStatusToBehavior();

                                        wa.setSentence(env->getSentence(),env->getSentenceID());
                                        vector<int> father = env->getFather();
                                        wa.setFather(father);
                                        wa.setAgReceptor(ag);
					wa.setAffinity(affinity);
                                        break;
                                }

                                it++;
                        }
                        else if((it->second.getCategory()==ANTIBODY) && (it->second.getStatus() == ACTIVE))
                        {
                                int matchSize = 0;
                                vector<int> ag = it->second.getRecReceptor();
                                double affinity = wa.calAffinity(ag,matchSize);
                                if((matchSize > 0) && (matchSize == (int)ag.size()))
                                {
                                        /*set status*/
                                        wa.setStatus(MATCH);
                                        it->second.setStatus(DIE);
                                        _updateAgNum();

                                        /*mapping behavior*/
                                        it->second.mapStatusToBehavior();

                                        wa.setSentence(env->getSentence(),env->getSentenceID());
                                        vector<int> father = env->getFather();
                                        wa.setFather(father);
                                        wa.setAgReceptor(ag);
					wa.setAffinity(affinity);
                                        break;
                                }

                                it++;
                        }
                        else
                        {
                                it++;
                        }
                }

                wa.mapStatusToBehavior();
        }
        else if(cate == ANTIGEN)
        {

                map<int,WordAgent>::iterator it =  vWordAgents[posi].begin();
                while(it != vWordAgents[posi].end())
                {
                        /*interact with Bcell agents*/
                        if((it->second.getCategory() == BCELL)&& (it->second.getStatus()== ACTIVE))
                        {
                                int matchSize = 0;
                                map<int,double> b = it->second.getDomReceptor();
                                double affinity = wa.calAffinity(b,matchSize);
                                vector<int> ag = wa.getRecReceptor();
                                if((matchSize > 0) && (matchSize == (int)ag.size()))
                                {
                                        /*set status*/
                                        wa.setStatus(DIE);
                                        it->second.setStatus(MATCH);
                                        /*mapping behavior*/
                                        it->second.mapStatusToBehavior();
                                        _updateAgNum();

                                        it->second.setSentence(env->getSentence(),env->getSentenceID());
					vector<int> father = env->getFather();
					it->second.setFather(father);
                                        it->second.setAgReceptor(ag);
					it->second.setAffinity(affinity);
                                        break;
                                }

                                it++;
                        }
                        else if((it->second.getCategory() == ANTIBODY)&& (it->second.getStatus()== ACTIVE))
                        {
                                vector<int> r1 = wa.getAgReceptor();
                                vector<int> r2 = it->second.getRecReceptor();

                                if(wa.isSame(r1,r2))
                                {
                                        /*set status*/
                                        wa.setStatus(DIE);
                                        it->second.setStatus(DIE);
                                        /*mapping behavior*/
                                        it->second.mapStatusToBehavior();
                                        _updateAgNum();

                                        break;
                                }

                                it++;
                        }
                        else
                        {
                                it++;
                        }
                }

                wa.mapStatusToBehavior();
        }
        else if(cate == MEMORYBCELL)
        {
                /*interact with Antigen agents*/
                map<int,WordAgent>::iterator it = vWordAgents[posi].begin();
                while(it != vWordAgents[posi].end())
                {
                        if((it->second.getCategory()== ANTIGEN) && (it->second.getStatus() == ACTIVE))
                        {
                                vector<int> r1 = wa.getAgReceptor();
                                vector<int> r2 = it->second.getRecReceptor();

                                if(wa.isSame(r1,r2))
                                {
                                        /*set status*/
                                        it->second.setStatus(DIE);
                                        _updateAgNum();

                                        /*mapping behavior*/
					wa.setStatus(DIE);
                                        it->second.mapStatusToBehavior();

                                        break;
                                }

                                it++;
                        }
                        else
                        {
                                it++;
                        }
                }


                wa.mapStatusToBehavior();
        }
        else if(cate == ANTIBODY)
        {
                /*interact with Antigen or B cell agents*/
                map<int,WordAgent>::iterator it = vWordAgents[posi].begin();
                while(it != vWordAgents[posi].end())
                {
                        if((it->second.getCategory()== ANTIGEN) && (it->second.getStatus() == ACTIVE))
                        {
                                vector<int> r1 = wa.getAgReceptor();
                                vector<int> r2 = it->second.getRecReceptor();

                                if(wa.isSame(r1,r2))
                                {
                                        /*set status*/
                                        it->second.setStatus(DIE);
                                        _updateAgNum();

                                        /*mapping behavior*/
					wa.setStatus(DIE);
                                        it->second.mapStatusToBehavior();

                                        break;
                                }

                                it++;
                        }
                        else if((it->second.getCategory()== BCELL) && (it->second.getStatus() == ACTIVE))
                        {
                                int matchSize = 0;
                                map<int,double> b = it->second.getDomReceptor();
                                double affinity = wa.calAffinity(b,matchSize);
                                vector<int> ag = wa.getRecReceptor();
                                if((matchSize > 0) && (matchSize == (int)ag.size()))
                                {
                                        /*set status*/
                                        wa.setStatus(DIE);
                                        it->second.setStatus(MATCH);
                                        /*mapping behavior*/
                                        it->second.mapStatusToBehavior();

                                        it->second.setSentence(env->getSentence(),env->getSentenceID());
					vector<int> father = env->getFather();
					it->second.setFather(father);
                                        it->second.setAgReceptor(ag);
					it->second.setAffinity(affinity);
                                        break;
                                }

                                it++;
                        }
                        else
                        {
                                it++;
                        }
                }


                wa.mapStatusToBehavior();
        }
        else if(cate == PLASMACELL)
        {
                /*secret antibody*/
                int id = wa.getID();
                int agentID = wa.getAgentID();
                pair<int,int> position = wa.getPosition();
                WordAgent ab(id,env,this,position,ANTIBODY,1);
                ab.setAgentID(agentID);
                vector<int> rec = wa.getRecReceptor();
                ab.setRecReceptor(rec);
                ab.setStatus(ACTIVE);
                ab.mapStatusToBehavior();

                int numOfAntibody = 10;
                for(int i = 0; i < numOfAntibody; i++)
                {
                        addWordAgent(ab);
                }

                wa.setStatus(DIE);
                wa.mapStatusToBehavior();
        }


        return true;
}

bool Simulator::select(WordAgent & wa)
{
        if(wa.getStatus() == MUTATE)
        {
                /**/
                int posi = _calcSub(wa.getPosition());
                map<int,WordAgent>::iterator it = vWordAgents[posi].begin();

                while(it != vWordAgents[posi].end())
                {
                        if((it->second.getCategory() == BCELL) && (it->second.getStatus() == MUTATE))
                        {
                                vector<int> s = wa.getAgReceptor();
                                vector<int> d = it->second.getAgReceptor();
				int sID = wa.getSentenceID();
				int dID = it->second.getSentenceID();
                                if(_isSame(s,d) && (sID == dID))
                                {
                                        if(it->second.getMutatedAffinity() < wa.getMutatedAffinity())
                                        {
                                                it->second.setStatus(ACTIVE);
                                                it->second.mapStatusToBehavior();
                                        }
                                        else if(it->second.getMutatedAffinity() > wa.getMutatedAffinity())
                                        {
                                                wa.setStatus(ACTIVE);
                                                break;
                                        }
                                }
                        }
                        it++;
                }

                if(wa.getStatus() == MUTATE)
                {
                        if(wa.calFeedback())
                        {
                                map<int,double> tmp = wa.getTmpReceptor();
                                wa.setDomReceptor(tmp);
                                wa.setStatus(MATURE);
                                env->updateFeatureWeights(tmp);
                        }
                        else
                        {
                                wa.setStatus(ACTIVE);
                        }
                }

                wa.mapStatusToBehavior();
        }

        return true;
}
bool Simulator::regulateByDelete(WordAgent & wa,int N)
{
        int c = N;
        int posi = _calcSub(wa.getPosition());
        map<int,WordAgent>::iterator it = vWordAgents[posi].begin();
        while(it != vWordAgents[posi].end())
        {
                {
                        if(it->second.getID() == wa.getID())
                        {
                                if(it->second.getStatus() == ACTIVE)
                                {
                                        it->second.setStatus(DIE);
                                        it->second.mapStatusToBehavior();
                                        c--;
                                }
                        }
                }
                if(c == 0)
                {
                        break;
                }
                it++;
        }
        return true;
}
int Simulator::getAgNum()
{
        return agNum;
}

bool Simulator::run(const Sentence & sen, const std::vector<int> & fa)
{
        /*reset interating objects*/
	bool hasRun = true;
	int size = 0;
	for(size_t i = 0; i < vWordAgents.size(); i++)
	{
	        size += vWordAgents[i].size();
        }
        cout<<endl<<"size of agents is "<<size<<endl;

	clock_t start,finish;
        double totaltime;
        start = clock();
	bool fout = false;
	env->setFeedbackFlag(false);
	std::pair<Sentence, vector<int> > p;
        p.first = sen;
        p.second = fa;
	while(hasRun){
	        hasRun = false;

                for(size_t i = 0; i < vWordAgents.size(); i++)
                {
			for(map<int,WordAgent>::iterator it = vWordAgents[i].begin(); it != vWordAgents[i].end(); it++)
			{
                                it->second.run();
                                if(_getAgNum() == 0)
                                {
                                       fout = true;
                                       cout<<"Ags are all killed!"<<endl;
                                       break;
                                }
				if(!env->getFeedbackFlag())
				{
				        hasRun = true;
                                }
                                else
                                {
                                    fout = true;
                                    break;
                                }
			}
			_release();
			if(fout)
			{
                                hasRun = false;
                                fout = false;
                                break;
                        }
		}
		finish = clock();
                totaltime = (double)(finish-start)/CLOCKS_PER_SEC;
                if(totaltime > TIMETHRESHOLD)
                {
                        break;
                }
	}


	/*remove antigen*/
	_removeAg();
	_resetStatus();

	return true;
}

bool Simulator::_release()
{
        for(size_t i = 0; i < vWordAgents.size(); i++)
        {
               for(map<int,WordAgent>::iterator it = vWordAgents[i].begin(); it != vWordAgents[i].end(); )
                {
                        if(it->second.getStatus() == DIE)
                        {
                                vWordAgents[i].erase(it++);
                        }
                        else
                        {
                                ++it;
                        }
                }
        }

        return true;
}

bool Simulator::_removeAg()
{
        for(size_t i = 0; i < vWordAgents.size(); i++)
        {
               for(map<int,WordAgent>::iterator it = vWordAgents[i].begin(); it != vWordAgents[i].end(); )
                {
                        if(it->second.getCategory() == ANTIGEN)
                        {
                                vWordAgents[i].erase(it++);
                        }
                        else
                        {
                                ++it;
                        }
                }
        }
        return true;
}

bool Simulator::_resetStatus()
{
        for(size_t i = 0; i < vWordAgents.size(); i++)
        {
               for(map<int,WordAgent>::iterator it = vWordAgents[i].begin(); it != vWordAgents[i].end();it++ )
                {
			int status = it->second.getStatus();
                        if((status != ACTIVE) && (status != MATURE))
                        {
                               it->second.setStatus(ACTIVE);
                        }
                }
        }
        return true;
}

int Simulator::_getAgNum()
{
        return agNum;
}
void Simulator::setAgNum(int num)
{
        agNum = num;
}

void Simulator::_updateAgNum()
{
        agNum--;
}


void Simulator::init()
{
        times = 0;
}

void Simulator::initAccuracy()
{
        historyAccuracy.clear();
}

void Simulator::recordAccuracy(double & acc)
{
        historyAccuracy.push_back(acc);
}

std::vector<double> Simulator::getAccuracy()
{
        return historyAccuracy;
}

bool Simulator::initLocalAgents(WordAgent * wa)
{

        wa->resetLocalAgents();
        int posi = _calcSub(wa->getPosition());
        map<int,WordAgent>::iterator it = vWordAgents[posi].begin();
        while(it != vWordAgents[posi].end())
        {
                int agentID = it->second.getAgentID();
                if(agentID != wa->getAgentID())
                {
                        it->second.insertLocalAgents(agentID);
                }
                it++;
        }

        return true;
}

int Simulator::calConcentration(WordAgent * wa)
{
	int concentration = 1;
	int posi = _calcSub(wa->getPosition());
	map<int,WordAgent>::iterator it = vWordAgents[posi].begin();
	int id = wa->getID();
	while(it != vWordAgents[posi].end())
	{
		if(id == it->second.getID())
		{
			concentration++;
		}
		it++;
	}
	return concentration;
}

bool Simulator::getNearbyAgents(const int id, const pair<int,int> & pos, vector<WordAgent> & nearbyAgents)
{
	nearbyAgents.clear();
	int posi = _calcSub(pos);
	map<int,WordAgent>::iterator it = vWordAgents[posi].begin();
	while(it != vWordAgents[posi].end())
	{
		if(id != it->second.getID())
		{
			nearbyAgents.push_back(it->second);
		}
		it++;
	}
	return true;
}

int Simulator::agentCount(std::pair<int,int> & position)
{
        int posi = _calcSub(position);
        return vWordAgents[posi].size();
}
