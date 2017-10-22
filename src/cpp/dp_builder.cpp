/*
 * dp_builder.cpp
 *
 *  Created on: Aug 17, 2015
 *      Author: Liang Ding
 */

#include <cstdlib>
#include <string>
#include <queue>
#include <stack>
#include "clique_key.h"
#include "dp_builder.h"
#include "dllib.h"

using namespace std;

#define DEBUG 1
#define COMPAT_PRINT 0
#define TRACEBACK_PRINT 0
#define SCORE_DEBUG_PRINT 0

const double initialScore = -999999.0;
const double invalidPatScore = -1000000.0; //when a child pattern number exceeds its number of available patterns
const double incompatScore = -99999.0;	//when a child pattern is not compatible with its parent


nt_compat_matrix_value::nt_compat_matrix_value(void){
	this->isCompat = false;
	this->childNt = '\0';
	this->parentNt = '\0';
}

void nt_compat_matrix_value::print(void){
	cout << this->isCompat << ' ' << this->childNt << ' ' << this->parentNt << endl;
}

nt_compat_matrix_value::~nt_compat_matrix_value(void){}


dp_builder::dp_builder(void){
	this->patternLimit = 0;
	this->numBestSols = 0;
	this->optPatIndex = 0;
}

void dp_builder::set_pattern_limit(int _limit){
	if(_limit <= 0){
		cout << "Error: number of patterns cannot be less than 1." << endl;
		exit(0);
	}
	this->patternLimit = _limit;
}

void dp_builder::set_num_best_sols(int _numSols){
	if(_numSols <= 0){
		cout << "Error: number of solutions cannot be less than 1." << endl;
		exit(0);
	}
	this->numBestSols = _numSols;
}


