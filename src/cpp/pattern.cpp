/*
 * pattern.cpp
 *
 *  Created on: Aug 14, 2015
 *      Author: Liang Ding
 */

using namespace std;

#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include "pattern.h"
#include "BkTree_Inverse.h"
#include "dllib.h"

edge::edge(void){
	this->left = 0;
	this->right = 0;
}

edge::edge(short _l, short _r){
	this->left = _l;
	this->right = _r;
}

edge::~edge(void){}




inter::inter(void){
	this->e = edge();
	this->interType.clear();
}

void inter::print(void){
	cout << '(' << this->e.left;
	cout << ' ';
	cout << this->e.right;
	cout << ' ';
	for(unsigned int i = 0; i < this->interType.size(); i++)
		cout << this->interType[i] << ' ';
	cout << ") " << endl;
}

inter::~inter(void){
	this->interType.clear();
}




pattern::pattern(void){
	this->score = 0;
	this->nts = "";
	this->pat.clear();
}

void pattern::print(void){
	cout << this->nts << endl;
	for(unsigned int i = 0; i < this->pat.size(); i++)
		this->pat[i].print();
	cout << "score: " << this->score << ' ';
	cout << std::endl << "###########################################" << endl << endl;
}

pattern::~pattern(void){
	this->pat.clear();
	this->nts.clear();
}

bool sortByScore(const pattern &lhs, const pattern &rhs){
	return lhs.score > rhs.score;
}



bag_patterns::bag_patterns(void){
	this->totalCount = 0;
	this->bPats.clear();
}

void bag_patterns::print(void){
	cout << "Frequency in database: " << this->totalCount << endl;
	cout << "Number of patterns: " << this->bPats.size() << endl;
	for(unsigned int i = 0; i < this->bPats.size(); i++){
		this->bPats[i].print();
		cout << endl;
	}
}

bag_patterns::~bag_patterns(void){
	this->bPats.clear();
}




bPat_db::bPat_db(void){
	int counter = 0;
	for(int i = 1; i < treewidth + 1; i++){
		for(int j = i + 1; j < treewidth + 2; j++){
			edge e = edge();
			e.left = i;
			e.right = j;
			indexEdgeMap[counter] = e;
			counter++;
		}
	}
}


/*void bPat_db::readPatDB(const char* _patFile){
	std::fstream inputfile(_patFile, std::ios::in);
	if(! inputfile){
		std::cout << "Error: the pattern file is not found." << std::endl;
		exit(0);
	}
	this->allBagPats.clear();

	char* temp;
	temp = new char[250000];
	string key = "";
	while(!inputfile.eof()){
		inputfile.getline(temp, 100, ':');
		if(temp[0] != '[')
			break;
		//cout << temp << endl;
		bag_patterns bp = bag_patterns();
		this->parseKey(temp, key, bp.totalCount);
		//cout << key << endl;
		inputfile.getline(temp, 250000, '\n');
		this->parseValue(temp, bp.bPats);
		//bp.print();
		this->allBagPats[key] = bp;
		//break;
	}
	delete temp;
	inputfile.close();
}*/


void bPat_db::parseKey(char* _temp, string& _key, int& _count){
	_key = "";
	string countStr = "";
	int counter = 0;
	int i = 0;
	while(_temp[i] != '\n'){
		if(_temp[i] == ']')
			counter++;
		_key += _temp[i];
		i++;
		if(counter == 2)
			break;
	}
	while(_temp[i] != ' '){
		countStr += _temp[i];
		i++;
	}
	_count = atoi(countStr.c_str());
}

