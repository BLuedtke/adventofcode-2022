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

#include "monkey_data.hpp"

using std::cout; //NOLINT
using std::get; //NOLINT
using namespace std::chrono;
using namespace std::chrono_literals;

using monkey_type = unsigned long long;

enum class parse_monkey {
	base,
	starting_items,
	operation,
	test_base,
	test_true,
	test_false
};

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

inline void print_monkey(const monkey_data& monkey, uint16_t indent_lvl) {
	cout << get_indent(indent_lvl) << "Monkey " << monkey.id << "\n";
	cout << get_indent(indent_lvl + 1) << "Starting items: ";
	for (const auto elem : monkey.held_items) {
		cout << elem;
		if (elem != monkey.held_items.back()) {
			cout << ", ";
		}
	}
	cout << "\n";
	cout << get_indent(indent_lvl + 1) << "Operation: " << monkey.op_string << "\n";
	cout << get_indent(indent_lvl + 1) << "Test: divisible by " << monkey.divisible_by << "\n";
	cout << get_indent(indent_lvl + 2) << "If true: throw to monkey " << monkey.tt_pass_monkey_id << "\n";
	cout << get_indent(indent_lvl + 2) << "If false: throw to monkey " << monkey.ff_pass_monkey_id << "\n\n";
}

void parse_monkey_base(monkey_data& curr_monkey, std::string& line) {
	auto space_pos = line.find(" ");
	if (space_pos == std::string::npos) {
		std::cerr << "   Unable to locate expected space in monkey string: " << line << "\n";
		std::terminate();
	}
	curr_monkey.id = std::stoul(line.substr(space_pos+1, 1));
	cout << "   Under-Construction ID: " << curr_monkey.id << "\n";
}

void parse_monkey_starting_items(monkey_data& curr_monkey, std::string& line) {
	auto colon_pos = line.find(":");
	if (colon_pos == std::string::npos) {
		std::cerr << "   Unable to locate expected colon in monkey string: " << line << "\n";
		std::terminate();
	}
	if (colon_pos + 2 >= line.size()) {
		cout << "   Monkey has no starting items.\n";
	} else {
		// Create substr, then tokenize
		auto start_items_str = line.substr(colon_pos + 2);
		// Remove commas from starting-items
		// from https://stackoverflow.com/a/83481
		auto end_pos = std::remove(start_items_str.begin(), start_items_str.end(), ',');
		start_items_str.erase(end_pos, start_items_str.end());
		
		// Split on whitespace using stringstream (https://stackoverflow.com/a/83481)
		auto iss_start_items = std::istringstream {start_items_str};
		monkey_type new_start_item {0};
		while (iss_start_items >> new_start_item) {
			curr_monkey.held_items.push_back(new_start_item);
			cout << "   Added starting item " << new_start_item << " for monkey " << curr_monkey.id << "\n";
		}
	}
}

