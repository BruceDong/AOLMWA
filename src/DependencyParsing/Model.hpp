#ifndef __MODEL__
#define __MODEL__

#include <map>
#include <vector>
#include <fstream>

#include "Sentence.hpp"
#include "FeatureTemplate.hpp"

using namespace std;

class Model{
private:
	int fNumber;

	fstream f;

	std::vector<double> fWeight;
	std::vector<double> gWeight;
	std::vector< vector<double> > globalWeight;
	std::vector<double> accFeatureWeight;
	std::vector<int> sentenceFeature;

	std::map<std::string, int> fMap;

	FeatureTemplate ft;

public:
        Model();
        ~Model();

	double wordPairWeight(const Sentence & sen,int p, int c);
	double wordPairWeight(const Sentence & sen, int senID,int p, int c);

	bool getFeatureIDVec(const Sentence & sen, int p, int c,
			std::vector<int> & featIDVec);
        double sumFeatureWeight(const std::vector<std::string> & featVec);

	int addFeature(const std::string & feat);
	bool getFeatures(const Sentence & sen, std::vector<std::vector<std::string> > & sens,std::vector<int> & fa);
	bool getAllFeatures(const Sentence & sen, std::vector<std::vector<std::string> > & sens, int senID);

	int initFeatureWeight(int learnTimes, int parts);
	bool savingFeatureWeights(int learnTimes, int parts);
	bool setFeatureWeight(std::vector<double> & newWeight);
	bool updateFeatureWeight(std::map<int, double> & newFea);
	std::vector<double> getFeatureWeight();
	std::vector<double> getFeatureWeights();
	void setAccumulateFeatureSize(int size);
	void accumulateFeatureWeight(vector<double> & fw);
	vector<double> getAccumulateFeatureWeight();

	void resetSentenceFeature();
	std::vector<int> getSentenceFeature();
	std::vector<double> mergeFeatureWeight(int learnTimes);


private:
	inline int _getFeatureID(const std::string & feat);
};

#endif
