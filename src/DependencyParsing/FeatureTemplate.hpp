#ifndef __FEATURETMPLATE__
#define __FEATURETMPLATE__

#include <vector>
#include <string>
#include <map>
#include <string>
//#include <ext/hash_map>

#include "Sentence.hpp"

using namespace std;
//using namespace __gnu_cxx;


class FeatureTemplate{


private:
	const Sentence * pSen;
	int p, c;
	std::map<int,std::vector<std::string> > mSF;

public:

    FeatureTemplate();
	bool abstractFeature(const Sentence & sen, int pa, int pb,
			std::vector<std::string> & featVec);
    bool abstractFeature(const int & senID, int pa, int pb,
			std::vector<std::string> & featVec);
    inline std::vector<std::string> abstractFeature(const int & senID, int pa, int pb);
    bool _initGainFeatures();
    bool insertFeatures(int v, std::vector<std::string> & f);
private:
	bool _abstractFeature(std::vector<std::string> & featVec);
	std::string _crtWord(int pos);
	std::string _crtPos(int pos);
	std::string _nextPos(int pos);
	std::string _prePos(int pos);
	bool _initHeader(std::string & str, int pos);

};

#endif