void dp_builder::dp(backbone_coor& _bbc, bktree_scanner& _bktree, bPat_db& _bpatDb, int _numSols, bool _useBestScore){
	//int maxCounter = 0;
	this->read_nt_compat_matrix();
	cout << "Running dynamic programming algorithm ... " << endl;
	this->scoreVector.resize(_bktree.get_num_cliques());
	bag_patterns bagPatterns = bag_patterns();
	bag_patterns childPatterns = bag_patterns();
	std::vector< std::vector< clique_key* > > depthOrderCliques = _bktree.get_depth_order_cliques();
	//int numPats = 0;
	/*vector<cliq_value> cliqValueVector;
	cliqValueVector.resize(this->patternLimit);
	for(int i = 0; i < this->patternLimit; i++)
		cliqValueVector[i].score.push_back(initialScore);  //score initialization*/
	double maxScore = 0.0;

	vector<short> baseVec;
	vector<short> currentVec;	//For enumerating all child patterns
	vector<short> childPatLimit;
	int workingScoreIndex;

	temp_score tmpScore = temp_score();
	vector<short> baseScoreVec;
	vector<short> curScoreVec; //For enumerating all scores for fixed child patterns
	vector<short> scoreSizeLimit;
	int workingIndex;
	double tempMaxScore = 0.0;
	bool tempIsContinue = false;

	int parentPatCounter = 0;
	cliq_value tempCliqValue = cliq_value();
	this->positivePatIndices.resize(_bktree.get_num_cliques());
	this->positivePats.resize(_bktree.get_num_cliques());
	//bool isBreakm;
	//bool mIsPushed = false;
	pattern tempChildPat = pattern();
	pattern tempParentPat = pattern();

	#if DEBUG == 1
		int zeroPatCounter = 0;
	#endif

	for(int i = int(depthOrderCliques.size())-1; i > -1; i--){ //traverse the cliques bottom-up from leaves
		for(unsigned int j = 0; j < depthOrderCliques[i].size(); j++){
			bagPatterns = bag_patterns();
			//bagPatterns = _bpatDb.bagPatsBktree[depthOrderCliques[i][j]->patternKey];
			bagPatterns = _bpatDb.bagPatsNtMerged[depthOrderCliques[i][j]->patternKey];

			#if SCORE_DEBUG_PRINT == 1
				depthOrderCliques[i][j]->print();
				cout << depthOrderCliques[i][j]->patternKey << endl;
				cout << "bagPatterns.totalCount: " << bagPatterns.totalCount << endl;
				cout << "bagPatterns.bPats.size(): " << bagPatterns.bPats.size() << endl;
				//cout << "indexInReverseDepthOrdering: " << depthOrderCliques[i][j]->indexInReverseDepthOrdering << endl;
				if(bagPatterns.totalCount == 0)
					zeroPatCounter++;
				cout << endl;
			#endif

			/*if(int(bagPatterns.bPats.size()) > this->patternLimit)
				numPats = this->patternLimit;
			else
				numPats = int(bagPatterns.bPats.size());*/

			//this->scoreVector.push_back(cliqValueVector);
			parentPatCounter = 0;
			//isBreakm = false;
			for(int m = 0; m < int(bagPatterns.bPats.size()); m++){	//m is pattern index of all patterns of parent bag
				//if(m >= int(bagPatterns.bPats.size()))		//m exceeds avaiable number of patterns
				//	break;
				//cout << "m: " << m << endl;

				if(depthOrderCliques[i][j]->children.size() == 0){ // leaf don't have a child
					//this->scoreVector[depthOrderCliques[i][j]->indexInReverseDepthOrdering][m].score[0] = bagPatterns.bPats[m].score;
					this->scoreVector[depthOrderCliques[i][j]->indexInReverseDepthOrdering].push_back(tempCliqValue);
					//here m should be equal to parentPatCounter
					this->scoreVector[depthOrderCliques[i][j]->indexInReverseDepthOrdering][parentPatCounter].score.push_back(bagPatterns.bPats[parentPatCounter].score);
					this->positivePatIndices[depthOrderCliques[i][j]->indexInReverseDepthOrdering].push_back(m);
					this->positivePats[depthOrderCliques[i][j]->indexInReverseDepthOrdering].push_back(bagPatterns.bPats[parentPatCounter]);
					parentPatCounter++;
					if(parentPatCounter > this->patternLimit)
						break;
				}
				else{	//Has children
					workingIndex = -1;
					//mIsPushed = false;
					baseVec.clear();
					currentVec.clear();
					childPatLimit.clear();
					for(unsigned n = 0; n < depthOrderCliques[i][j]->children.size(); n++){
					//for(unsigned n = 0; n < 3; n++){
						baseVec.push_back(0);
						currentVec.push_back(0);
						childPatLimit.push_back(short(this->positivePatIndices[depthOrderCliques[i][j]->children[n]->indexInReverseDepthOrdering].size()));
					}

					//stores multiple best scores
					priority_queue<temp_score, std::vector<temp_score>, comparator> score_minHeap;

					//vectorGenerator is a function to enumerate all child patterns
					//The old vectorGenerator won't work because the numbers of patterns with positive scores of children are different
					while(vectorGenerator_vary_len(baseVec, childPatLimit, currentVec, workingIndex) != "Done"){ //enumerate indices of all positive child patterns
						maxScore = bagPatterns.bPats[m].score;
						tempParentPat = bagPatterns.bPats[m];
						tempIsContinue = false;
						for(unsigned n = 0; n < currentVec.size(); n++){	//add scores from all children
							//childPatterns = _bpatDb.bagPatsBktree[depthOrderCliques[i][j]->children[n]->patternKey];
							//childPatterns = _bpatDb.bagPatsNtMerged[depthOrderCliques[i][j]->children[n]->patternKey];
							if(currentVec[n] >= int(this->positivePats[depthOrderCliques[i][j]->children[n]->indexInReverseDepthOrdering].size())){//n exceeds available number of patterns
								maxScore = invalidPatScore;
								tempIsContinue = true;
								break;
							}

							//tempChildPat = childPatterns.bPats[this->positivePatIndices[depthOrderCliques[i][j]->children[n]->indexInReverseDepthOrdering][currentVec[n]]];
							tempChildPat = this->positivePats[depthOrderCliques[i][j]->children[n]->indexInReverseDepthOrdering][currentVec[n]];
							/*if(!this->bagCompat(depthOrderCliques[i][j]->childrenkCliqMatch[n], bagPatterns.bPats[m],
									childPatterns.bPats[this->positivePatIndices[depthOrderCliques[i][j]->children[n]->indexInReverseDepthOrdering][currentVec[n]]])){
								maxScore = incompatScore;
								break;
							}*/

							#if SCORE_DEBUG_PRINT == 1
								cout << "currentVec[n]: " << currentVec[n] << endl;
								//cout << "positivePatIndices[].size: " << this->positivePatIndices[depthOrderCliques[i][j]->children[n]->indexInReverseDepthOrdering].size() << endl;
								//cout << "maxScore before compat: " << maxScore << endl;
							#endif

							if(!this->bagCompat_nt_merged(depthOrderCliques[i][j]->childrenkCliqMatch[n], tempParentPat, tempChildPat)){
								maxScore = incompatScore;
								#if SCORE_DEBUG_PRINT == 1
									cout << "Incompatible" << endl;
								#endif
								tempIsContinue = true;
								break;
							}

							if(_useBestScore)
								maxScore += this->scoreVector[depthOrderCliques[i][j]->children[n]->indexInReverseDepthOrdering][currentVec[n]].score[0];

							#if SCORE_DEBUG_PRINT == 1
								cout << "maxScore after compat: " << maxScore << endl << endl;
							#endif
						}

						if(tempIsContinue)
							continue;

						if(!_useBestScore){
							workingScoreIndex = -1;
							baseScoreVec.clear();
							curScoreVec.clear();
							scoreSizeLimit.clear();
							for(unsigned n = 0; n < currentVec.size(); n++){
								baseScoreVec.push_back(0);
								curScoreVec.push_back(0);
								scoreSizeLimit.push_back(this->scoreVector[depthOrderCliques[i][j]->children[n]->indexInReverseDepthOrdering][currentVec[n]].score.size()-1);
							}

							tempMaxScore = maxScore;
							while(vectorGenerator_vary_len(baseScoreVec, scoreSizeLimit, curScoreVec, workingScoreIndex) != "Done"){ //enumerate indices of all child scores
								maxScore = tempMaxScore;
								/*cout << endl;
								cout << "currentVec.size(): " << currentVec.size() << endl;
								cout << "curScoreVec.size(): " << curScoreVec.size() << endl;*/
								for(unsigned n = 0; n < currentVec.size(); n++){	//add all possible scores
									/*cout << "n: " << n << endl;
									cout << "currentVec[n]: " << currentVec[n] << endl;
									cout << "curScoreVec[n]: " << curScoreVec[n] << endl;
									cout << "child score: " << this->scoreVector[depthOrderCliques[i][j]->children[n]->indexInReverseDepthOrdering][currentVec[n]].score[curScoreVec[n]] << endl;*/
									maxScore += this->scoreVector[depthOrderCliques[i][j]->children[n]->indexInReverseDepthOrdering][currentVec[n]].score[curScoreVec[n]];
								}

								//cout << "maxScore: " << maxScore << endl;
								//if(maxScore > 210){
								//	exit(0);
								//}

								//if(maxScore > 204.864)
								//	maxCounter++;

								if(maxScore > 0){//if a positive score
									/*if( !mIsPushed ){ //if no score
										this->scoreVector[depthOrderCliques[i][j]->indexInReverseDepthOrdering].push_back(tempCliqValue);
										this->scoreVector[depthOrderCliques[i][j]->indexInReverseDepthOrdering][parentPatCounter].score.push_back(maxScore);
										this->scoreVector[depthOrderCliques[i][j]->indexInReverseDepthOrdering][parentPatCounter].childPatternNums.resize(1);
										this->scoreVector[depthOrderCliques[i][j]->indexInReverseDepthOrdering][parentPatCounter].childPatternNums[0] = currentVec;
										this->positivePatIndices[depthOrderCliques[i][j]->indexInReverseDepthOrdering].push_back(m);
										this->positivePats[depthOrderCliques[i][j]->indexInReverseDepthOrdering].push_back(tempParentPat);
										mIsPushed = true;
										parentPatCounter++;
										if(parentPatCounter > this->patternLimit){
											isBreakm = true;
											break;
										}
										//cout << "indexInReverseDepthOrdering].size2: " << int(this->scoreVector[depthOrderCliques[i][j]->indexInReverseDepthOrdering].size()) << endl;
									}
									else{
										if(maxScore > this->scoreVector[depthOrderCliques[i][j]->indexInReverseDepthOrdering][parentPatCounter-1].score[0]){
											this->scoreVector[depthOrderCliques[i][j]->indexInReverseDepthOrdering][parentPatCounter-1].score[0] = maxScore;
											//this->scoreVector[depthOrderCliques[i][j]->indexInReverseDepthOrdering][m].cliq = depthOrderCliques[i][j]->cliq;
											//this->scoreVector[depthOrderCliques[i][j]->indexInReverseDepthOrdering][m].selfPatternNums.resize(1);
											//this->scoreVector[depthOrderCliques[i][j]->indexInReverseDepthOrdering][m].selfPatternNums[0] = m;
											//this->scoreVector[depthOrderCliques[i][j]->indexInReverseDepthOrdering][m].patternKey.resize(1);
											//this->scoreVector[depthOrderCliques[i][j]->indexInReverseDepthOrdering][m].patternKey[0] = depthOrderCliques[i][j]->patternKey;
											//this->scoreVector[depthOrderCliques[i][j]->indexInReverseDepthOrdering][m].indexInReverseDepthOrdering.resize(1);
											//this->scoreVector[depthOrderCliques[i][j]->indexInReverseDepthOrdering][m].indexInReverseDepthOrdering[0] = depthOrderCliques[i][j]->indexInReverseDepthOrdering;
											this->scoreVector[depthOrderCliques[i][j]->indexInReverseDepthOrdering][parentPatCounter-1].childPatternNums.resize(1);
											this->scoreVector[depthOrderCliques[i][j]->indexInReverseDepthOrdering][parentPatCounter-1].childPatternNums[0] = currentVec;
											this->positivePatIndices[depthOrderCliques[i][j]->indexInReverseDepthOrdering][parentPatCounter-1] = m;
											this->positivePats[depthOrderCliques[i][j]->indexInReverseDepthOrdering][parentPatCounter-1] = tempParentPat;
										}
									}*/
									tmpScore = temp_score();
									tmpScore.score = maxScore;
									tmpScore.currentVec = currentVec;
									//tmpScore.pat = tempParentPat;
									if(int(score_minHeap.size()) < _numSols)
										score_minHeap.push(tmpScore);
									else{
										if(maxScore > score_minHeap.top().score){
											score_minHeap.pop();
											score_minHeap.push(tmpScore);
										}
									}
								}//end if
							}//end while scores
						}//end useBestScore
						else{
							if(maxScore > 0){//if a positive score
								tmpScore = temp_score();
								tmpScore.score = maxScore;
								tmpScore.currentVec = currentVec;
								//tmpScore.pat = tempParentPat;
								if(int(score_minHeap.size()) < _numSols)
									score_minHeap.push(tmpScore);
								else{
									if(maxScore > score_minHeap.top().score){
										score_minHeap.pop();
										score_minHeap.push(tmpScore);
									}
								}
							}//end if
						}
					}//end while pats

					if(int(score_minHeap.size()) > 0){
						this->scoreVector[depthOrderCliques[i][j]->indexInReverseDepthOrdering].push_back(tempCliqValue);
						this->scoreVector[depthOrderCliques[i][j]->indexInReverseDepthOrdering][parentPatCounter].score.resize(score_minHeap.size());
						this->scoreVector[depthOrderCliques[i][j]->indexInReverseDepthOrdering][parentPatCounter].childPatternNums.resize(score_minHeap.size());
						this->positivePatIndices[depthOrderCliques[i][j]->indexInReverseDepthOrdering].push_back(m);
						this->positivePats[depthOrderCliques[i][j]->indexInReverseDepthOrdering].push_back(tempParentPat);
						for(int l = int(score_minHeap.size())-1; l > -1; l--){
							tmpScore = score_minHeap.top();
							score_minHeap.pop();
							this->scoreVector[depthOrderCliques[i][j]->indexInReverseDepthOrdering][parentPatCounter].score[l] = tmpScore.score;
							this->scoreVector[depthOrderCliques[i][j]->indexInReverseDepthOrdering][parentPatCounter].childPatternNums[l] = tmpScore.currentVec;
						}
						parentPatCounter++;
						if(parentPatCounter > this->patternLimit){
							//isBreakm = true;
							break;
						}
					}

					//if(isBreakm)
					//	break;
				}//end else
				//need a function to select numBestSols scores
			}//end for m

			if(this->positivePatIndices[depthOrderCliques[i][j]->indexInReverseDepthOrdering].size() == 0){ //no pattern has positive score
				cout << "A bag does not have a pattern with positive score. Program terminates." << endl;
				//#if COMPAT_PRINT == 1
					depthOrderCliques[i][j]->print();
					cout << depthOrderCliques[i][j]->patternKey << endl;
					cout << "bagPatterns.totalCount: " << bagPatterns.totalCount << endl;
					cout << "bagPatterns.bPats.size(): " << bagPatterns.bPats.size() << endl;
					//cout << "indexInReverseDepthOrdering: " << depthOrderCliques[i][j]->indexInReverseDepthOrdering << endl;
					if(bagPatterns.totalCount == 0)
						zeroPatCounter++;
					cout << endl;
				//#endif
				exit(0);
			}

			#if COMPAT_PRINT == 1
				//cout << "[indexInReverseDepthOrdering].size1: " << int(this->scoreVector[depthOrderCliques[i][j]->indexInReverseDepthOrdering].size()) << endl;
				//cout << "parentPatCounter: " << parentPatCounter << endl;
				cout << "#############################################################" << endl << endl;
			#endif
		}//end for j
	}//end for i

	#if DEBUG == 0
		cout << "zeroPatCounter: " << zeroPatCounter << endl;
		cout << "optimalScore:  " << endl;
		for(unsigned int i = 0; i < this->scoreVector[this->scoreVector.size()-1].size(); i++){
			for(unsigned int j = 0; j < this->scoreVector[this->scoreVector.size()-1][i].score.size(); j++)
				cout << this->scoreVector[this->scoreVector.size()-1][i].score[j] << ' ';
			cout << endl << endl;
		}
		cout << endl;
		//cout << this->scoreVector[1][0].score[0] << endl;
	#endif

	opt_score tmpOptScore;
	priority_queue<opt_score, std::vector<opt_score>, comparator_opt> opt_minHeap;
	for(unsigned int i = 0; i < this->scoreVector[this->scoreVector.size()-1].size(); i++){
		for(unsigned int j = 0; j < this->scoreVector[this->scoreVector.size()-1][i].score.size(); j++){
			tmpOptScore = opt_score();
			tmpOptScore.score = this->scoreVector[this->scoreVector.size()-1][i].score[j];
			tmpOptScore.currentVec = this->scoreVector[this->scoreVector.size()-1][i].childPatternNums[j];
			tmpOptScore.patIndex = short(i);
			if(int(opt_minHeap.size()) < _numSols)
				opt_minHeap.push(tmpOptScore);
			else{
				if(tmpOptScore.score > opt_minHeap.top().score){
					opt_minHeap.pop();
					opt_minHeap.push(tmpOptScore);
				}
			}
		}
	}

	/*this->opt_cliq_value.resize(1);
	this->opt_cliq_value[0].score.resize(1);
	this->opt_cliq_value[0].score[0] = initialScore;
	this->optPatIndex = 0;
	for(unsigned int i = 0; i < this->scoreVector[this->scoreVector.size()-1].size(); i++){
		if(this->scoreVector[this->scoreVector.size()-1][i].score[0] > opt_cliq_value[0].score[0]){
			opt_cliq_value[0] = this->scoreVector[this->scoreVector.size()-1][i];
			this->optPatIndex = short(i);
		}
	}
	if(this->opt_cliq_value[0].score[0] == initialScore){unsigned
		cout << "Strange! Optimal score is not better than the initial score." << endl;
		cout << "Check your input." << endl;
		exit(0);
	}*/

	#if TRACEBACK_PRINT == 1
		cout << "Root: " << endl;
		_bktree.get_root()->print();
		this->opt_cliq_value[0].print();
	#endif

	//this->optSeq.resize(1);
	//this->optSeq[0].resize(_bbc.getSeqLen());
	this->optSeq_nt_merged.resize(opt_minHeap.size());
	for(unsigned int i = 0; i < opt_minHeap.size(); i++)
		this->optSeq_nt_merged[i].resize(_bbc.getSeqLen());
	this->optInters.resize(opt_minHeap.size());
	cout << "Running traceback ... " << endl;

	//Note that optPatIndex and childPatternNums[0] are indices of positivePatIndices, not the indices of patterns
	//this->traceback(_bktree.get_root(), this->optPatIndex, this->opt_cliq_value[0].childPatternNums[0], _bktree, _bpatDb);

	_numSols = int(opt_minHeap.size());
	for(int i = 0; i < _numSols; i++){
		tmpOptScore = opt_score();
		tmpOptScore = opt_minHeap.top();
		cout << "Optimal score " << i << ": " << tmpOptScore.score << endl;
		opt_minHeap.pop();
		this->traceback_nt_merged(short(i), _bktree.get_root(), tmpOptScore.patIndex, tmpOptScore.currentVec);
	}
	cout << "Done" << endl;
	//cout << "maxCounter: " << maxCounter << endl;

	/*#if DEBUG == 0
		cout << "Best Score: " << this->opt_cliq_value[0].score[0] << endl;
		cout << "Seq size: " << _bbc.getSeqLen() << endl;
		cout << "Optimal sequence: " << endl;
		for(unsigned int i = 0; i < this->optSeq_nt_merged[0].size(); i++){
			cout << '[';
			cout << this->optSeq_nt_merged[0][i];
			cout << ']';
		}
		cout << endl;
		cout << "Optimal Inters: " << endl;
		for(set<string>::iterator it1=this->optInters[0].begin(); it1!=this->optInters[0].end(); ++it1)
			cout << *it1 << endl;
		cout << endl;
	#endif*/
}

