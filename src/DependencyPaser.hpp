#ifndef __DEPENDENCY_PASER__
#define __DEPENDENCY_PASER__

#include <map>
#include <string>
#include <fstream>

#include "Model.hpp"
#include "Trainer.hpp"
#include "Sentence.hpp"
#include "Predictor.hpp"

class DependencyPaser{
private:

	fstream file;

	std::vector<double> oldfw;
	std::vector<double> newfw;
	std::vector<std::vector<double> > vfw;

	Model * pModel;
	Trainer * pTrainer;
	Predictor * pPredictor;
	Evaluation * pEvaluation;
public:
	DependencyPaser();
	~DependencyPaser();
	bool loadModel(const char * file);
	bool saveModel(const char * file);
	bool trainFile(const char * file);

	double predict(const Sentence & sen, int senID,std::vector<int> & fa);
	double predict(const Sentence & sen,std::vector<int> & fa);
	bool predictFile(const char * testFile, const char * outFile);
	bool predictFile(const char * testFile, const char * outFile, int Iter, int Num);

	double evaluate(const char * resultFile, const char * evaluateFile);

	void parsing(const char * trainFile,const char * testFile, const char * outFile,
                        const char * evaluateFile);
private:
	bool _readFileAddBCell(const char * file);

	bool _readFileTrain(const char * file);
        bool _readFileTrain(const char * trainFile,const char * testFile,
                                const char * outFile, const char * evaluateFile);

	void _printEvaluateLine(const char * evaluateFile);
};

#endif
