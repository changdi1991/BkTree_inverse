/*
 * bktree_scanner.cpp
 *
 *  Created on: Aug 5, 2015
 *      Author: Liang Ding
 */

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <stack>
#include <sstream>
#include <cmath>
#include "bktree_scanner.h"

using namespace std;

bktree_scanner::bktree_scanner(void){
	this->elapseTime = 0.0;
	this->totalWeight = 0;
	this->maxWeight = 0;
	this->root = NULL;
	this->depth = 0;
	this->numCliques = 0;
	return;
}

bktree_scanner::bktree_scanner(const char* bktreeFile){
	this->elapseTime = 0.0;
	this->totalWeight = 0;
	this->maxWeight = 0;
	this->depth = 0;
	this->root = NULL;
	this->numCliques = 0;

	std::fstream inputfile(bktreeFile, std::ios::in);
	if(! inputfile){
		cout << "Error in bktree_scanner: bktree file not found.\n" << endl;
		exit(0);
	}

	//read the number of vertices
	if(inputfile.eof()){
		cout << "Error in bktree_scanner: \
					 input bktree file is an empty file.\n" << endl;
		exit(0);
	}//end if

	std::string temp;

	for(int i = 0; i < 2; i++)
		inputfile >> temp;
	inputfile >> this->elapseTime;

	for(int i = 0; i < 5; i++)
			inputfile >> temp;
	inputfile >> this->totalWeight;

	stack<clique_key *> incompleteBranches;
	bool isRoot = true;
	int numChildren = 0;
	clique_key *parentCliq = NULL;
	clique_key *cliqKey = NULL;
	bool isBreak = false;
	while(inputfile.good()){
		cliqKey = new clique_key();
		inputfile.seekg(2, ios_base::cur);
		for(int i = 0; i < treewidth-1; i++){
			inputfile >> temp;
			if(temp == "[empty]"){
				isBreak = true;
				break;
			}
			cliqKey->cliq.push_back(atoi(temp.c_str()));
		}
		if(isBreak)
			break;

		this->numCliques++;
		inputfile >> temp;
		cliqKey->cliq.push_back(atoi(temp.substr(0, temp.length()-2).c_str()));
		inputfile >> temp;
		cliqKey->regions = temp.substr(0, temp.length()-1);
		inputfile >> temp;
		if(isRoot){ 	//read the maximum weight in the root clique
			this->maxWeight = atoi(temp.substr(0, temp.length()-1).c_str());
		}
		inputfile >> temp;
		cliqKey->cliq.push_back(atoi(temp.substr(0, temp.length()-1).c_str()));
		sort(cliqKey->cliq.begin(), cliqKey->cliq.end());

		if(!isRoot){	//assign child to parent
			cliqKey->depth = parentCliq->depth + 1;
			if(cliqKey->depth > this->depth)
				this->depth = cliqKey->depth;
			parentCliq->children.push_back(cliqKey);
			parent_child_kcliq_match pckm = parent_child_kcliq_match();
			for(unsigned int m = 0; m < parentCliq->cliq.size(); m++){
				for(unsigned int n = 0; n < cliqKey->cliq.size(); n++){
					if(parentCliq->cliq[m] == cliqKey->cliq[n]){
						pckm.parentKcliqIndex.push_back(m);
						pckm.childKcliqIndex.push_back(n);
						break;
					}
				}
			}
			parentCliq->childrenkCliqMatch.push_back(pckm);
		}
		if(isRoot){		//store root address
			this->root = cliqKey;
			this->root->depth = 1;
			isRoot = false;
		}

		inputfile >> temp;
		numChildren = this->cal_num_children(temp);

		if(numChildren > 1){
			for(int i = 0; i < numChildren-1; i++)
				incompleteBranches.push(cliqKey);
			parentCliq = cliqKey;
		}
		else if(numChildren == 1){
			parentCliq = cliqKey;
		}
		else if(numChildren == 0){
			if(incompleteBranches.size() == 0)
				break;
			parentCliq = incompleteBranches.top();
			incompleteBranches.pop();
		}
	}
	inputfile.close();
	//this->root->print_recursive();
	//cout << this->depth << endl;
	return;
}

clique_key* bktree_scanner::get_root(void){
	return this->root;
}

