#pragma once

#include <cstdint>
#include <vector>
#include <functional>
#include <string>

typedef unsigned long long monkey_type;

class monkey_data {
public:
	
	monkey_type id {0};
	std::vector<monkey_type> held_items {};
	
	std::function<monkey_type(monkey_type)> monkey_operation {};
	// string description of operation for debug only
	std::string op_string {};
	monkey_type divisible_by {1};
	size_t tt_pass_monkey_id {0};
	size_t ff_pass_monkey_id {0};
	size_t number_of_inspections {0};
};