void bPat_db::parseValue(char* _temp, vector<pattern>& _bPats){
	int i = 0;
	string tempMultiInter = "";
	string tempScore = "";
	int edgeCounter = 0;

	while(true){
		if(_temp[i] == ';' && _temp[i+1] == ' ' && int(_temp[i+2]) == 0)
			break;
		pattern pt = pattern();
		if(_temp[i] == ' ')
			i++;
		else if(_temp[i] == ';' && _temp[i+1] == ' '){
			i++;
			i++;
		}
		while(_temp[i] != '['){
			pt.nts += _temp[i];
			i++;
		}
		edgeCounter = 0;
		while(_temp[i] != ']'){
			while(_temp[i] != '\'')
				i++;
			i++;
			tempMultiInter = "";
			while(_temp[i] != '\''){
				tempMultiInter += _temp[i];
				i++;
			}
			inter ter = inter();
			this->parseMultiInters(tempMultiInter, ter.interType);
			ter.e = this->indexEdgeMap[edgeCounter];
			pt.pat.push_back(ter);
			//ter.print();
			i++;
			edgeCounter++;
		}
		i++;
		i++;
		tempScore = "";
		while(_temp[i] != ';'){
			tempScore += _temp[i];
			i++;
		}
		pt.score = atof(tempScore.substr(0, tempScore.length()-1).c_str());
		_bPats.push_back(pt);
	}
	return;
}

void bPat_db::parseMultiInters(string _multiInter, vector<string>& _inter){
	string tempSingleInter = "";
	for(unsigned int i = 0; i < _multiInter.length(); i++){
		if(_multiInter[i] == ','){
			_inter.push_back(tempSingleInter);
			tempSingleInter = "";
			i++;
		}
		else
			tempSingleInter += _multiInter[i];
	}
	_inter.push_back(tempSingleInter);
}


/*void bPat_db::readPatBktree(const char* _patFile,
						bktree_scanner& _bktree,
						int _relaxNumPatsCutoff,
						int _relaxDis)
{
	fstream inputfile(_patFile, ios::in);
	if(! inputfile){
		cout << "Error: the pattern file is not found." << endl;
		exit(0);
	}

	vector< vector< clique_key* > > depthOrderCliques = _bktree.get_depth_order_cliques();
	set<string> tempKeySet;
	for(int i = int(depthOrderCliques.size())-1; i > -1; i--){ //traverse the cliques bottom-up from leaves
		for(unsigned int j = 0; j < depthOrderCliques[i].size(); j++){
			tempKeySet.insert(depthOrderCliques[i][j]->patternKey);
		}
	}
	this->bagPatsBktree.clear();

	char* temp;
	temp = new char[250000];
	string key = "";
	set<string> relaxNeededKeySet;  //store the pattern keys that need to be relaxed
	while(!inputfile.eof()){
		inputfile.getline(temp, 100, ':');
		if(temp[0] != '[')
			break;
		bag_patterns bp = bag_patterns();
		this->parseKey(temp, key, bp.totalCount);
		inputfile.getline(temp, 250000, '\n');
		if(tempKeySet.find(key) != tempKeySet.end()){
			this->parseValue(temp, bp.bPats);
			this->bagPatsBktree[key] = bp;
			if(bp.totalCount < _relaxNumPatsCutoff){
				relaxNeededKeySet.insert(key);
			}
		}
	}
	delete temp;
	inputfile.close();

	for (set<string>::iterator it=tempKeySet.begin(); it!= tempKeySet.end(); ++it){
		if(this->bagPatsBktree[*it].totalCount == 0)
			relaxNeededKeySet.insert(*it);
	}

	this->relax2(relaxNeededKeySet, _relaxDis);
}*/


