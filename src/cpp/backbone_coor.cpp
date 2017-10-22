/*
 * backbone_coor.cpp
 *
 *  Created on: Aug 17, 2015
 *      Author: Liang Ding
 */

#include <iostream>
#include <fstream>
#include <cstdlib>
#include "backbone_coor.h"

using namespace std;

coor::coor(void){
	this->x = 0.0;
	this->y = 0.0;
	this->z = 0.0;
}

void coor::print(void){
	cout << this->x << ' ' << this->y << ' ' << this->z << endl;
}

coor::~coor(void){}




backbone_coor::backbone_coor(void){
	this->seqLen = 0;
	this->coors.clear();
}

int backbone_coor::getSeqLen(void){
	return this->seqLen;
}

coor backbone_coor::getCoors(int _index){
	if(_index < 0 || _index >= int(this->coors.size())){
		cout << "Error: Invalid index for getCoors()." << endl;
		exit(0);
	}
	return this->coors[_index];
}

void backbone_coor::readBbc(const char* _bbcFile){
	std::fstream inputfile(_bbcFile, std::ios::in);
	if(! inputfile){
		cout << "Error: coordinate file does not exist.\n" << endl;
		exit(0);
	}

	while(inputfile.good()){
		coor c = coor();
		inputfile >> c.x;
		inputfile >> c.y;
		inputfile >> c.z;
		if(c.x == 0.0 && c.y == 0.0 && c.z == 0.0)
			break;
		this->coors.push_back(c);
	}
	this->seqLen = int(this->coors.size());
	//this->print();
}

void backbone_coor::print(void){
	for (vector<coor>::iterator it = this->coors.begin() ; it != this->coors.end(); ++it)
		it->print();
}

backbone_coor::~backbone_coor(void){
	this->coors.clear();
}
