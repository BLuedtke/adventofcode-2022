/**
 * @file main.cpp
 * @author Bernhard Luedtke
 * @brief 
 * @version 0.1
 * @date 2022-12-05
 * 
*/


#include <algorithm>
#include <iostream>
#include <fstream>
#include <ios>
#include <iomanip>
#include <numbers>
#include <numeric>
#include <chrono>
#include <vector>
#include <string>
#include <array>
#include <tuple>
#include <filesystem>
#include <memory>
#include <sstream>


using std::cout; //NOLINT
using std::get; //NOLINT
using namespace std::chrono;
using namespace std::chrono_literals;


// From https://stackoverflow.com/a/25829233
inline std::string& ltrim(std::string& s, const char* t = " \t\n\r\f\v") {
	s.erase(0, s.find_first_not_of(t));
	return s;
}

inline std::string get_indent(uint16_t indent_level) {
	std::string ret {};
	for (uint16_t i = 0; i < indent_level; ++i) {
		ret += "  ";
	}
	return ret;
}





int main(int argc, char **argv){
	
	cout << "------------------------------------------------------\n";
	
	return 0;
}
