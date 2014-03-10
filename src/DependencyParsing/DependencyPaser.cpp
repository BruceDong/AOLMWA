#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <cassert>
#include <cstring>
#include <cstdlib>

#include "DependencyPaser.hpp"
#include "Parameter.hpp"

using namespace std;

DependencyPaser::DependencyPaser()
{
	pModel = new Model();
	pPredictor = new Predictor((Model *)pModel);
	pEvaluation = new Evaluation((Predictor *) pPredictor,(Model *) pModel);
	pTrainer = new Trainer((Model *) pModel, (Evaluation *) pEvaluation);
}

DependencyPaser::~DependencyPaser()
{
	delete pModel;
	delete pTrainer;
	delete pPredictor;
}

bool DependencyPaser::loadModel(const char * file)
{
	return true;
}

bool DependencyPaser::saveModel(const char * file)
{
	return true;
}

bool DependencyPaser::_readFileAddBCell(const char * file)
{
	ifstream fin(file);
	if(fin)
	{
		cout<<file<<endl;
	}
	string line;
	vector<vector<string> > senes;
	int senID = 1;
	while(getline(fin, line)){
		if(line == ""){
			vector<int> father;
			Sentence sen;
			sen.push_back(make_pair("ROOT", "ORG"));
			father.push_back(-1);
			for(size_t i = 0; i < senes.size(); i++){
				sen.push_back(make_pair(senes[i][1], senes[i][3]));
				father.push_back(atoi(senes[i][6].c_str()));
			}

			pTrainer->addBCells(senID, sen, father);
			cout<<"."<<senID<<" ";
			senes.clear();
			senID++;
		}
		else{
			vector<string> item;
			string tmp;
			istringstream sin(line);
			while(sin >> tmp){
				item.push_back(tmp);
			}

			senes.push_back(item);
		}

	}

	return true;
}

bool DependencyPaser::_readFileTrain(const char * trainFile,const char * testFile, const char * outFile, const char * evaluateFile)
{
	string line;
	vector<vector<string> > senes;
	_printEvaluateLine(evaluateFile);
	ifstream fin(trainFile);
	vector<vector<string> > f;
	vector<string> t;
	while(getline(fin,line))
	{
	        if(line == "")
	        {
	                f.push_back(t);
	                t.clear();
                }
                else
                {
                        t.push_back(line);
                }

        }
        fin.close();
        pTrainer->initSentences();


	for(size_t i = 0; i < LEARNTIMES; i++)
	{
	        cout<<"Learning "<<i+1<<" times"<<endl;

	        int senNum = 1;
		int times = i + 1;
		int parts = (int)f.size()/PART;
                for(int p = 0; p < parts; p++)
                {
                        int size = pModel->initFeatureWeight(i,p);
                        oldfw = pModel->getFeatureWeight();
                        pModel->setAccumulateFeatureSize(size);
                        pTrainer->constructBcellNet(p, i);

                        for(int j = p*PART; j < ((1+p)*PART); j++)
                        {
                                for(size_t m = 0 ; m < f[j].size(); m++)
                                {
                                        vector<string> item;
                                        string tmp;
                                        istringstream sin(f[j][m]);
                                        while(sin >> tmp){
                                                item.push_back(tmp);
                                        }
                                        senes.push_back(item);
                                }

                                vector<int> father;
                                Sentence sen;
                                sen.push_back(make_pair("ROOT", "ORG"));
                                father.push_back(-1);
                                for(size_t i = 0; i < senes.size(); i++){
                                        sen.push_back(make_pair(senes[i][1], senes[i][3]));
                                        father.push_back(atoi(senes[i][6].c_str()));
                                }
                                pTrainer->rfTrain(sen, senNum, father, times);
                                senNum++;
                                senes.clear();
                        }

                        /*store word agents*/
                        pTrainer->storeWordAgent(i,p);
                        /*reset feature weight*/
                        pModel->savingFeatureWeights(i,p);
                }

                predictFile(testFile,outFile,i+1,senNum);
	}

        return true;
}

