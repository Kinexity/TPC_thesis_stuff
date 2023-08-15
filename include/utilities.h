#pragma once
#ifndef utilities_h
#define utilities_h
#include <string>
#include <iostream>
#include <chrono>
#include <vector>
#include <cmath>
#include <format>

inline std::string filename_string(std::string path_str) {
	return path_str.substr(path_str.rfind("\\") + 1, path_str.size() - path_str.rfind("\\") - 1);
};

#define _endl_ " (" << filename_string(__FILE__) << "; " << __LINE__ << ")" << '\n'
#define checkpoint std::cout << "checkpoint" << _endl_

//using namespace std;

std::string get_date_string();

std::vector<double> nums(double start, double end, size_t N);

#endif // !utilities_h
