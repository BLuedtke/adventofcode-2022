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
