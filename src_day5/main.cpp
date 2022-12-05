/**
 * @file main.cpp
 * @author Bernhard Luedtke
 * @brief 
 * @version 0.1
 * @date 2022-12-05
 * 
*/


#include <iostream>
#include <fstream>
#include <ios>
#include <iomanip>
#include <numbers>
#include <chrono>

#include <vector>
#include <string>
#include <array>
#include <tuple>
#include <filesystem>

using std::cout; //NOLINT
using std::get; //NOLINT
using namespace std::chrono;
using namespace std::chrono_literals;
constexpr size_t targetSize = 100000;



std::vector<std::tuple<uint16_t,uint16_t,uint16_t>> get_instructions_from_file(const std::filesystem::path& path) {
	std::ifstream instr_file{path, std::ios_base::in};
	std::vector<std::tuple<uint16_t,uint16_t,uint16_t>> instr {};
	//instr.reserve(512);
	if (instr_file.is_open()) {
		std::string nop_1 {};
		uint16_t count_n {};
		std::string nop_2 {};
		uint16_t from_n {};
		std::string nop_3 {};
		uint16_t to_n {};
		while (instr_file >> nop_1 >> count_n >> nop_2 >> from_n >> nop_3 >> to_n) {
			//cout << nop_1 << count_n << nop_2 << from_n << nop_3 << to_n << std::endl;
			instr.push_back({count_n, from_n, to_n});
		}
	}
	return instr;
}


void solve_9000(const std::vector<std::tuple<uint16_t,uint16_t,uint16_t>>& instruction_ref, std::vector<std::vector<char>>& stacks) {
	for (const auto instr : instruction_ref) {
		const auto number_to_move = get<0>(instr);
		const auto from = get<1>(instr) - 1;
		const auto to = get<2>(instr) - 1;
		for (uint16_t nbr = 0; nbr < number_to_move; ++nbr) {
			stacks.at(to).push_back(stacks.at(from).back());
			stacks.at(from).pop_back();
		}
	}
}

void solve_9001(const std::vector<std::tuple<uint16_t,uint16_t,uint16_t>>& instruction_ref, std::vector<std::vector<char>>& stacks) {
	for (const auto& instr : instruction_ref) {
		const auto number_to_move = get<0>(instr);
		auto& to_vec = stacks.at(get<2>(instr) - 1);
		auto& from_vec = stacks.at(get<1>(instr) - 1);
		to_vec.insert(to_vec.end(), std::make_move_iterator(from_vec.end() - number_to_move),
									std::make_move_iterator(from_vec.end()));
		from_vec.erase(from_vec.end()-number_to_move, from_vec.end());
	}
}

void print_stacks(const std::vector<std::vector<char>>& stacks) {
	for (size_t i = 0; i < stacks.size(); ++i) {
		cout << i << ":";
		for (size_t n = 0; n < stacks.at(i).size(); ++n) {
			cout << " [" << stacks.at(i).at(n) << "]";
		}
		cout << "\n";
	}
}

void print_solution(const std::vector<std::vector<char>>& stacks) {
	for (const auto& vec : stacks) {
		cout << vec.back();
	}
	cout << "\n";
}