bool dp_builder::bagCompat(parent_child_kcliq_match& _pckm, pattern& _parentPat, pattern& _childPat){
	#if COMPAT_PRINT == 1
		cout << "parent pat: ";
		_parentPat.print();
		cout << "child pat: ";
		_childPat.print();
	#endif

	unsigned int i = 0;
	for(i = 0; i < _pckm.parentKcliqIndex.size(); i++){
		if(_parentPat.nts[_pckm.parentKcliqIndex[i]] != _childPat.nts[_pckm.childKcliqIndex[i]])
			return false;
	}

	/*unsigned int j = 0, m = 0, n = 0;
	for(i = 0; i < _pckm.parentKcliqIndex.size() - 1; i++){
		for(j = i + 1; j < _pckm.parentKcliqIndex.size(); j++){
			for(m = 0; m < _parentPat.pat.size(); m++)
				if(_parentPat.pat[m].e.left == _pckm.parentKcliqIndex[i] + 1
						&& _parentPat.pat[m].e.right == _pckm.parentKcliqIndex[j] + 1)
					break;
			for(n = 0; n < _childPat.pat.size(); n++)
				if(_childPat.pat[n].e.left == _pckm.childKcliqIndex[i] + 1
						&& _childPat.pat[n].e.right == _pckm.childKcliqIndex[j] + 1)
					break;
			if(_parentPat.pat[m].interType != _childPat.pat[n].interType)
				return false;
		}
	}*/
	return true;
}


