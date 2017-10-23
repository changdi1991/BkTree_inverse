# BkTree_inverse

BkTree_inverse is a software to predict RNA sequences from a given scaffold of a RNA 3D structure.

## Build program
cd build  
make clean  
make all

## Use binary
$ ./bin/BkTree_Inverse  (built with g++ 5.4.0)  
Usage: Bktree_Inverse [-bbc] bbcFile [-bktree] bktreeFile [other options]  
[-bbc]     backbone coordinate file that store coordinates of C1 atoms [Required]  
[-bktree]  backbone k-tree produced by the mspkt program [Required]  
[-pa]      output candidate file of the 3-tree [Optional]  
[-mpa]     merged candidate file of the 3-tree [Optional]  
[-pl]      limit of number of candidates for a bag [Optional] [DEFAULT=200]  
[-sol]     number of solutions [Optional] [DEFAULT=10]  
[-seq]     sequence file [Required]  
[-inter]   interaction file [Required]  
[-help]    display usage information  

## Usage Example
./bin/BkTree_Inverse -bbc ./inputs/2OIUP.bbc -bktree ./inputs/2OIUP.bktree -mpa ./parameters/patternDbDebug.txt -seq ./outputs/1DK1B.seq -inter ./outputs/1DK1B.inter