int main(int argc, char **argv){
	const std::vector<std::tuple<uint16_t,uint16_t,uint16_t>> instructions_example {
		{1, 2, 1},
		{3, 1, 3},
		{2, 2, 1},
		{1, 1, 2}
	};
	std::vector<std::vector<char>> stacks_example {
		{'Z', 'N'},
		{'M', 'C', 'D'},
		{'P'}
	};

	const std::vector<std::tuple<uint16_t,uint16_t,uint16_t>> instructions {
		{3, 8, 9},
		{2, 2, 8},
		{5, 4, 2},
		{7, 1, 4},
		{3, 8, 2},
		{3, 2, 7},
		{1, 7, 4},
		{3, 2, 9},
		{4, 7, 9},
		{1, 5, 2},
		{2, 3, 4},
		{5, 9, 5},
		{6, 9, 3},
		{5, 9, 5},
		{1, 9, 7},
		{2, 3, 1},
		{7, 3, 9},
		{2, 7, 2},
		{5, 2, 4},
		{1, 2, 9},
		{2, 1, 9},
		{7, 6, 1},
		{2, 7, 3},
		{2, 3, 9},
		{1, 7, 4},
		{1, 9, 2},
		{3, 1, 8},
		{2, 3, 4},
		{5, 9, 2},
		{1, 3, 9},
		{8, 5, 7},
		{1, 6, 1},
		{15, 4, 1},
		{4, 2, 5},
		{5, 9, 7},
		{1, 9, 5},
		{5, 1, 2},
		{3, 8, 9},
		{1, 7, 6},
		{11, 1, 2},
		{7, 5, 3},
		{4, 2, 6},
		{7, 3, 4},
		{3, 5, 9},
		{2, 2, 5},
		{5, 1, 8},
		{2, 6, 8},
		{3, 8, 9},
		{9, 4, 9},
		{9, 7, 4},
		{2, 8, 1},
		{1, 8, 7},
		{6, 2, 7},
		{5, 2, 4},
		{5, 7, 2},
		{2, 1, 7},
		{2, 6, 4},
		{7, 7, 1},
		{3, 2, 6},
		{1, 8, 7},
		{2, 9, 3},
		{2, 3, 1},
		{1, 2, 5},
		{4, 6, 5},
		{2, 2, 3},
		{3, 5, 7},
		{1, 5, 3},
		{9, 1, 7},
		{2, 9, 5},
		{13, 4, 1},
		{5, 7, 2},
		{3, 3, 1},
		{2, 2, 9},
		{1, 2, 7},
		{5, 5, 6},
		{2, 2, 4},
		{5, 1, 3},
		{9, 7, 8},
		{2, 9, 5},
		{3, 5, 4},
		{5, 9, 2},
		{10, 4, 8},
		{1, 4, 1},
		{2, 8, 4},
		{4, 8, 2},
		{3, 6, 8},
		{7, 8, 7},
		{10, 9, 3},
		{7, 3, 2},
		{11, 2, 3},
		{13, 3, 9},
		{1, 6, 3},
		{1, 1, 2},
		{1, 2, 8},
		{3, 3, 4},
		{1, 2, 9},
		{1, 4, 1},
		{10, 8, 3},
		{11, 9, 7},
		{1, 6, 2},
		{14, 7, 1},
		{2, 2, 9},
		{4, 7, 6},
		{1, 2, 4},
		{3, 4, 2},
		{4, 2, 9},
		{10, 3, 4},
		{3, 6, 1},
		{5, 9, 5},
		{5, 5, 8},
		{1, 9, 7},
		{2, 9, 6},
		{1, 9, 8},
		{2, 4, 8},
		{1, 4, 5},
		{2, 3, 1},
		{2, 3, 7},
		{27, 1, 2},
		{2, 7, 1},
		{9, 4, 6},
		{9, 6, 5},
		{5, 8, 6},
		{26, 2, 3},
		{1, 2, 5},
		{1, 2, 7},
		{1, 8, 4},
		{1, 7, 8},
		{24, 3, 5},
		{1, 8, 5},
		{1, 4, 3},
		{1, 7, 1},
		{1, 8, 9},
		{7, 1, 7},
		{8, 6, 4},
		{4, 7, 6},
		{1, 3, 9},
		{2, 9, 1},
		{3, 7, 9},
		{8, 4, 6},
		{3, 9, 1},
		{1, 3, 6},
		{1, 8, 2},
		{10, 5, 4},
		{1, 3, 8},
		{13, 5, 3},
		{1, 2, 9},
		{1, 8, 9},
		{1, 3, 8},
		{1, 9, 2},
		{3, 6, 9},
		{7, 4, 9},
		{4, 3, 9},
		{2, 6, 8},
		{2, 4, 5},
		{10, 9, 3},
		{1, 1, 9},
		{1, 4, 8},
		{1, 1, 4},
		{1, 4, 5},
		{4, 6, 3},
		{1, 9, 5},
		{1, 6, 9},
		{2, 6, 5},
		{1, 9, 2},
		{1, 6, 7},
		{18, 5, 2},
		{22, 3, 7},
		{19, 7, 1},
		{3, 8, 5},
		{4, 9, 3},
		{2, 7, 2},
		{1, 8, 1},
		{19, 1, 3},
		{2, 7, 5},
		{13, 3, 9},
		{4, 1, 2},
		{3, 5, 1},
		{11, 9, 1},
		{11, 2, 8},
		{3, 9, 3},
		{3, 5, 2},
		{2, 1, 4},
		{5, 2, 7},
		{12, 1, 5},
		{2, 4, 5},
		{9, 5, 8},
		{1, 5, 3},
		{4, 2, 3},
		{2, 7, 5},
		{6, 2, 8},
		{17, 8, 9},
		{2, 9, 6},
		{2, 7, 1},
		{15, 9, 6},
		{2, 2, 4},
		{9, 8, 5},
		{2, 1, 3},
		{12, 6, 2},
		{2, 3, 9},
		{5, 6, 3},
		{4, 5, 3},
		{11, 3, 4},
		{2, 9, 4},
		{6, 5, 2},
		{13, 4, 3},
		{1, 4, 5},
		{1, 4, 8},
		{18, 2, 6},
		{2, 5, 3},
		{1, 8, 3},
		{1, 2, 5},
		{1, 7, 8},
		{28, 3, 6},
		{2, 3, 4},
		{3, 5, 9},
		{2, 5, 9},
		{3, 9, 3},
		{5, 3, 4},
		{1, 9, 3},
		{1, 9, 1},
		{1, 3, 4},
		{45, 6, 2},
		{1, 8, 3},
		{2, 4, 6},
		{5, 4, 2},
		{1, 3, 7},
		{3, 2, 9},
		{1, 4, 8},
		{3, 6, 1},
		{42, 2, 8},
		{2, 9, 2},
		{4, 2, 6},
		{2, 2, 7},
		{1, 9, 6},
		{2, 8, 9},
		{4, 1, 8},
		{1, 6, 4},
		{1, 4, 8},
		{1, 2, 5},
		{3, 7, 4},
		{39, 8, 3},
		{7, 8, 5},
		{8, 5, 7},
		{35, 3, 1},
		{4, 3, 7},
		{10, 7, 2},
		{2, 9, 6},
		{3, 4, 2},
		{1, 7, 5},
		{1, 7, 8},
		{1, 5, 4},
		{12, 1, 6},
		{1, 8, 1},
		{1, 4, 5},
		{14, 6, 8},
		{9, 8, 6},
		{5, 6, 1},
		{11, 2, 9},
		{1, 9, 8},
		{6, 8, 3},
		{6, 9, 2},
		{8, 1, 9},
		{3, 3, 6},
		{7, 1, 4},
		{1, 5, 9},
		{8, 9, 8},
		{7, 6, 8},
		{1, 9, 3},
		{3, 6, 4},
		{3, 9, 1},
		{4, 3, 2},
		{1, 6, 7},
		{1, 4, 2},
		{13, 1, 7},
		{6, 4, 8},
		{1, 7, 3},
		{1, 4, 6},
		{1, 9, 5},
		{1, 3, 5},
		{19, 8, 9},
		{1, 6, 5},
		{6, 9, 2},
		{2, 5, 8},
		{1, 5, 2},
		{4, 1, 4},
		{8, 9, 4},
		{3, 9, 8},
		{2, 9, 1},
		{6, 7, 5},
		{12, 4, 2},
		{6, 8, 3},
		{1, 4, 1},
		{1, 3, 1},
		{13, 2, 3},
		{4, 5, 3},
		{1, 4, 9},
		{1, 8, 9},
		{12, 3, 2},
		{1, 9, 1},
		{2, 5, 9},
		{3, 9, 5},
		{1, 7, 5},
		{3, 7, 3},
		{1, 5, 4},
		{1, 5, 8},
		{9, 2, 3},
		{2, 2, 3},
		{3, 1, 9},
		{1, 8, 9},
		{3, 9, 1},
		{9, 2, 6},
		{1, 9, 5},
		{6, 2, 3},
		{2, 6, 9},
		{3, 6, 3},
		{1, 4, 3},
		{2, 9, 6},
		{2, 7, 2},
		{2, 2, 8},
		{24, 3, 7},
		{2, 5, 6},
		{2, 8, 2},
		{7, 2, 8},
		{8, 3, 6},
		{2, 1, 3},
		{1, 1, 2},
		{1, 5, 2},
		{15, 7, 4},
		{9, 7, 9},
		{7, 9, 1},
		{5, 8, 1},
		{4, 1, 4},
		{19, 4, 3},
		{22, 3, 5},
		{1, 7, 5},
		{9, 5, 4},
		{6, 1, 3},
		{6, 3, 1},
		{4, 5, 4},
		{1, 2, 1},
		{1, 2, 6},
		{4, 6, 1},
		{1, 3, 6},
		{3, 6, 3},
		{2, 9, 8},
		{2, 5, 3},
		{2, 5, 1},
		{10, 6, 4},
		{4, 4, 9},
		{7, 4, 3},
		{2, 8, 7},
		{4, 9, 3},
		{5, 5, 7},
		{1, 5, 1},
		{1, 6, 3},
		{1, 8, 4},
		{1, 8, 3},
		{13, 4, 5},
		{1, 1, 8},
		{6, 5, 3},
		{1, 7, 6},
		{5, 7, 6},
		{9, 1, 8},
		{1, 8, 4},
		{1, 7, 1},
		{1, 4, 1},
		{5, 3, 7},
		{3, 7, 9},
		{1, 5, 4},
		{6, 8, 6},
		{1, 9, 3},
		{2, 9, 5},
		{7, 5, 9},
		{1, 7, 5},
		{2, 5, 3},
		{10, 6, 8},
		{2, 6, 1},
		{1, 4, 9},
		{1, 7, 5},
		{8, 8, 2},
		{1, 1, 7},
		{1, 9, 7},
		{1, 5, 1},
		{3, 9, 8},
		{7, 8, 7},
		{6, 7, 1},
		{1, 8, 7},
		{4, 7, 1},
		{16, 3, 7},
		{4, 3, 1},
		{5, 7, 8},
		{16, 1, 4},
		{9, 1, 7},
		{1, 3, 4},
		{15, 4, 8},
		{1, 3, 1},
		{2, 1, 6},
		{2, 4, 9},
		{17, 8, 2},
		{6, 9, 5},
		{8, 7, 8},
		{2, 6, 9},
		{4, 5, 7},
		{2, 8, 5},
		{1, 5, 9},
		{11, 2, 6},
		{4, 6, 1},
		{5, 2, 8},
		{2, 9, 2},
		{1, 9, 3},
		{3, 1, 8},
		{1, 3, 6},
		{7, 6, 9},
		{2, 5, 4},
		{6, 7, 4},
		{4, 8, 1},
		{1, 5, 2},
		{1, 6, 1},
		{7, 9, 8},
		{2, 7, 9},
		{9, 2, 9},
		{5, 9, 3},
		{3, 2, 8},
		{4, 8, 7},
		{9, 7, 2},
		{3, 1, 3},
		{14, 8, 1},
		{2, 8, 3},
		{1, 9, 4},
		{3, 7, 9},
		{8, 3, 9},
		{2, 2, 7},
		{12, 1, 8},
		{4, 1, 6},
		{2, 6, 7},
		{1, 6, 7},
		{9, 4, 7},
		{9, 7, 4},
		{1, 1, 6},
		{2, 3, 6},
		{2, 6, 8},
		{12, 9, 8},
		{2, 6, 9},
		{2, 9, 7},
		{1, 8, 5},
		{5, 7, 5},
		{1, 9, 1},
		{3, 4, 1},
		{5, 4, 8},
		{4, 1, 7},
		{1, 4, 2},
		{19, 8, 4},
		{2, 7, 5},
		{14, 8, 5},
		{2, 7, 8},
		{3, 9, 8},
		{19, 4, 2},
		{9, 2, 4},
		{2, 7, 8},
		{15, 5, 9},
		{15, 9, 8},
		{1, 5, 9},
		{11, 8, 7},
		{4, 5, 8},
		{1, 5, 9},
		{2, 9, 5},
		{2, 2, 6},
		{14, 2, 9},
		{12, 8, 9},
		{3, 8, 4},
		{7, 9, 2},
		{4, 7, 9},
		{1, 6, 9},
		{1, 7, 5},
		{1, 6, 2},
		{3, 5, 4},
		{19, 9, 4},
		{1, 5, 1},
		{1, 9, 8},
		{1, 1, 7},
		{1, 8, 9},
		{4, 7, 2},
		{3, 7, 6},
		{18, 4, 2},
		{17, 2, 3},
		{2, 6, 8},
		{17, 3, 6},
		{13, 2, 1},
		{2, 8, 3},
		{2, 2, 9},
		{6, 1, 9},
		{1, 3, 4},
		{1, 3, 9},
		{8, 6, 4},
		{20, 4, 8},
		{3, 4, 8},
		{15, 8, 2},
		{11, 2, 6},
		{2, 1, 7},
		{7, 9, 8},
		{6, 9, 3},
		{1, 6, 5}
	};

	std::vector<std::vector<char>> stacks {
	 {'P','F','M','Q','W','G','R','T'}, // 1
	 {'H','F','R'},                     // 2
	 {'P','Z','R','V','G','H','S','D'}, // 3
	 {'Q','H','P','B','F','W','G'},
	 {'P','S','M','J','H'},
	 {'M','Z','T','H','S','R','P','L'},
	 {'P','T','H','N','M','L'},
	 {'F','D','Q','R'},
	 {'D','S','C','N','L','P','H'}
	};
	
	//solve_9000(instructions, stacks);
	//solve_9000(instructions_example, stacks_example);
	//print_stacks(stacks);
	//print_stacks(stacks_example);
	
	/**/
	const auto instr_from_file = get_instructions_from_file("/home/bernhard/Projects/adventofcode-2022/src_day5/instructions_full.txt");
	auto start1 = steady_clock::now();
	//for (const auto& elem : instr_from_file) {
	//	cout << "Move " << get<0>(elem) << " from " << get<1>(elem) << " to " << get<2>(elem) << "\n";
	//}
	solve_9001(instr_from_file, stacks);
	//solve_9001(instructions, stacks);
	auto end1 = steady_clock::now();
	std::cout << "TIME: " << duration_cast<nanoseconds>(end1 - start1).count() << " ns\n";
	std::cout << "TIME: " << duration_cast<microseconds>(end1 - start1).count() << " mus\n";
	print_stacks(stacks);
	print_solution(stacks);
	cout << "DMRDFRHHH (Target)\n";
	/**/
	
	//solve_9001(instructions_example, stacks_example);
	//print_stacks(stacks_example);
	
	//DMRDFRHHH
	
	return 0;
}