bool dp_builder::bagCompat_nt_merged(parent_child_kcliq_match& _pckm, pattern& _parentPat, pattern& _childPat){
	#if COMPAT_PRINT == 1
		cout << "parent pat: ";
		_parentPat.print();
		cout << "child pat before: ";
		_childPat.print();
	#endif

	unsigned int i = 0;
	string key = "";
	for(i = 0; i < _pckm.parentKcliqIndex.size(); i++){
		key = "";
		key += _childPat.nts[_pckm.childKcliqIndex[i]];
		key += _parentPat.nts[_pckm.parentKcliqIndex[i]];
		if( !this->ntCompatMatrixMap[key].isCompat )
			return false;
		_childPat.nts[_pckm.childKcliqIndex[i]] = this->ntCompatMatrixMap[key].childNt;
		_parentPat.nts[_pckm.parentKcliqIndex[i]] = this->ntCompatMatrixMap[key].parentNt;
	}

	unsigned int j = 0, m = 0, n = 0;
	for(i = 0; i < _pckm.parentKcliqIndex.size() - 1; i++){
		for(j = i + 1; j < _pckm.parentKcliqIndex.size(); j++){
			for(m = 0; m < _parentPat.pat.size(); m++)
				if(_parentPat.pat[m].e.left == _pckm.parentKcliqIndex[i] + 1
						&& _parentPat.pat[m].e.right == _pckm.parentKcliqIndex[j] + 1)
					break;
			for(n = 0; n < _childPat.pat.size(); n++)
				if(_childPat.pat[n].e.left == _pckm.childKcliqIndex[i] + 1
						&& _childPat.pat[n].e.right == _pckm.childKcliqIndex[j] + 1)
					break;
			if(_parentPat.pat[m].interType != _childPat.pat[n].interType)
				return false;
		}
	}

	#if COMPAT_PRINT == 1
		cout << "child pat after: ";
		_childPat.print();
	#endif
	return true;
}


