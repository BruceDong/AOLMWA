#include "Model.hpp"
#include <iostream>
#include <algorithm>

Model::Model()
{
	accFeatureWeight.resize(fWeight.size());
        sentenceFeature.clear();
}
Model::~Model()
{
        f.close();
}
double Model::wordPairWeight(const Sentence & sen, int senID,int p, int c)
{

	vector<string> featVec;
	ft.abstractFeature(senID, p, c, featVec);

	return sumFeatureWeight(featVec);
}

double Model::wordPairWeight(const Sentence & sen,int p, int c)
{
        vector<string> featVec;
	ft.abstractFeature(sen,p, c,featVec);

	return sumFeatureWeight(featVec);
}

bool Model::getFeatureIDVec(const Sentence & sen, int p, int c,
		vector<int> & featIDVec)
{
	vector<string> featVec;
	ft.abstractFeature(sen, p, c, featVec);
	featIDVec.clear();
	for(size_t i = 0; i < featVec.size(); i++){
		featIDVec.push_back(addFeature(featVec[i]));
	}
	return true;
}


double Model::sumFeatureWeight(const vector<string> & featVec)
{
	double sum = 0.0;

        int fid = 0;
	for(size_t i = 0; i < featVec.size(); i++){
		fid = _getFeatureID(featVec[i]);

		if(fid == -1) continue;
		sum += fWeight[fid];
	}

	return sum;
}

inline int Model::_getFeatureID(const string & feat)
{

	if(fMap.find(feat) == fMap.end()) return -1;

	return fMap[feat];
}

int Model::addFeature(const string & feat)
{
	if(fMap.find(feat) == fMap.end()){
		int id = fMap.size();
		fMap[feat] = id;
	}

	return fMap[feat];
}

bool Model::getFeatures(const Sentence & sen,vector<vector<string> > &sens, vector<int> & fa)
{
	for(size_t j = 1; j < sens.size(); j++)
	{
		vector<string> fea;
		ft.abstractFeature(sen, fa[j], j, fea);
	}

	return true;
}

bool Model::getAllFeatures(const Sentence & sen, std::vector<std::vector<std::string> > & sens, int senID)
{
        vector<int> vfid;
    int n = sen.size();
    for(int i = 0; i < n; i++){
		for(int j = 0; j < n; j++){
			if(j == i) continue;
			int key = int(senID * 1e4 + i * 1e2 + j);

			vector<string> fea;
            ft.abstractFeature(sen, i, j, fea);
            for(size_t i = 0 ; i < fea.size(); i++)
            {
                    int fid = _getFeatureID(fea[i]);;
                    if(fid != -1)
                    {
                            vector<int>::iterator result = find( vfid.begin( ), vfid.end( ), fid ); //查找3
                            if  ( result == vfid.end( ) )
                            {
                                    vfid.push_back(fid);
                                }

                        }

                        ft.insertFeatures(key,fea);

                        }
		}
	}
	sort(vfid.begin(),vfid.end());

	for(size_t i = 0; i < vfid.size(); i++)
	{
	        f<<vfid[i]<<" ";
        }
        f<<endl;


    return true;
}

int Model::initFeatureWeight(int learnTimes, int parts)
{
        fWeight.resize((int)fMap.size());
        if(learnTimes == 0)
        {
                for(size_t i = 0; i < fWeight.size(); i++)
                {
                        fWeight[i] = 0.0;
                }
        }
        else
        {
                for(size_t i = 0; i < globalWeight[parts].size(); i++)
                {
                        fWeight[i] = globalWeight[parts][i];
                }
        }
        return (int)fWeight.size();
}

bool Model::savingFeatureWeights(int learnTimes, int parts)
{
        if(fWeight.size() == 0)
        return false;

        if(learnTimes == 0)
        {
                globalWeight.push_back(fWeight);
        }
        else
        {
                globalWeight[parts] = fWeight;
        }

        return true;
}

std::vector<double> Model::mergeFeatureWeight()
{
        gWeight.resize(fWeight.size());
        for(size_t i = 0; i < gWeight.size(); i++)
        {
                for(size_t p = 0; p < globalWeight.size(); p++)
                {
                        gWeight[i] += (globalWeight[p][i] - gWeight[i]);
                }
        }

        for(size_t p = 0; p < globalWeight.size(); p++)
        {
                for(size_t i = 0; i < globalWeight[p].size(); i++)
                {
                        globalWeight[p][i] = gWeight[i];
                }
        }

        return gWeight;
}

vector<double> Model::getFeatureWeight()
{
	return fWeight;
}

bool Model::setFeatureWeight(std::vector<double> & newWeight)
{
        for(size_t i = 0; i < fWeight.size();i++)
        {
                fWeight[i] = newWeight[i];
        }
        return true;
}

bool Model::updateFeatureWeight(map<int, double> & domFeatures)
{
	map<int, double>::iterator it;
	for(it = domFeatures.begin(); it != domFeatures.end(); it++)
	{
		if(fWeight[it->first] < it->second)
		{
		        fWeight[it->first] = it->second;
		}


	}
	return true;
}

std::vector<double> Model::getFeatureWeights()
{
        return fWeight;
}

std::vector<int> Model::getSentenceFeature()
{
        return sentenceFeature;
}

void Model::resetSentenceFeature()
{
        sentenceFeature.clear();
}

void Model::accumulateFeatureWeight(vector<double> & fw)
{
	if(accFeatureWeight.size() != fw.size())
	{
		cout<<"acc "<<accFeatureWeight.size()<<", fw "<<fw.size()<<endl;
		cout<<"size is not the same"<<endl;
		exit(-1);
	}
	for(size_t i = 0; i < fw.size(); i++)
	accFeatureWeight[i] += fw[i];
}

vector<double> Model::getAccumulateFeatureWeight()
{
	return accFeatureWeight;
}

void Model::setAccumulateFeatureSize(int size)
{
	accFeatureWeight.resize(size);
}