void bPat_db::readPatBktree2(bktree_scanner& _bktree,
						int _relaxNumPatsCutoff,
						int _relaxDis)
{
	vector< vector< clique_key* > > depthOrderCliques = _bktree.get_depth_order_cliques();
	set<string> tempKeySet;
	vector<string> tokens3dDisIndexDis(2);
	vector<string> tokens3dDis(6);
	vector<int> tokensBktreeInt(6);
	string patFileName, key;
	char* temp;

	this->bagPatsBktree.clear();
	for(int i = int(depthOrderCliques.size())-1; i > -1; i--){ //traverse the cliques bottom-up from leaves
		for(unsigned int j = 0; j < depthOrderCliques[i].size(); j++){
			tokens3dDisIndexDis = split(depthOrderCliques[i][j]->patternKey, ']');
			tokens3dDis = split(tokens3dDisIndexDis[0].substr(1, tokens3dDisIndexDis[0].size()), ',');
			tokensBktreeInt.clear();
			for(unsigned int t = 0; t < tokens3dDis.size(); t++)
				tokensBktreeInt.push_back(atoi(tokens3dDis[t].c_str()));
			patFileName = "";
			for(unsigned int t = 0; t < 2; t++)
				patFileName += IntToString(tokensBktreeInt[t]) + "_";
			patFileName += IntToString(tokensBktreeInt[2]) + ".txt";

			fstream inputfile(("./parameters/patternDb/" + patFileName).c_str(), ios::in);
			if(! inputfile){
				cout << "Error: the pattern file " << patFileName << " is not found." << endl;
				exit(0);
			}

			temp = new char[250000];
			key = "";
			while(!inputfile.eof()){
				inputfile.getline(temp, 100, ':');
				if(temp[0] != '[')
					break;
				bag_patterns bp = bag_patterns();
				this->parseKey(temp, key, bp.totalCount);
				inputfile.getline(temp, 250000, '\n');
				if(key == depthOrderCliques[i][j]->patternKey){
					this->parseValue(temp, bp.bPats);
					this->bagPatsBktree[key] = bp;
				}
			}
			delete temp;
			inputfile.close();
			tempKeySet.insert(depthOrderCliques[i][j]->patternKey);
		}
	}

	set<string> relaxNeededKeySet;  //store the pattern keys that need to be relaxed
	for (set<string>::iterator it=tempKeySet.begin(); it!= tempKeySet.end(); ++it){
		if(this->bagPatsBktree[*it].totalCount < _relaxNumPatsCutoff)
			relaxNeededKeySet.insert(*it);
	}

	int relaxCounter = 0;
	while(relaxNeededKeySet.size() > 0){
		relaxCounter++;
		cout << "Number of relaxing needed keys: " << relaxNeededKeySet.size() << endl;
		cout << "Candidate distance is relaxing " << relaxCounter << " times." << endl;
		this->relax2(relaxNeededKeySet, _relaxDis);
		_relaxDis++;
		if(relaxCounter > 3){ //relaxing distance cannot be greater than 3
			cout << "Relaxing stops: relaxing distance exceeds 3." << endl;
			break;
		}

		relaxNeededKeySet.clear();
		for (set<string>::iterator it=tempKeySet.begin(); it!= tempKeySet.end(); ++it){
			if(this->bagPatsBktree[*it].totalCount < _relaxNumPatsCutoff){
				//cout << *it << endl;
				//cout << this->bagPatsBktree[*it].bPats.size() << endl << endl;
				relaxNeededKeySet.insert(*it);
				this->bagPatsBktree[*it].bPats.clear();
				this->bagPatsBktree[*it].totalCount = 0;
			}
		}

		//cout << "################################################" << endl;
	}
}


/*void bPat_db::relax(const char* _patFile, set<string>& _relaxNeededKeySet, int _relaxDis){
	fstream inputfile(_patFile, ios::in);
	if(! inputfile){
		cout << "Error: the pattern file is not found." << endl;
		exit(0);
	}

	char* temp;
	temp = new char[250000];
	string key = "";
	while(!inputfile.eof()){
		inputfile.getline(temp, 100, ':');
		if(temp[0] != '[')
			break;
		bag_patterns bp = bag_patterns();
		this->parseKey(temp, key, bp.totalCount);
		inputfile.getline(temp, 250000, '\n');
		for (set<string>::iterator it=_relaxNeededKeySet.begin(); it!=_relaxNeededKeySet.end(); ++it){
			if(this->withinRelaxDis(key, *it, _relaxDis)){
				this->parseValue(temp, bp.bPats);
				for(unsigned int i = 0; i < bp.bPats.size(); i++){
					this->bagPatsBktree[*it].bPats.push_back(bp.bPats[i]);
					this->bagPatsBktree[*it].totalCount++;
				}
			}
		}
	}

	inputfile.close();
}*/