void parse_monkey_operation(monkey_data& curr_monkey, std::string& line) {
	// find equal sign in equation
	// then parse based on whitespace sep (https://stackoverflow.com/a/83481)
	auto equal_pos = line.find("=");
	if (equal_pos == std::string::npos) {
		std::cerr << "   Unable to locate expected equal sign in monkey string: " << line << "\n";
		std::terminate();
	}
	auto eq_rhs = line.substr(equal_pos + 1);
	auto iss_eq = std::istringstream {eq_rhs};
	monkey_type new_start_item {0};
	std::string rhs_1 {};
	std::string rhs_op {};
	std::string rhs_2 {};
	// Technically don't need while here, as it should only work once in any case.
	while (iss_eq >> rhs_1 >> rhs_op >> rhs_2) {
		//cout << "   Operation Equation was parsed to: " << rhs_1 << " " << rhs_op << " " << rhs_2 << ";\n";
		std::string log_fct {};
		if (rhs_1 == "old" && rhs_2 == "old") {
			if (rhs_op == "+") {
				curr_monkey.monkey_operation = [] (monkey_type input) {return input + input;};
				log_fct = "old + old";
			} else if (rhs_op == "*") {
				curr_monkey.monkey_operation = [] (monkey_type input) {return input * input;};
				log_fct = "old * old";
			} else if (rhs_op == "-") {
				curr_monkey.monkey_operation = [] (monkey_type input) {return input - input;};
				log_fct = "old - old";
			}
		} else if (rhs_1 != "old" || rhs_2 != "old") {
			monkey_type rhs_constant_elem {0};
			if (rhs_1 != "old") {
				rhs_constant_elem = std::stoul(rhs_1);
				std::cerr << "   Equation parse WARNING: left-hand of operator is a constant\n";
			} else if (rhs_2 != "old") {
				rhs_constant_elem = std::stoul(rhs_2);
			}
			// Assume that last elem of eq is the constant (i.e. rhs2)
			if (rhs_op == "+") {
				curr_monkey.monkey_operation = [rhs_constant_elem] (monkey_type input) {return input + rhs_constant_elem;};
				log_fct = "old + " + std::to_string(rhs_constant_elem);
			} else if (rhs_op == "*") {
				curr_monkey.monkey_operation = [rhs_constant_elem] (monkey_type input) {return input * rhs_constant_elem;};
				log_fct = "old * " + std::to_string(rhs_constant_elem);
			} else if (rhs_op == "-") {
				curr_monkey.monkey_operation = [rhs_constant_elem] (monkey_type input) {return input - rhs_constant_elem;};
				log_fct = "old - " + std::to_string(rhs_constant_elem);
			}
		} else if (rhs_1 != "old" && rhs_2 != "old") {
			monkey_type rhs_const_1 = std::stoul(rhs_1);
			monkey_type rhs_const_2 = std::stoul(rhs_2);
			if (rhs_op == "+") {
				curr_monkey.monkey_operation = [rhs_const_1,rhs_const_2] (monkey_type input) {return rhs_const_1 + rhs_const_2;};
				log_fct = std::to_string(rhs_const_1) + " + " + std::to_string(rhs_const_2);
			} else if (rhs_op == "*") {
				curr_monkey.monkey_operation = [rhs_const_1,rhs_const_2] (monkey_type input) {return rhs_const_1 * rhs_const_2;};
				log_fct = std::to_string(rhs_const_1) + " * " + std::to_string(rhs_const_2);
			} else if (rhs_op == "-") {
				curr_monkey.monkey_operation = [rhs_const_1,rhs_const_2] (monkey_type input) {return rhs_const_1 - rhs_const_2;};
				log_fct = std::to_string(rhs_const_1) + " - " + std::to_string(rhs_const_2);
			}
		}
		curr_monkey.op_string = "new = " + log_fct;
		cout << "   Set operation to " << log_fct << "; for monkey " << curr_monkey.id << "\n";
	}
}

void parse_monkey_test_base(monkey_data& curr_monkey, std::string& line) {
	// This may have to be rewritten if tests other than "divisible by"
	// shall be possible in the future.
	auto last_space_pos = line.find_last_of(" ");
	if (last_space_pos == std::string::npos) {
		std::cerr << "   Unable to locate last expected space in monkey string: " << line << "\n";
		std::terminate();
	}
	auto line_end = line.substr(last_space_pos + 1);
	curr_monkey.divisible_by = std::stoul(line_end);
	cout << "   Set divisible_by to " << std::stoul(line_end) << " for monkey " << curr_monkey.id << "\n";
}

void parse_monkey_test_true_or_false(monkey_data& curr_monkey, std::string& line) {
	line = ltrim(line);
	bool test_outcome = false;
	if (line.starts_with("If true:")) {
		test_outcome = true;
	} //otherwise false by default
	auto last_space_pos = line.find_last_of(" ");
	if (last_space_pos == std::string::npos) {
		std::cerr << "   Unable to locate last expected space in monkey string: " << line << "\n";
		std::terminate();
	}
	auto line_end = line.substr(last_space_pos + 1);
	if (test_outcome) {
		curr_monkey.tt_pass_monkey_id = std::stoul(line_end);
		cout << "   Set tt_pass_monkey_id to " << std::stoul(line_end) << " for monkey " << curr_monkey.id << "\n";
	} else {
		curr_monkey.ff_pass_monkey_id = std::stoul(line_end);
		cout << "   Set ff_pass_monkey_id to " << std::stoul(line_end) << " for monkey " << curr_monkey.id << "\n";
	}
}

