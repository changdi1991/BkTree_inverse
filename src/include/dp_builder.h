/*
 * dp_builder.h
 *
 *  Created on: Aug 17, 2015
 *      Author: Liang Ding
 */

#ifndef SRC_INCLUDE_DP_BUILDER_H_
#define SRC_INCLUDE_DP_BUILDER_H_

#include <iostream>
#include <set>
#include <map>
#include "cliq_value.h"
#include "bktree_scanner.h"
#include "pattern.h"
#include "backbone_coor.h"

class nt_compat_matrix_value{
public:
	nt_compat_matrix_value(void);
	~nt_compat_matrix_value(void);

	bool isCompat;

	char childNt;

	char parentNt;

	void print(void);
};


class dp_builder{
private:
	//number of pattern considered in DP algorithm
	int patternLimit;

	//the best number of solutions are stored in the DP table
	int numBestSols;

	//Every clique of the bktree has t best cliq_values
	std::vector< std::vector<cliq_value> > scoreVector;

	//pattern indices for the patterns with positive scores
	std::vector< std::vector< short > > positivePatIndices;

	//pattern with positive scores. They are stored because the nucleotides in the
	//	pattern are different with patterns in the database after merging.
	std::vector< std::vector< pattern > > positivePats;

	std::vector<cliq_value> opt_cliq_value;

	short optPatIndex;

	std::vector< std::string > optSeq;

	//1st vector refers to centern number of best solutions;
	//2nd vector stores multiple nt sources from different bags for a position of an optimal solution.
	std::vector< std::vector< std::string > > optSeq_nt_merged;

	std::vector< std::set<std::string> > optInters;

	std::map<std::string, nt_compat_matrix_value> ntCompatMatrixMap;

	void read_nt_compat_matrix(void);
public:
	dp_builder(void);

	void set_pattern_limit(int _limit);

	void set_num_best_sols(int _numSols);

	void dp(backbone_coor& _bbc, bktree_scanner& _bktree, bPat_db& _bpatDb, int _numSols, bool _useBestScore);

	//Return true if parent pattern and one of it child pattern is compatible, false otherwise.
	bool bagCompat(parent_child_kcliq_match&, pattern&, pattern&);

	bool bagCompat_nt_merged(parent_child_kcliq_match&, pattern&, pattern&);

	void traceback(clique_key* _root,
					short _rootPatIndex,
					vector<short>& _childPatIndices,
					bktree_scanner& _bktree,
					bPat_db& _bpatDb);

	void traceback_nt_merged(short optSolIndex, clique_key* _root,
					short _rootPatIndex,
					vector<short>& _childPatIndices);

	void opt_to_file(const char* _ntsFilePath, const char* _interFilePath);

	void opt_to_file_merged(const char* _ntsFilePath, const char* _interFilePath);

	~dp_builder(void);
};


class temp_score{
public:
	float score;

	std::vector<short> currentVec;

	//pattern pat;

	temp_score(void){
		this->score = 0.0;
		this->currentVec.clear();
		//pat = pattern();
	}

	~temp_score(void){
		this->currentVec.clear();
	};
};


class opt_score{
public:
	float score;

	std::vector<short> currentVec;

	short patIndex;

	opt_score(void){
		this->score = 0.0;
		this->currentVec.clear();
		this->patIndex = 0;
	}

	~opt_score(void){
		this->currentVec.clear();
	};
};


struct comparator {
	bool operator()(temp_score i, temp_score j){
		return i.score > j.score;
	}
};


struct comparator_opt {
	bool operator()(opt_score i, opt_score j){
		return i.score > j.score;
	}
};


#endif /* SRC_INCLUDE_DP_BUILDER_H_ */