/*bool bPat_db::withinRelaxDis(string _keyDb, string _keyBktree, int _relaxDis){
	if(_keyDb == _keyBktree)
		return false;

	vector<string> tokens3dDisIndexDis = split(_keyDb, ']');
	vector<string> tokens3dDis = split(tokens3dDisIndexDis[0].substr(1, tokens3dDisIndexDis[0].size()), ',');
	vector<int> tokensDbInt(tokens3dDis.size());
	for(unsigned int i = 0; i < tokens3dDis.size(); i++){
		tokensDbInt.push_back(atoi(tokens3dDis[i].c_str()));
	}
	tokens3dDisIndexDis = split(_keyBktree, ']');
	tokens3dDis = split(tokens3dDisIndexDis[0].substr(1, tokens3dDisIndexDis[0].size()), ',');
	vector<int> tokensBktreeInt(tokens3dDis.size());
	for(unsigned int i = 0; i < tokens3dDis.size(); i++){
		tokensBktreeInt.push_back(atoi(tokens3dDis[i].c_str()));
	}

	for(unsigned int i = 0; i < tokensDbInt.size(); i++){
		if(tokensDbInt[i] > tokensBktreeInt[i] + _relaxDis)
			return false;
		if(tokensDbInt[i] < tokensBktreeInt[i] - _relaxDis)
			return false;
	}
	return true;
}*/


void bPat_db::relax2(set<string>& _relaxNeededKeySet, int _relaxDis){
	vector<string> tokens3dDisIndexDis(2);
	vector<string> tokens3dDis(6);
	vector<int> tokensBktreeInt(6);
	vector<int> currentVec(6);
	char* temp;
	int workingIndex;
	string currentKey;
	string patFileName = "";
	for (set<string>::iterator it=_relaxNeededKeySet.begin(); it!=_relaxNeededKeySet.end(); ++it){
		this->bagPatsBktree[*it].bPats.clear();
		this->bagPatsBktree[*it].totalCount = 0;
		tokens3dDisIndexDis = split(*it, ']');
		tokens3dDis = split(tokens3dDisIndexDis[0].substr(1, tokens3dDisIndexDis[0].size()), ',');
		tokensBktreeInt.clear();
		currentVec.clear();
		workingIndex = -1;
		for(unsigned int i = 0; i < tokens3dDis.size(); i++)
			tokensBktreeInt.push_back(atoi(tokens3dDis[i].c_str()));
		for(unsigned int i = 0; i < tokensBktreeInt.size(); i++){
			if(tokensBktreeInt[i] - _relaxDis < 3){
				//cout << tokensBktreeInt[i] << " " << _relaxDis << endl;
				//for(unsigned int i = 0; i < tokensBktreeInt.size(); i++)
				//	cout << tokensBktreeInt[i] << " ";
				//cout << endl;
				//cout << "Strange! 3D distance found to be less than 1." << endl;
				//exit(0);
				currentVec.push_back(3);
			}
			else
				currentVec.push_back(tokensBktreeInt[i] - _relaxDis);
		}
		while(true){
			currentKey = "";
			patFileName = this->key3DGenerator(tokensBktreeInt, _relaxDis, currentVec, workingIndex, currentKey);
			//cout << patFileName << endl;
			//for(unsigned int x = 0; x < currentVec.size(); x++)
			//	cout << currentVec[x] << ' ';
			//cout << endl;
			currentKey = currentKey + tokens3dDisIndexDis[1] + "]";
			//if(currentKey == *it)
			//	continue;
			if(patFileName == "Done")
				break;

			fstream inputfile(("./parameters/patternDb/" + patFileName).c_str(), ios::in);
			if(! inputfile){
				/*cout << *it << endl;
				cout << currentKey << endl;
				cout << "Error: the pattern file " << patFileName << " is not found." << endl;
				exit(0);*/
				continue;
			}

			temp = new char[250000];
			string key = "";
			while(!inputfile.eof()){
				inputfile.getline(temp, 100, ':');
				if(temp[0] != '[')
					break;
				bag_patterns bp = bag_patterns();
				this->parseKey(temp, key, bp.totalCount);
				inputfile.getline(temp, 250000, '\n');
				if(key == currentKey){
					this->bagPatsBktree[*it].totalCount += bp.totalCount;
					this->parseValue(temp, bp.bPats);
					for(unsigned int i = 0; i < bp.bPats.size(); i++)
						this->bagPatsBktree[*it].bPats.push_back(bp.bPats[i]);
				}
			}
			delete temp;

			inputfile.close();
		}
		sort(this->bagPatsBktree[*it].bPats.begin(), this->bagPatsBktree[*it].bPats.end(), sortByScore);
	}
}


