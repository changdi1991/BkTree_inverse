/*
 * main.cpp
 *
 *  Created on: May 4, 2015
 *      Author: Liang Ding
 */

#include <iostream>
#include <ctime>
#include <iomanip>
#include <cstring>
#include <cstdlib>
#include "BkTree_Inverse.h"
#include "bktree_scanner.h"
#include "pattern.h"
#include "backbone_coor.h"
#include "dp_builder.h"

using namespace std;

#define DEBUG 0

short treewidth = 3;

void Usage(void);

int main(int argc, char *argv[]){
	clock_t start, finish;
	double totaltime;
	start=clock();

	char* bbcFile = (char*) "";
	char *bktreeFile = (char*) "";
	char* patternFile = (char*) "";
	char* mergedPatternFile = (char*) "";
	char *seqFile = (char*) "";
	char *interFile = (char*) "";
	int patternLimit = 1000;
	int numSols = 100;
	bool useBestScore = false;

	if(argc == 1){
		Usage();
		exit(0);
	}

	for(int i = 0; i < argc; i++){
		if(argv[i][0] == '-'){
			if( strcmp(argv[i], "-bbc") == 0 ){
				if((i+1) >= argc){	Usage();	}
					bbcFile = argv[++i];
			}
			else if( strcmp(argv[i], "-bktree") == 0 ){
				if((i+1) >= argc){	Usage();	}
				bktreeFile = argv[++i];
			}
			else if( strcmp(argv[i], "-pl") == 0 ){
				if((i+1) >= argc){	Usage();	}
				patternLimit = atoi(argv[++i]);
			}
			else if( strcmp(argv[i], "-pa") == 0 ){
				if((i+1) >= argc){	Usage();	}
				patternFile = argv[++i];
			}
			else if( strcmp(argv[i], "-mpa") == 0 ){
				if((i+1) >= argc){	Usage();	}
				mergedPatternFile = argv[++i];
			}
			else if( strcmp(argv[i], "-sol") == 0 ){
				if((i+1) >= argc){	Usage();	}
				numSols = atoi(argv[++i]);
			}
			else if( strcmp(argv[i], "-bs") == 0 ){
				useBestScore = true;
			}
			else if( strcmp(argv[i], "-seq") == 0 ){
				if((i+1) >= argc){	Usage();	}
				seqFile = argv[++i];
			}
			else if( strcmp(argv[i], "-inter") == 0 ){
				if((i+1) >= argc){	Usage();	}
				interFile = argv[++i];
			}
			else if( strcmp(argv[i], "-help") == 0 )
				Usage();
		}
	}

	if(strcmp(bbcFile, "") == 0){
		cout << "Error: input of backbone coordinates is required." << endl;
		exit(0);
	}
	if(strcmp(bktreeFile, "") == 0){
		cout << "Error: input of a backbone 3-tree is required." << endl;
		exit(0);
	}
	if(strcmp(patternFile, "") == 0 && strcmp(mergedPatternFile, "") == 0){
		cout << "Error: input of a pattern or merged pattern database is required." << endl;
		exit(0);
	}

	backbone_coor bbc = backbone_coor();
	bbc.readBbc(bbcFile);
	bktree_scanner bktree = bktree_scanner(bktreeFile);
	bktree.depth_ordering();
	bktree.prepare_pattern_key(bbc);
	//bktree.depth_ordering_print_reverse();

	bPat_db bpatDb = bPat_db();
	//bpatDb.readPatDB(patternFile);
	//bpatDb.readPatBktree(patternFile, bktree, 10, 1);

	if(strcmp(patternFile, "") != 0){
		bpatDb.readPatBktree2(bktree, 50, 1);
		bpatDb.patBktree_toFile(patternFile);
	}

	if(strcmp(mergedPatternFile, "") != 0){
		if(strcmp(seqFile, "") == 0){
			cout << "Error: an output sequence file is required." << endl;
			exit(0);
		}
		if(strcmp(interFile, "") == 0){
			cout << "Error: an output interaction file is required." << endl;
			exit(0);
		}

		bpatDb.read_nt_merged_pat(mergedPatternFile);
		dp_builder dpBuilder = dp_builder();
		dpBuilder.set_pattern_limit(patternLimit);
		dpBuilder.dp(bbc, bktree, bpatDb, numSols, useBestScore);
		//dpBuilder.opt_to_file(seqFile, interFile);
		dpBuilder.opt_to_file_merged(seqFile, interFile);
	}

	finish=clock();
	totaltime=(double)(finish-start)/CLOCKS_PER_SEC;
	std::cout << "Elapse time: " << totaltime << " secs." << std::endl;
	return 0;
};


void Usage(void){
	cout << "Usage: Bktree_Inverse [-bbc] bbcFile [-bktree] bktreeFile [other options]" << endl
			<< setw(11) << left << "[-bbc]" << "backbone coordinate file that store coordinates of C1 atoms [Required]" << endl
			<< setw(11) << left << "[-bktree]" << "backbone k-tree produced by the mspkt program [Required]" << endl
			<< setw(11) << left << "[-pa]" << "output candidate file of the 3-tree [Optional]" << endl
			<< setw(11) << left << "[-mpa]" << "merged candidate file of the 3-tree [Optional]" << endl
			<< setw(11) << left <<" [-pl]" << "limit of number of candidates for a bag [Optional] [DEFAULT=200]" << endl
			<< setw(11) << left <<" [-sol]" << "number of solutions [Optional] [DEFAULT=10]" << endl
			<< setw(11) << left << "[-seq]" << "sequence file [Required]" << endl
			<< setw(11) << left << "[-inter]" << "interaction file [Required]" << endl
			<< setw(11) << left << "[-help]" << "display usage information" << endl;
	exit(-1);
	return;
};