//Note that _rootPatIndex and _childPatIndices are indices of positivePatIndices, not the indices of patterns
void dp_builder::traceback(clique_key* _root,
							short _rootPatIndex,
							vector<short>& _childPatIndices,
							bktree_scanner& _bktree,
							bPat_db& _bpatDb)
{
	bag_patterns bagPatterns = bag_patterns();
	pattern pat = pattern();
	string tempInter = "";
	bagPatterns = _bpatDb.bagPatsBktree[_root->patternKey];
	pat = bagPatterns.bPats[this->positivePatIndices[_root->indexInReverseDepthOrdering][_rootPatIndex]];
	for(unsigned int m = 0; m < pat.nts.size(); m++)
		this->optSeq[0][_root->cliq[m]] = pat.nts[m];
	for(unsigned int m = 0; m < pat.pat.size(); m++){
		if(pat.pat[m].interType[0] != ""){
			tempInter = IntToString(_root->cliq[pat.pat[m].e.left-1]);
			tempInter += "_" + IntToString(_root->cliq[pat.pat[m].e.right-1]);
			for(unsigned int n = 0; n < pat.pat[m].interType.size(); n++)
				tempInter += "_" + pat.pat[m].interType[n];
			this->optInters[0].insert(tempInter);
		}
	}

	#if TRACEBACK_PRINT == 1
		_root->print();
		pat.print();
		cout << "--------------------------------------------" << endl;
	#endif

	for(unsigned int m = 0; m < _root->children.size(); m++){
		if(_root->children[m]->children.size() > 0 && this->scoreVector[_root->children[m]->indexInReverseDepthOrdering]
					[this->positivePatIndices[_root->children[m]->indexInReverseDepthOrdering][_childPatIndices[m]]].childPatternNums.size() == 0)
			continue;

		/*#if TRACEBACK_PRINT == 1
			cout << "Child " << m << ": " << endl;
			_root->children[m]->print();
			this->scoreVector[_root->children[m]->indexInReverseDepthOrdering][this->positivePatIndices[_root->children[m]->indexInReverseDepthOrdering][_childPatIndices[m]]].print();
			cout << "--------------------------------------------" << endl;
		#endif*/

		this->scoreVector[_root->children[m]->indexInReverseDepthOrdering][this->positivePatIndices[_root->children[m]->indexInReverseDepthOrdering][_childPatIndices[m]]].childPatternNums.resize(1);
		this->traceback(_root->children[m], _childPatIndices[m],
				this->scoreVector[_root->children[m]->indexInReverseDepthOrdering]
				[this->positivePatIndices[_root->children[m]->indexInReverseDepthOrdering][_childPatIndices[m]]].childPatternNums[0], _bktree, _bpatDb);
	}
}


