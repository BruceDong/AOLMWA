#ifndef _EVALUATION_H
#define _EVALUATION_H

#include <vector>
#include <fstream>

#include "Predictor.hpp"
#include "Model.hpp"
#include "WordAgent.hpp"

class WordAgent;

class Evaluation{
private:
	Predictor * pPredictor;
	Model * pModel;
	fstream fout;
public:
	Evaluation(Predictor * pre, Model * mod);
	~Evaluation();

        void   printLine();
	void   getValueDiff(double value, double mutateValue, double & diff);
	double calAccuracy(const std::vector<int> & newLabel,
			const std::vector<int> & orgLabel);
	double evalute(Sentence & sen, int senID,std::vector<int> & standard);
	std::pair<int, double> calFeedback(const Sentence & sen,
	                WordAgent * wa,const std::vector<int> & standard);
};

#endif
