
#include <math.h>
#include <iostream>

#include "Parameter.hpp"
#include "Evaluation.hpp"

using namespace std;

Evaluation::Evaluation(Predictor * pre, Model * mod)
{
	pPredictor = pre;
	pModel = mod;
}

Evaluation::~Evaluation()
{}

double Evaluation::calAccuracy(const vector<int> & newLabel,
		const vector<int> & orgLabel)
{
	if(newLabel.size() != orgLabel.size())
	{
		int a;
		cout<<"sizes are not the same!"<<endl;
		cin>>a;
		return -1.0;
	}
	double accuracy = 0.0;
	for(size_t i = 1; i < newLabel.size(); i++){
		accuracy += (newLabel[i] == orgLabel[i]);
	}
	return accuracy / (double)(newLabel.size() - 1);
}


pair<int, double>  Evaluation::calFeedback(const Sentence & sen, WordAgent * wa,
                const vector<int> & standard)
{
	vector<double> tmp = pModel->getFeatureWeight();
	vector<int> father;
	father.resize(standard.size());
	double value = pPredictor->predict(sen,father);
	double accuracy = calAccuracy(father, standard);

	map<int, double> tmpDomFeature = wa->getTmpReceptor();
	pModel->updateFeatureWeight(tmpDomFeature);
	vector<double> tmp2 = pModel->getFeatureWeight();

	vector<int> mutatefather;
	mutatefather.resize(standard.size());
	double mutatevalue = pPredictor->predict(sen,mutatefather);
	double mutateaccuracy = calAccuracy(mutatefather, standard);


	int differ_acc = int((mutateaccuracy - accuracy) * PRECISION);

	pModel->setFeatureWeight(tmp);

	pair<int,double> p;
	if(differ_acc > 0)
	{
	        p.first = 1;
                p.second = mutateaccuracy;

                cout<<".";
                cout<<endl;
                cout<<"accuracy vs maccuracy : ("<<accuracy<<" : "
                    <<mutateaccuracy<<") ;"<<endl;
                return p;
        }

	p.first = -1;
	p.second = 0.0;
	return p;
}

double Evaluation::evalute(Sentence & sen, int senID,std::vector<int> & standard)
{
        vector<int> father;
        father.resize(standard.size());
	double value = pPredictor->predict(sen,father);
	double accuracy = calAccuracy(father, standard);
	vector<double> fw = pModel->getFeatureWeight();
	return accuracy;
}