int bktree_scanner::cal_num_children(string _temp){
	if(_temp == "[empty]")
		return 0;
	string uniqueChildBranches = "";
	uniqueChildBranches.push_back(_temp[1]);
	bool isUnique = true;
	for(unsigned int i = 0; i < _temp.length(); i++){
		if(_temp[i] == ','){
			isUnique = true;
			for(unsigned int j = 0; j < uniqueChildBranches.size(); j++){
				if(_temp[i+1] == uniqueChildBranches[j]){
					isUnique = false;
					break;
				}
			}
			if(isUnique)
				uniqueChildBranches.push_back(_temp[i+1]);
		}
	}
	return int(uniqueChildBranches.size());
}


void bktree_scanner::depth_ordering(void){
	this->depthOrderCliques.resize(this->depth);
	this->depth_ordering_recur(this->root);
	int indexCounter = 0;
	for(int i = int(this->depthOrderCliques.size())-1; i > -1; i--){
		for(unsigned int j = 0; j < this->depthOrderCliques[i].size(); j++){
			this->depthOrderCliques[i][j]->indexInReverseDepthOrdering = indexCounter;
			indexCounter++;
		}
	}
}


void bktree_scanner::depth_ordering_recur(clique_key* _root){
	this->depthOrderCliques[_root->depth-1].push_back(_root);
	if(_root->children.size() > 0){
		for(unsigned int i = 0; i < _root->children.size(); i++)
			this->depth_ordering_recur(_root->children[i]);
	}
}


void bktree_scanner::depth_ordering_print(void){
	cout << "Number of cliques: " << this->numCliques << endl;
	for(unsigned int i = 0; i < this->depthOrderCliques.size(); i++){
		cout << "Depth " << i << ": " << endl;
		for(unsigned int j = 0; j < this->depthOrderCliques[i].size(); j++){
			this->depthOrderCliques[i][j]->print();
		}
		cout << endl << endl;
	}
}


void bktree_scanner::depth_ordering_print_reverse(void){
	cout << "Number of cliques: " << this->numCliques << endl;
	for(int i = int(this->depthOrderCliques.size())-1; i > -1; i--){
		cout << "Depth " << i << ": " << endl;
		for(unsigned int j = 0; j < this->depthOrderCliques[i].size(); j++){
			this->depthOrderCliques[i][j]->print();
		}
		cout << endl << endl;
	}
}


int bktree_scanner::get_num_cliques(void){
	return this->numCliques;
}


std::vector< std::vector< clique_key* > > bktree_scanner::get_depth_order_cliques(void){
	return this->depthOrderCliques;
}

string bktree_scanner::IntToString(int number){
  ostringstream oss;
  oss << number;
  return oss.str();
}

double bktree_scanner::distance_3d(coor& c1, coor& c2){
	double x = c2.x - c1.x;
	double y = c2.y - c1.y;
	double z = c2.z - c1.z;
	return sqrt(x*x + y*y + z*z);
}

int bktree_scanner::remove_large_distance(int dis){
	if(dis > 4)
		return 4;
	return dis;
}

int bktree_scanner::modify_3D_distance(int dis){
	if(dis > 20)
		return 20;
	else if(dis < 3)
		return 3;
	return dis;
}

void bktree_scanner::prepare_pattern_key(backbone_coor& _bbc){
	coor tempLeft;
	coor tempRight;
	string patternKey = "";
	for(int i = int(this->depthOrderCliques.size())-1; i > -1; i--){ //traverse the cliques bottom-up from leaves
		for(unsigned int j = 0; j < this->depthOrderCliques[i].size(); j++){
			//this->depthOrderCliques[i][j]->print();
			patternKey = '[';
			for(unsigned int m = 0; m < this->depthOrderCliques[i][j]->cliq.size()-1; m++){
				for(unsigned int n = m+1; n < this->depthOrderCliques[i][j]->cliq.size(); n++){
					tempLeft = _bbc.getCoors(this->depthOrderCliques[i][j]->cliq[m]);
					tempRight = _bbc.getCoors(this->depthOrderCliques[i][j]->cliq[n]);
					patternKey += this->IntToString(this->modify_3D_distance(int(this->distance_3d(tempLeft, tempRight))));
					patternKey += ", ";
				}
			}
			patternKey = patternKey.substr(0, patternKey.size()-2) + "][";
			for(unsigned int m = 0; m < this->depthOrderCliques[i][j]->cliq.size()-1; m++){
				patternKey += this->IntToString(this->remove_large_distance(this->depthOrderCliques[i][j]->cliq[m+1]
						      - this->depthOrderCliques[i][j]->cliq[m]));
				patternKey += ", ";
			}
			patternKey = patternKey.substr(0, patternKey.size()-2) + ']';
			//cout << patternKey << endl << endl;
			this->depthOrderCliques[i][j]->patternKey = patternKey;
		}
	}
}

bktree_scanner::~bktree_scanner(void){}