inline std::vector<monkey_data> read_monkey_file_input(const std::filesystem::path& path) {
	std::ifstream instr_file{path, std::ios_base::in};
	
	if (!instr_file.is_open()) {
		return {};
	}
	parse_monkey parse_mode = parse_monkey::base;
	monkey_data under_construction {};
	std::vector<monkey_data> monkeys {};
	std::string line {};
	while (std::getline(instr_file, line)) {
		if (line.empty()) {
			continue;
		}
		//cout << "parsed line: " << line << "\n";
		if (parse_mode == parse_monkey::base) {
			parse_monkey_base(under_construction, line);
			parse_mode = parse_monkey::starting_items;
		} else if (parse_mode == parse_monkey::starting_items) {
			parse_monkey_starting_items(under_construction, line);
			parse_mode = parse_monkey::operation;
		} else if (parse_mode == parse_monkey::operation) {
			parse_monkey_operation(under_construction, line);
			parse_mode = parse_monkey::test_base;
		} else if (parse_mode == parse_monkey::test_base) {
			parse_monkey_test_base(under_construction, line);
			parse_mode = parse_monkey::test_true;
		} else if (parse_mode == parse_monkey::test_true) {
			parse_monkey_test_true_or_false(under_construction, line);
			parse_mode = parse_monkey::test_false;
		} else if (parse_mode == parse_monkey::test_false) {
			parse_monkey_test_true_or_false(under_construction, line);
			cout << "================= ADDED MONKEY " << under_construction.id << " =================\n";
			monkeys.push_back(std::move(under_construction));
			under_construction = monkey_data {};
			parse_mode = parse_monkey::base;
		}
	}
	cout << "------------------------------------------------------\n";
	return monkeys;
}

// Verify that id's of monkeys match indexes in collection
inline bool verify_monkey_ids_match_indices(const std::vector<monkey_data>& monkeys) {
	for (size_t i = 0; i < monkeys.size(); ++i) {
		if (static_cast<monkey_type>(i) != monkeys.at(i).id) {
			return false;
		}
	}
	return true;
}

inline void reset_number_of_inspections(std::vector<monkey_data>& monkeys) {
	for (auto& monkey : monkeys) {
		monkey.number_of_inspections = 0;
	}
}

inline monkey_type get_monkey_lcm(const std::vector<monkey_data>& monkeys) {
	std::vector<monkey_type> divs {};
	for (const auto& m : monkeys) {
		divs.push_back(m.divisible_by);
	}
	while (divs.size() > 1) {
		auto lcm_ = std::lcm(divs.at(divs.size()-1), divs.at(divs.size()-2));
		divs.pop_back();
		divs.back() = lcm_;
	}
	return divs.front();
}

void execute_monkey_round_v3(std::vector<monkey_data>& monkeys, monkey_type lcm_) {
	for (size_t i = 0; i < monkeys.size(); ++i) {
		// Monkey at position i is 'playing'
		auto& monkey = monkeys.at(i);
		//cout << "Monkey " << i << ":\n";
		// Inspection loop
		for (size_t item_index = 0; item_index < monkey.held_items.size(); ++item_index) {
			auto& item = monkey.held_items.at(item_index);
			monkey.number_of_inspections++;
			//cout << "  Monkey inspects an item with a worry level of " << item << ".\n";
			if (item >= lcm_) {
				item = item % lcm_;
			}
			item = monkey.monkey_operation(item);
			//cout << "    Monkey operation applied, worry level is now " << item << ".\n";
			size_t thrown_to {};
			if ((item % monkey.divisible_by) == 0) {
				//cout << "    Current worry level is divisible by " << monkey.divisible_by << ".\n";
				thrown_to = monkey.tt_pass_monkey_id;
			} else {
				//cout << "    Current worry level is not divisible by " << monkey.divisible_by << ".\n";
				thrown_to = monkey.ff_pass_monkey_id;
			}
			//cout << "    Item with worry level " << item << " is thrown to monkey " << thrown_to << ".\n";
			monkeys.at(thrown_to).held_items.push_back(item);
		}
		monkey.held_items.clear();
	}
}

