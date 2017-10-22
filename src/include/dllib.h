/*
 * dllib.h
 *
 *  Created on: Aug 21, 2015
 *      Author: Liang Ding
 *
 *      Liang Ding's C++ library
 */

#ifndef SRC_INCLUDE_DLLIB_H_
#define SRC_INCLUDE_DLLIB_H_

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <algorithm>

//String split. The first puts the results in a pre-constructed vector, the second returns a new vector.
std::vector<std::string>& split(const std::string &s, char delim, std::vector<std::string> &elems);

std::vector<std::string> split(const std::string &s, char delim);


// integer to a string
std::string IntToString(int number);

// vector enumerator. For example, enumerate from (0, 0, 0) to (5, 5, 5)
// 		base vector is (0, 0, 0), _relaxDis is 5, _currentVec stores the vector that
//		is currently enumerating.
std::string vectorGenerator(std::vector<short>& _baseVec,
								int _relaxDis,
								std::vector<short>& _currentVec,
								int& _workingIndex);

// vector enumerator. For example, enumerate from (0, 0, 0) to (5, 9, 6)
// 		base vector is (0, 0, 0), _relaxDis is (5, 9, 6), _currentVec stores the vector that
//		is currently enumerating.
std::string vectorGenerator_vary_len(std::vector<short>& _baseVec,
								std::vector<short>& _relaxDis,
								std::vector<short>& _currentVec,
								int& _workingIndex);


#endif /* SRC_INCLUDE_DLLIB_H_ */