void dp_builder::traceback_nt_merged(short _optSolIndex, clique_key* _root,
				short _rootPatIndex,
				vector<short>& _childPatIndices)
{
	//bag_patterns bagPatterns = bag_patterns();
	pattern pat = pattern();
	string tempInter = "";
	//bagPatterns = _bpatDb.bagPatsBktree[_root->patternKey];
	pat = this->positivePats[_root->indexInReverseDepthOrdering][_rootPatIndex];
	//pat.print();

	for(unsigned int m = 0; m < pat.nts.size(); m++){
		/*if(_root->cliq[m] == 14){
			_root->print();
			pat.print();
			cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl << endl;
		}*/
		this->optSeq_nt_merged[_optSolIndex][_root->cliq[m]].push_back(pat.nts[m]);
	}

	for(unsigned int m = 0; m < pat.pat.size(); m++){
		if(pat.pat[m].interType[0] != ""){
			tempInter = IntToString(_root->cliq[pat.pat[m].e.left-1]);
			tempInter += "_" + IntToString(_root->cliq[pat.pat[m].e.right-1]);
			for(unsigned int n = 0; n < pat.pat[m].interType.size(); n++)
				tempInter += "_" + pat.pat[m].interType[n];
			this->optInters[_optSolIndex].insert(tempInter);
		}
	}

	#if TRACEBACK_PRINT == 1
		_root->print();
		pat.print();
		cout << "--------------------------------------------" << endl;
	#endif

	for(unsigned int m = 0; m < _root->children.size(); m++){
		if(_root->children[m]->children.size() > 0 && this->scoreVector[_root->children[m]->indexInReverseDepthOrdering][_childPatIndices[m]].childPatternNums.size() == 0)
			continue;

		/*#if TRACEBACK_PRINT == 1
			cout << "Child " << m << ": " << endl;
			_root->children[m]->print();
			this->scoreVector[_root->children[m]->indexInReverseDepthOrdering][this->positivePatIndices[_root->children[m]->indexInReverseDepthOrdering][_childPatIndices[m]]].print();
			cout << "--------------------------------------------" << endl;
		#endif*/

		//this->scoreVector[_root->children[m]->indexInReverseDepthOrdering][this->positivePatIndices[_root->children[m]->indexInReverseDepthOrdering][_childPatIndices[m]]].childPatternNums.resize(1);
		this->traceback_nt_merged(_optSolIndex, _root->children[m], _childPatIndices[m],
				this->scoreVector[_root->children[m]->indexInReverseDepthOrdering][_childPatIndices[m]].childPatternNums[0]);
	}
}


