/*
 * cliq_value.cpp
 *
 *  Created on: Aug 18, 2015
 *      Author: Liang Ding
 */

#include <iostream>
#include "cliq_value.h"

using namespace std;

cliq_value::cliq_value(void){
	this->score.clear();
	this->childPatternNums.clear();
}

void cliq_value::print(void){
	for(unsigned int i = 0; i < this->score.size(); i++){
		cout << "Score: " << this->score[i] << endl;
	}

	cout << "Child pattern numbers: ";
	for(unsigned int i = 0; i < this->childPatternNums.size(); i++){
		for(unsigned int j = 0; j < this->childPatternNums[i].size(); j++)
			cout << this->childPatternNums[i][j] << ' ';
	}
	cout << endl << endl;
}

cliq_value::~cliq_value(void){
	this->score.clear();
	this->childPatternNums.clear();
}
