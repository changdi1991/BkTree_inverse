/*
 * cliq_value.h
 *
 *  Created on: Aug 18, 2015
 *      Author: root
 */

#ifndef SRC_INCLUDE_CLIQ_VALUE_H_
#define SRC_INCLUDE_CLIQ_VALUE_H_

#include <vector>

class cliq_value{
public:
	cliq_value(void);

	//store certain number of best solutions
	std::vector<float> score;

	//std::vector<short> cliq;

	//std::vector<std::string> patternKey;

	//std::vector<int> indexInReverseDepthOrdering;

	//std::vector<int> selfPatternNums;

	std::vector< std::vector<short> > childPatternNums;

	void print(void);

	~cliq_value(void);
};



#endif /* SRC_INCLUDE_CLIQ_VALUE_H_ */
