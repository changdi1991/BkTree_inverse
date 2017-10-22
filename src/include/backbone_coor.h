/*
 * backbone_coor.h
 *
 *  Created on: Aug 17, 2015
 *      Author: Liang Ding
 */

#ifndef SRC_INCLUDE_BACKBONE_COOR_H_
#define SRC_INCLUDE_BACKBONE_COOR_H_

#include <vector>

class coor{
public:
	coor(void);
	double x;
	double y;
	double z;
	void print(void);
	~coor(void);
};

class backbone_coor{
private:
	int seqLen;

	std::vector<coor> coors;
public:
	backbone_coor(void);

	int getSeqLen(void);

	//read backbone coordinates from a file
	void readBbc(const char* _bbcFile);

	//return a coor of a given index
	coor getCoors(int _index);

	void print(void);

	~backbone_coor(void);
};


#endif /* SRC_INCLUDE_BACKBONE_COOR_H_ */