void execute_monkey_round(std::vector<monkey_data>& monkeys) {
	for (size_t i = 0; i < monkeys.size(); ++i) {
		// Monkey at position i is 'playing'
		auto& monkey = monkeys.at(i);
		cout << "Monkey " << i << ":\n";
		// Inspection loop
		for (size_t item_index = 0; item_index < monkey.held_items.size(); ++item_index) {
			auto& item = monkey.held_items.at(item_index);
			monkey.number_of_inspections++;
			cout << "  Monkey inspects an item with a worry level of " << item << ".\n";
			item = monkey.monkey_operation(item);
			cout << "    Monkey operation applied, worry level is now " << item << ".\n";
			item = item / 3;
			cout << "    Monkey gets bored with item. Worry level is divided by 3 to " << item << ".\n";
			size_t thrown_to {};
			if ((item % monkey.divisible_by) == 0) {
				cout << "    Current worry level is divisible by " << monkey.divisible_by << ".\n";
				thrown_to = monkey.tt_pass_monkey_id;
			} else {
				cout << "    Current worry level is not divisible by " << monkey.divisible_by << ".\n";
				thrown_to = monkey.ff_pass_monkey_id;
			}
			cout << "    Item with worry level " << item << " is thrown to monkey " << thrown_to << ".\n";
			monkeys.at(thrown_to).held_items.push_back(item);
		}
		monkey.held_items.clear();
	}
}

inline void print_post_round(const std::vector<monkey_data>& monkeys) {
	for (const auto& monkey : monkeys) {
		cout << "Monkey " << monkey.id << ": ";
		for (const auto item : monkey.held_items) {
			cout << item;
			if (item != monkey.held_items.back()) {
				cout << ", ";
			}
		}
		cout << "\n";
	}
}

inline void print_inspections_and_held_count(const std::vector<monkey_data>& monkeys) {
	for (const auto& monkey : monkeys) {
		cout << "Monkey " << monkey.id << " inspected items " << monkey.number_of_inspections << " times. Holds " << monkey.held_items.size();
		cout << "\n";
	}
}

inline void print_inspections_and_held(const std::vector<monkey_data>& monkeys) {
	for (const auto& monkey : monkeys) {
		cout << "Monkey " << monkey.id << " inspected items " << monkey.number_of_inspections << " times. Holds: ";
		for (const auto item : monkey.held_items) {
			cout << item;
			if (item != monkey.held_items.back()) {
				cout << ", ";
			}
		}
		cout << "\n";
	}
}

inline void print_inspections(const std::vector<monkey_data>& monkeys) {
	for (const auto& monkey : monkeys) {
		cout << "Monkey " << monkey.id << " inspected items " << monkey.number_of_inspections << " times.\n";
	}
}

inline size_t calculate_monkey_business(const std::vector<monkey_data>& monkeys) {
	if (monkeys.size() < 2) {
		std::cerr << "Not enough monkeys (< 2) to calculate monkey business!\n";
		return 0;
	}
	std::vector<size_t> inspections {};
	for (const auto& monkey : monkeys) {
		inspections.push_back(monkey.number_of_inspections);
	}
	std::sort(inspections.begin(), inspections.end(), std::greater<size_t>());
	return inspections.at(0) * inspections.at(1);
}


int main(int argc, char **argv){
	//auto monkeys = read_monkey_file_input("/home/bernhard/Projects/adventofcode-2022/src_day11/input_example.txt");
	auto monkeys = read_monkey_file_input("/home/bernhard/Projects/adventofcode-2022/src_day11/input_full.txt");
	for (const auto& monkey : monkeys) {
		print_monkey(monkey, 0);
	}
	cout << "------------------------------------------------------\n";
	cout << "Verifying ID-Indices matchup...";
	if(verify_monkey_ids_match_indices(monkeys)) {
		cout << " match confirmed.\n";
	} else {
		cout << " match NOT confirmed.\n";
		return 1;
	}
	cout << "------------------------------------------------------\n";
	auto lcm_ = get_monkey_lcm(monkeys);
	cout << "LCM: " << lcm_ << "\n";
	cout << "------------------------------------------------------\n";
	
	cout << "Starting to play.\n";
	print_inspections_and_held(monkeys);
	
	//const size_t rounds_to_play = 20;
	const size_t rounds_to_play = 10000;
	
	
	for (size_t i = 0; i < rounds_to_play; ++i) {
		if (((i+1) % 1000) == 0 || i == 19 || i == 0) {
			cout << "\nPlaying round " << i + 1 << "\n";
		}
		execute_monkey_round_v3(monkeys, lcm_);
		if (((i+1) % 1000) == 0 || i == 19 || i == 0) {
			print_inspections_and_held_count(monkeys);
		}
	}
	cout << "\n";
	cout << "Monkey Business: " << calculate_monkey_business(monkeys) << "\n";
	return 0;
}
