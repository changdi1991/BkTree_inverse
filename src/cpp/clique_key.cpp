/*
 * clique_key.cpp
 *
 *  Created on: Aug 5, 2015
 *      Author: Liang Ding
 */

#include "clique_key.h"

using namespace std;

parent_child_kcliq_match::parent_child_kcliq_match(void){
	this->childKcliqIndex.clear();
	this->parentKcliqIndex.clear();
	this->parentKcliqIndex.reserve(3);
	this->childKcliqIndex.reserve(3);
}

void parent_child_kcliq_match::print(void){
	unsigned int i = 0;
	for(i = 0; i < this->parentKcliqIndex.size(); i++)
		cout << this->parentKcliqIndex[i] << ' ';
	cout << "| ";
	for(i = 0; i < this->childKcliqIndex.size(); i++)
			cout << this->childKcliqIndex[i] << ' ';
	cout << endl;
}

parent_child_kcliq_match::~parent_child_kcliq_match(void){
	this->childKcliqIndex.clear();
	this->parentKcliqIndex.clear();
}



clique_key::clique_key(void){
	this->cliq.clear();
	this->children.clear();
	this->regions.clear();
	this->depth = 0;
	this->indexInReverseDepthOrdering = 0;
	this->patternKey = "";
}

void clique_key::print(void){
	unsigned int i = 0;
	cout << '(';
	for(i = 0; i < this->cliq.size()-1; i++)
		cout << this->cliq[i] << ", ";
	cout << this->cliq[i];
	cout << ')' << this->regions << ' ' << this->depth << endl;
	cout << "Index: " << this->indexInReverseDepthOrdering << endl;
	//cout << endl;
	cout << "childrenkCliqMatch -- ";
	for(i = 0; i < this->childrenkCliqMatch.size(); i++){
		cout << "child " << i << ": ";
		this->childrenkCliqMatch[i].print();
	}
	cout << endl;
}

void clique_key::print_recursive(void){
	unsigned int i = 0;
	cout << '(';
	for(i = 0; i < this->cliq.size()-1; i++)
		cout << this->cliq[i] << ", ";
	cout << this->cliq[i];
	cout << ')' << this->regions << ' ' << this->depth << endl;
	cout << "Index: " << this->indexInReverseDepthOrdering << endl;
	if(this->children.size() == 0)
		cout << "No children." << endl;
	else{
		cout << "Children:" << endl;
		for(i = 0; i < this->children.size(); i++)
			this->children[i]->print_recursive();
	}
	//cout << endl;
}

clique_key::~clique_key(void){
	this->cliq.clear();
	this->children.clear();
	this->regions.clear();
}
