#include "Trainer.hpp"
#include "Parameter.hpp"

#include <iostream>
#include <vector>
#include <fstream>


using namespace std;

Trainer::Trainer(Model * pm, Evaluation * eva) : pModel(pm)
{
	pEnv = new Environment(ROWS, COLS,eva,pm);
	simu = new Simulator(pEnv,eva);
	pEva = eva;
	f.open("./result/mid",ios::out);
	vSen.clear();
}

Trainer::~Trainer()
{
         f.close();
	delete pEnv;
	delete simu;
}

bool Trainer::rfTrain(const Sentence & sen, const int senID,const vector<int> & fa, int iter)
{
        pEnv->setSentence(sen);
        pEnv->setSentenceID(senID);
        pEnv->setFather(fa);

        _injectAntigen(sen, fa);

        cout<<"(s, "<<senID<<")";

        simu->getLearnTime(iter);
	if(simu->run(sen,fa))
        {
		vector<double> fw = pModel->getFeatureWeight();
		pModel->accumulateFeatureWeight(fw);

                return true;
        }

	return false;
}

bool Trainer::addBCells(int senID, const Sentence & sen, const vector<int> & fa)
{
	vector<int> features;
	for(size_t i = 1; i < sen.size(); i++){
		int j = fa[i];
		int bi = _buildBCell(sen[i].first);
		int bj = _buildBCell(sen[j].first);
		pModel->getFeatureIDVec(sen, j, i, features);
		BCells[bi].addRecFeature(features);
		BCells[bi].setSentence(sen,senID);
		BCells[bj].addDomFeature(features);
		BCells[bj].setSentence(sen,senID);
	}
	return true;
}

int Trainer::_buildBCell(const string & word)
{
        if(wordID.find(word) == wordID.end()){
		wordID[word] = BCells.size();
		pair<int,int> pos = pEnv->getRandomPosition();
		BCells.push_back(WordAgent(wordID[word], pEnv,simu,pos, BCELL,1));
	}
	int res = wordID[word];
	if((int)wordFreq.size() <= res){
		wordFreq.push_back(0);
	}
	wordFreq[res] ++;
	return res;
}



bool Trainer::constructBcellNet(int p, int learntimes)
{
        if(learntimes != 0)
        {
                simu->resetWordAgents(p);
                return true;
        }
        int begin = p*PART;
        int end   = (1 + p)*PART;

        cout<<"Constructing the "<<1 + p<<" region B cell network..."<<endl;
        int sizeofBcell = 0;

	for(size_t i = 0; i < BCells.size(); i++)
	{
	        for(int j = begin; j < end; j++)
	        {
	                int wordSenId = j + 1;

	                if(BCells[i].isInSentence(wordSenId))
	                {
				cout<<"B cell "<<wordSenId<<" ";
                                simu->addWordAgent(BCells[i]);
                                sizeofBcell++;
	                }
	        }
	}

	cout<<"Construct finished!"<<endl;
	return true;
}

bool Trainer::_buildAntigen(const Sentence & sen,int child,const std::string & word, int parent,const std::string & fword)
{
        Antigens.push_back(WordAgent(wordID[word],pEnv,simu,pEnv->getRandomPosition(), ANTIGEN,1));
        Antigens.push_back(WordAgent(wordID[fword],pEnv,simu,pEnv->getRandomPosition(), ANTIGEN,1));
        vector<int> features;
        pModel->getFeatureIDVec(sen, parent, child, features);
        vector<WordAgent>::iterator it = Antigens.end();
        (--(--it));
        (*it).addRecFeature(features);
        Antigens.erase(Antigens.end());

	return true;
}

bool Trainer::_addAntigenToSimulator(const Sentence & sen, const std::vector<int> & fa)
{
	return true;
}

bool Trainer::_addAntigen()
{
        vector<pair<int,int> > positions;

        for(size_t i = 0; i < ROWS; i++)
        {
                for(size_t j = 0; j < COLS; j++)
                {
                        positions.push_back(make_pair(i,j));
                }
        }

        pEnv->initAgID();
        if(Antigens.size() > 0)
        {
                int pos = -1;
                cout<<"Size of antigens is "<<Antigens.size()<<endl;
		int size = (int)Antigens.size();
		cout<<"size is "<<size<<endl;
                simu->setAgNum(size);
                for(size_t p = 0; p < Antigens.size(); p++)
                {
                        pos = p%positions.size();
                        Antigens[p].setPosition(positions[pos]);
                        simu->addWordAgent(Antigens[p]);
                }
        }

        Antigens.clear();

        return true;
}

bool Trainer::_injectAntigen(const Sentence & sen, const std::vector<int> & fa)
{
	for(size_t i = 1; i < sen.size(); i++)
	{
		int j = fa[i];
		_buildAntigen(sen,i,sen[i].first,j,sen[j].first);
		/*clone antigens*/
		cloneAntigens();
	}

	_addAntigen();

	return true;
}

bool Trainer::cloneAntigens()
{
        int l = (int)Antigens.size() - 1;

        for(size_t p = 1; p < ROWS * COLS; p++)
        {
                WordAgent wa(Antigens[l].getID(), pEnv,simu,Antigens[l].getPosition(), ANTIGEN,1);
                vector<int> rec = Antigens[l].getRecReceptor();
                wa.setRecReceptor(rec);
                Antigens.push_back(wa);
        }


        return true;
}

bool Trainer::cloneBCells()
{
	for(size_t i = 0; i < wordFreq.size(); i++){
		for(int j = 1; j < wordFreq[i]; j++){

                        if(pEnv->getLocalAgentsNum(BCells[i].getPosition()) < MAXNUMAGENT)
                        {
                                pEnv->setLocalAgentsNum(BCells[i].getPosition());
                                WordAgent wa(BCells[i].getID(), pEnv,simu,BCells[i].getPosition(), BCELL,1);
                                map<int,double> m = BCells[i].getDomReceptor();
                                wa.initDomReceptor(m);
                                vector<int> v = BCells[i].getRecReceptor();
                                wa.initRecReceptor(v);
                                BCells.push_back(wa);
                        }
                        else
                        {
                                bool flag = true;
                                while(flag)
                                {
                                        pair<int,int> pos = pEnv->getRandomPosition();
                                        if(pEnv->getLocalAgentsNum(pos) < MAXNUMAGENT)/*local number of agents must be lower than threshold*/
                                        {
                                                pEnv->setLocalAgentsNum(pos);

                                                WordAgent wa(BCells[i].getID(), pEnv,simu,pos, BCELL,1);
                                                map<int,double> m = BCells[i].getDomReceptor();
                                                wa.initDomReceptor(m);
                                                vector<int> v = BCells[i].getRecReceptor();
                                                wa.initRecReceptor(v);
                                                BCells.push_back(wa);
                                                flag = false;
                                        }
                                }
                        }
		}
		cout<<".";
	}
	return true;
}

bool Trainer::initSentenceID()
{
        pEnv->setSentenceID(0);
        return true;
}

void Trainer::testSub()
{
        pEnv->testSub(20);
}

void Trainer::storeWordAgent(int lt, int p)
{
        simu->storeLocalWordAgent(lt,p);
}

bool Trainer::saveFeatureWeights()
{
        fstream out("./result/fweight",ios::out|ios::app);
        if(out)
        {
                vector<double> fw = pModel->getFeatureWeight();
                feaSet.push_back(fw);
                for(size_t i = 0; i < fw.size();i++)
                {
                        out<<fw[i]<<"\t";
                }
                out<<endl;

        }
        out.close();
        return true;

}

void Trainer::initSentences()
{
       vSen.clear();
}



