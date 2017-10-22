/*
 * pattern.h
 *
 *  Created on: Aug 14, 2015
 *      Author: Liang Ding
 */

#ifndef SRC_INCLUDE_PATTERN_H_
#define SRC_INCLUDE_PATTERN_H_

#include <map>
#include <vector>
#include <set>
#include "bktree_scanner.h"

class edge{
public:
	short left;
	short right;

	//constructor
	edge(void);
	edge(short, short);
	~edge(void);
};


// An interaction edge in a pattern e.g., ['3 4 s55']. An edge may have many interaction edges, e.g., ['1 3 cWW', '1 3 s55'].
class inter{
public:
	edge e;

	//an edge can have different interactions, so use a vector
	std::vector<std::string> interType;

	inter(void);
	void print(void);
	~inter(void);
};


class pattern{
public:
	//score of the pattern
	double score;

	std::string nts;

	std::vector<inter> pat;

	pattern(void);
	void print(void);
	~pattern(void);
};


bool sortByScore(const pattern &lhs, const pattern &rhs);


class bag_patterns{
public:
	int totalCount; 	//frequency in database

	std::vector<pattern> bPats;

	bag_patterns(void);

	void print(void);

	~bag_patterns(void);

};


class bPat_db{
private:
	//parse the key string of patterns
	void parseKey(char*, std::string&, int&);

	//parse the string of patterns
	void parseValue(char*, std::vector<pattern>&);

	//parse multi-interactions
	void parseMultiInters(std::string, std::vector<std::string>&);

	std::map<int, edge> indexEdgeMap;

	//add more patterns within relaxed 3D distances to a patternKey
	//void relax(const char* _patFile, std::set<std::string>& _relaxNeededKeySet, int _relaxDis);

	//check if two keys are within relaxed distance
	//bool withinRelaxDis(string _keyDb, string _keyBktree, int _relaxDis);

	//a speed up of relax
	void relax2(std::set<std::string>& _relaxNeededKeySet, int _relaxDis);

	std::string key3DGenerator(std::vector<int>& _baseVec, int _relaxDis, std::vector<int>& _currentVec, int& _workingIndex, std::string& _currentKey);
public:
	std::map< std::string, bag_patterns > allBagPats;

	//read the entire pattern database (with large memory usage)
	//void readPatDB(const char*);

	std::map< std::string, bag_patterns > bagPatsBktree;

	std::map< std::string, bag_patterns > bagPatsNtMerged;

	//read patterns for the bags in a given bktree
	//Can relax the 3d distance to include more patterns
	//	Relax the distance if number of patterns is less than _relaxNumPats.
	//  _relaxDis defines how large the relaxed distance is.
	//void readPatBktree(const char*, bktree_scanner&, int _relaxNumPatsCutoff, int _relaxDis);

	// a speedup of readPatBktree
	void readPatBktree2(bktree_scanner&, int _relaxNumPatsCutoff, int _relaxDis);

	void read_nt_merged_pat(const char*);

	void patBktree_toFile(const char*);

	bPat_db(void);
	~bPat_db(void);
};

#endif /* SRC_INCLUDE_PATTERN_H_ */
