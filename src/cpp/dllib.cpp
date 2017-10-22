/*
 * dllib.cpp
 *
 *  Created on: Aug 21, 2015
 *      Author: Liang Ding
 */

#include "dllib.h"
#include <vector>

//String split. The first puts the results in a pre-constructed vector, the second returns a new vector.
std::vector<std::string>& split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}


std::string IntToString(int number){
	  std::ostringstream oss;
	  oss << number;
	  return oss.str();
}


std::string vectorGenerator(std::vector<short>& _baseVec,
								int _relaxDis,
								std::vector<short>& _currentVec,
								int& _workingIndex)
{
	//for(unsigned int i = 0; i < _currentVec.size(); i++)
	//	std::cout << _currentVec[i] << ' ';
	//std::cout << std::endl;
	if(_workingIndex == -1){
		_workingIndex++;
	}
	else{
		while(_currentVec[_workingIndex] >= _baseVec[_workingIndex] + _relaxDis){
			_workingIndex++;
			if(_workingIndex >= int(_baseVec.size()))
				return "Done";
		}
		_currentVec[_workingIndex]++;
		for(int i = 0; i < _workingIndex; i++)
			_currentVec[i] = _baseVec[i];
		_workingIndex = 0;
	}
	return "Unfinished";
}


// vector enumerator. For example, enumerate from (0, 0, 0) to (5, 9, 6)
// 		base vector is (0, 0, 0), _relaxDis is (5, 9, 6), _currentVec stores the vector that
//		is currently enumerating.
std::string vectorGenerator_vary_len(std::vector<short>& _baseVec,
								std::vector<short>& _relaxDis,
								std::vector<short>& _currentVec,
								int& _workingIndex)
{
	if(_workingIndex == -1){
		_workingIndex++;
	}
	else{
		while(_currentVec[_workingIndex] >= _baseVec[_workingIndex] + _relaxDis[_workingIndex]){
			_workingIndex++;
			if(_workingIndex >= int(_baseVec.size()))
				return "Done";
		}
		_currentVec[_workingIndex]++;
		for(int i = 0; i < _workingIndex; i++)
			_currentVec[i] = _baseVec[i];
		_workingIndex = 0;
	}
	return "Unfinished";
}