string bPat_db::key3DGenerator(vector<int>& _baseVec, int _relaxDis, vector<int>& _currentVec, int& _workingIndex, string& _currentKey){
	if(_workingIndex == -1)
		_workingIndex++;
	else{
		while(_currentVec[_workingIndex] >= _baseVec[_workingIndex] + _relaxDis){
			_workingIndex++;
			if(_workingIndex >= int(_baseVec.size()))
				return "Done";
		}
		_currentVec[_workingIndex]++;
		for(int i = 0; i < _workingIndex; i++)
			_currentVec[i] = _baseVec[i] - _relaxDis;
		_workingIndex = 0;
	}

	/*for(unsigned int x = 0; x <_baseVec.size(); x++)
		cout << _baseVec[x] << ' ';
	cout << endl;
	for(unsigned int x = 0; x <_currentVec.size(); x++)
		cout << _currentVec[x] << ' ';
	cout << endl;
	cout << "_relaxDis: " << _relaxDis << endl;
	cout << "_workingIndex: " << _workingIndex << endl;*/
	//cout << endl;

	_currentKey = "[";
	for(unsigned int i = 0; i < _currentVec.size()-1; i++)
		_currentKey += IntToString(_currentVec[i]) + ", ";
	_currentKey += IntToString(_currentVec[_currentVec.size()-1]) + "]";

	string patFileName = "";
	for(unsigned int i = 0; i < 2; i++)
		patFileName += IntToString(_currentVec[i]) + "_";
	patFileName += IntToString(_currentVec[2]) + ".txt";
	return patFileName;
}


void bPat_db::patBktree_toFile(const char* _fileName){
	cout << "Write patterns to " << _fileName << endl;
	fstream outputStream(_fileName, ios::out);
	for(map<string, bag_patterns>::iterator it=this->bagPatsBktree.begin(); it!=this->bagPatsBktree.end(); ++it){
		outputStream << it->first;
		outputStream << it->second.totalCount << " : ";
		//outputStream << "Number of patterns: " << it->second.bPats.size() << endl;
		for(unsigned int i = 0; i < it->second.bPats.size(); i++){
			outputStream << it->second.bPats[i].nts << "[";
			for(unsigned int j = 0; j < it->second.bPats[i].pat.size(); j++){
				outputStream << "'";
				for(unsigned int m = 0; m < it->second.bPats[i].pat[j].interType.size(); m++){
					if(m == it->second.bPats[i].pat[j].interType.size() - 1)
						outputStream << it->second.bPats[i].pat[j].interType[m];
					else
						outputStream << it->second.bPats[i].pat[j].interType[m] << ", ";
				}
				if(j == it->second.bPats[i].pat.size() - 1)
					outputStream << "'";
				else
					outputStream << "', ";
			}
			outputStream << "] " << it->second.bPats[i].score << " ; ";
		}
		outputStream << endl;
	}
	outputStream.close();
}


void bPat_db::read_nt_merged_pat(const char* _ntMergedPatFile){
	fstream inputStream(_ntMergedPatFile, ios::in);
	if(! inputStream){
		cout << "Error: the pattern file is not found." << endl;
		exit(0);
	}
	this->bagPatsNtMerged.clear();

	char* temp;
	temp = new char[250000];
	string key = "";
	set<string> relaxNeededKeySet;  //store the pattern keys that need to be relaxed
	while(!inputStream.eof()){
		inputStream.getline(temp, 100, ':');
		if(temp[0] != '[')
			break;
		bag_patterns bp = bag_patterns();
		this->parseKey(temp, key, bp.totalCount);
		inputStream.getline(temp, 250000, '\n');
		this->parseValue(temp, bp.bPats);
		this->bagPatsNtMerged[key] = bp;
	}
	delete temp;
	inputStream.close();
	return;
}


bPat_db::~bPat_db(void){}