bool DependencyPaser::predictFile(const char * testFile, const char * outFile, int Iter, int Num)
{
        cout<<"Predicting...";

	ifstream fin(testFile);
	ofstream fout(outFile, ios::out|ios::app);
	string line;
	vector<vector<string> > senes;
	oldfw = pModel->mergeFeatureWeight();
	vfw.push_back(oldfw);
	newfw.resize(oldfw.size());
	double N = double(Iter)*(double)Num;
	for(size_t i = 0; i < vfw.size();i++)
	{
	        for(size_t j = 0; j < vfw[i].size(); j++)
	        {
	                newfw[j] += vfw[i][j];
                }
        }

        for(size_t i = 0 ; i < newfw.size(); i++)
        {
                newfw[i] = newfw[i]/N;
        }

        pModel->setFeatureWeight(newfw);


	int senNum = 0;
	double sum  = 0.0;
	double sum1 = 0.0;
	double total = 0.0;
	while(getline(fin, line)){
		if(line == ""){
			vector<int> father;
			Sentence sen;
			sen.push_back(make_pair("ROOT", "ORG"));
			for(size_t i = 0; i < senes.size(); i++){
				sen.push_back(make_pair(senes[i][1], senes[i][3]));
			}

			father.resize(sen.size());
			predict(sen,father);
			int a = 0;
			for(size_t i = 1; i < senes.size(); i++){
				if(father[i+1] == atoi(senes[i][6].c_str()))
				{
				        a++;
                                }

			}
			cout<<"corr "<<a<<" "<<"total "<<senes.size()-1.0<<endl;

			double acc = (double)a/(double)(senes.size()-1.0);
			sum += acc;
			sum1 += a;
			total += double(senes.size() -1.0);
			senNum++;
			senes.clear();

		}
		else{
			vector<string> item;
			string tmp;
			istringstream sin(line);
			while(sin >> tmp){
				item.push_back(tmp);
			}
			senes.push_back(item);
		}
	}

	cout<<"acc is "<<sum1/total<<endl;
	fout<<Iter<<" is "<<sum1/total<<endl;
	fout.close();
	cout<<endl<<"Predicting finished!"<<endl;
	return true;

}

double DependencyPaser::predict(const Sentence & sen, int senID, vector<int> & fa)
{
	return pPredictor->predict(sen, senID, fa);
}

double DependencyPaser::predict(const Sentence & sen,std::vector<int> & fa)
{
        return pPredictor->predict(sen, fa);
}

double DependencyPaser::evaluate(const char * outFile, const char * evaluateFile)
{
        cout<<"Evaluating..."<<endl;
        double accuracy = 0.0;
        int all = 0;
        int correct = 0;
        ifstream fin(outFile);
        ofstream fout(evaluateFile,ios::app);
        string line;
        while(getline(fin,line))
        {
                if(line != "")
                {
                        vector<string> item;
			string tmp;
			istringstream sin(line);
			while(sin >> tmp){
				item.push_back(tmp);
			}

			if(atoi(item[(int)item.size() - 1].c_str()) == 1)
			{
			        correct++;
                        }
                        all++;
                }
        }

        accuracy = (double)correct/(double)all;
        cout<<"Evaluating finished!"<<endl;
        cout<<"Accuracy is "<<accuracy<<endl;

        fout<<"Accuracy is "<<accuracy<<endl;
        return accuracy;
}

void DependencyPaser::parsing(const char * trainFile,const char * testFile, const char * outFile, const char * evaluateFile)
{
        cout<<"Initilizing B cell Network...";
        _readFileAddBCell(trainFile);
        cout<<"Initilizing finished!"<<endl;

        cout<<"Online learning...";
        _readFileTrain(trainFile,testFile,outFile,evaluateFile);
        cout<<"Online learning finished!";
}

void DependencyPaser::_printEvaluateLine(const char * evaluateFile)
{
        ofstream fout(evaluateFile,ios::out);
        fout<<"-------------------------------------------------------"<<endl;
        fout.close();
}