void dp_builder::opt_to_file(const char* _ntsFilePath, const char* _interFilePath){
	fstream outputStream(_ntsFilePath, ios::out);
	outputStream << this->optSeq[0];
	outputStream.close();

	outputStream.open(_interFilePath, ios::out);
	for(set<string>::iterator it1=this->optInters[0].begin(); it1!=this->optInters[0].end(); ++it1)
		outputStream << *it1 << endl;
	//outputStream << endl;
	outputStream.close();
	return;
}


void dp_builder::opt_to_file_merged(const char* _ntsFilePath, const char* _interFilePath){
	string wholeName = "";
	for(unsigned int l = 0; l < this->optSeq_nt_merged.size(); l++){
		wholeName = string(_ntsFilePath) + "_" + IntToString(l);
		fstream outputStream(wholeName.c_str(), ios::out);
		for(unsigned int i = 0; i < this->optSeq_nt_merged[l].size(); i++){
			outputStream << '[';
			outputStream << this->optSeq_nt_merged[l][i];
			outputStream << ']';
		}
		outputStream.close();
	}

	for(unsigned int l = 0; l < this->optInters.size(); l++){
		wholeName = string(_interFilePath) + "_" + IntToString(l);
		fstream outputStream(wholeName.c_str(), ios::out);
		for(set<string>::iterator it1 = this->optInters[l].begin(); it1 != this->optInters[l].end(); ++it1)
			outputStream << *it1 << endl;
		//outputStream << endl;
		outputStream.close();
	}
	return;
}


void dp_builder::read_nt_compat_matrix(void){
	fstream inputStream("./parameters/nt_compatible_matrix.txt", ios::in);
	if(! inputStream){
		cout << "Error: the nt_compatible_matrix file is not found in psrc directory." << endl;
		exit(0);
	}
	char temp = '\0';
	string key = "";
	string tempMergedNt = "";
	nt_compat_matrix_value tempValue = nt_compat_matrix_value();
	while(!inputStream.eof()){
		key = "";
		inputStream >> temp;
		key += temp;
		inputStream >> temp;
		key += temp;
		//cout << key << ' ';
		inputStream >> temp;
		tempValue = nt_compat_matrix_value();
		if(temp == '1')
			tempValue.isCompat = true;
		inputStream >> tempMergedNt;
		tempValue.childNt = tempMergedNt.at(0);
		inputStream >> tempMergedNt;
		tempValue.parentNt = tempMergedNt.at(0);
		//tempValue.print();
		this->ntCompatMatrixMap[key] = tempValue;
	}
	inputStream.close();
}


dp_builder::~dp_builder(void){
	this->scoreVector.clear();
	this->optSeq.clear();
	this->optInters.clear();
}
