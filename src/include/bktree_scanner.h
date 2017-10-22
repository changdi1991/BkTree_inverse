/*
 * bktree_scanner.h
 *
 *  Created on: Aug 5, 2015
 *      Author: Liang Ding
 */

#ifndef SRC_INCLUDE_BKTREE_SCANNER_H_
#define SRC_INCLUDE_BKTREE_SCANNER_H_

#include <string>
#include "clique_key.h"
#include "backbone_coor.h"

class bktree_scanner
{
private:
	std::string IntToString(int number);

	double distance_3d(coor&, coor&);

	int remove_large_distance(int dis);

	int modify_3D_distance(int dis);
private:
	//Elapse time used to build the 3-bb-tree
	double elapseTime;

	//Total weight
	int totalWeight;

	//Maximum weight of bktree
	int maxWeight;

	//Root clique
	clique_key *root;

	//number of cliques(nodes of bktree)
	int numCliques;

	//calculate number of chilren of a clique
	int cal_num_children(std::string);

	//depth of this bktree
	int depth;

	//Store reordered cliques in the bktree by the depth of the bktree.
	//Ordered cliques will be used in dynamic programming algorithm.
	std::vector< std::vector< clique_key* > > depthOrderCliques;
public:
	//default constructor
	bktree_scanner(void);

	//constructor with a file path as parameter to read a backbone k-tree
	bktree_scanner(const char*);

	clique_key* get_root(void);

	//Reorder the cliques in the bktree by the depth of the bktree.
	void depth_ordering(void);

	//helper function of depth_ording for recursive calls
	void depth_ordering_recur(clique_key *_root);

	//print each node of this bktree in depth ordering (breath-first)
	void depth_ordering_print(void);

	//print each node of this bktree in depth ordering (breath-first and leaf first)
	void depth_ordering_print_reverse(void);

	int get_num_cliques(void);

	std::vector< std::vector< clique_key* > > get_depth_order_cliques(void);

	//generate the key to retrieve the patterns from pat_db
	void prepare_pattern_key(backbone_coor& _bbc);

	~bktree_scanner(void);
};


#endif /* SRC_INCLUDE_BKTREE_SCANNER_H_ */
