/*
 * clique_key.h
 *
 * A clique of a backbone k-tree
 *
 *  Created on: Aug 5, 2015
 *      Author: Liang Ding
 */

#ifndef SRC_INCLUDE_CLIQUE_KEY_H_
#define SRC_INCLUDE_CLIQUE_KEY_H_

#include <iostream>
#include <string>
#include <vector>
#include "BkTree_Inverse.h"

using namespace std;

class parent_child_kcliq_match{
public:
	vector<short> parentKcliqIndex;
	vector<short> childKcliqIndex;

	parent_child_kcliq_match(void);
	void print(void);
	~parent_child_kcliq_match(void);
};


class clique_key{
public:
	clique_key(void);

	vector<short> cliq;

	vector<clique_key*> children;

	vector< parent_child_kcliq_match > childrenkCliqMatch;

	//depth of the clique in a bktree. Root has depth 1.
	short depth;

	// 1/0 represents open/close of a region
	string regions;

	//To be used in dp_builder
	int indexInReverseDepthOrdering;

	//key to access its patterns
	string patternKey;

	void print(void);

	void print_recursive(void);

	~clique_key(void);
};


#endif /* SRC_INCLUDE_CLIQUE_KEY_H_ */
