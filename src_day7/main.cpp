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
#include <chrono>
#include <vector>
#include <string>
#include <array>
#include <tuple>
#include <filesystem>
#include <memory>

#include "dirtree.hpp"

using std::cout; //NOLINT
using std::get; //NOLINT
using namespace std::chrono;
using namespace std::chrono_literals;
typedef unsigned long long ull_;
ull_ total_size = 0;

enum class parse_mode {
	parsing_command,
	parsing_dir
};

inline void print_dir(const std::shared_ptr<dirtree_entry>& dir, ull_ indent_level) {
	const auto indentor {"   "};
	for (ull_ i = 0; i < indent_level; ++i) {
		cout << indentor;
	}
	cout << "- " << dir->id;
	if (dir->is_leaf) {
		cout << " (file, size: " << dir->data_size << " )\n";
	} else {
		cout << " (dir, size: " << dir->data_size << " )\n";
		for (const auto& child : dir->children) {
			print_dir(child, indent_level + 1);
		}
	}
}

inline ull_ get_and_set_dir_size(std::shared_ptr<dirtree_entry>& dir) {
	if (dir->is_leaf) {
		return 0;
	}
	ull_ sum = 0;
	for (auto& child : dir->children) {
		if (child->is_leaf) {
			sum += child->data_size;
		} else {
			sum += get_and_set_dir_size(child);
		}
	}
	dir->data_size = sum;
	return sum;
}

inline std::shared_ptr<dirtree_entry>* parse_command_line_v2(const std::string& line, std::shared_ptr<dirtree_entry>* _active, 
                         std::shared_ptr<dirtree_entry>& root, parse_mode& p) 
{
	if (p == parse_mode::parsing_command) {
		if (line == "$ cd /") {
			_active = &root;
		} else if (line.starts_with("$ ls")) {
			p = parse_mode::parsing_dir;
		} else if (line.starts_with("$ cd ..")) {
			if (_active->get()->id != "/") {
				cout << "Went up from " << _active->get()->id;
				_active = &(_active->get()->parent);
				cout << " to " << _active->get()->id << "\n";
			} else {
				cout << "TODO_FIXME tried to go up but already at root\n";
			}
		} else if (line.starts_with("$ cd ")) {
			auto before_name_pos = line.find_last_of(" ");
			auto end_str = line.substr(before_name_pos + 1);
			for (size_t i = 0; i < _active->get()->children.size(); ++i){
				if (_active->get()->children.at(i)->id == end_str) {
					cout << "Switched from " << _active->get()->id;
					_active = &(_active->get()->children.at(i));
					cout << " to " << _active->get()->id << "\n";
					break;
				}
			}
		}
	} else {
		if (line.starts_with("$")) {
			p = parse_mode::parsing_command;
			_active = parse_command_line_v2(line, _active, root, p);
		} else if (line.starts_with("dir")) {
			auto before_name_pos = line.find_last_of(" ");
			auto end_str = line.substr(before_name_pos + 1);
			std::shared_ptr<dirtree_entry> ds (new dirtree_entry(end_str, 0, false, {}, *_active));
			_active->get()->children.push_back(std::move(ds));
			cout << "Created new dir entry named " << end_str << "\n";
		} else {
			// Line started with number -> file with size
			auto before_name_pos = line.find_last_of(" ");
			auto start_str = line.substr(0, before_name_pos);
			auto end_str = line.substr(before_name_pos + 1);
			// Add as child leaf with size
			std::shared_ptr<dirtree_entry> ds (new dirtree_entry(end_str, std::stol(start_str), true, {}, *_active));
			_active->get()->children.push_back(std::move(ds));
			cout << "Created new file entry named " << end_str << "\n";
		}
	}
	return _active;
}

inline void parse_command_line(const std::string& line, std::shared_ptr<dirtree_entry>** _active, 
                         std::shared_ptr<dirtree_entry>& root, parse_mode& p) 
{
	std::shared_ptr<dirtree_entry>* active = *_active;
	if (p == parse_mode::parsing_command) {
		if (line == "$ cd /") {
			active = &root;
		} else if (line.starts_with("$ ls")) {
			p = parse_mode::parsing_dir;
			return;
		} else if (line.starts_with("$ cd ..")) {
			if (active->get()->id != "/") {
				cout << "Went up from " << (*active)->id;
				active = &(active->get()->parent);
				cout << " to " << (*active)->id << "\n";
			} else {
				cout << "TODO_FIXME tried to go up but already at root\n";
			}
		} else if (line.starts_with("$ cd ")) {
			auto before_name_pos = line.find_last_of(" ");
			auto end_str = line.substr(before_name_pos + 1);
			for (size_t i = 0; i < active->get()->children.size(); ++i){
				if (active->get()->children.at(i)->id == end_str) {
					cout << "Switched from " << (*active)->id;
					active = &(active->get()->children.at(i));
					cout << " to " << (*active)->id << "\n";
					break;
				}
			}
		}
	} else {
		if (line.starts_with("$")) {
			p = parse_mode::parsing_command;
			parse_command_line(line, &active, root, p);
		} else if (line.starts_with("dir")) {
			auto before_name_pos = line.find_last_of(" ");
			auto end_str = line.substr(before_name_pos + 1);
			std::shared_ptr<dirtree_entry> ds (new dirtree_entry(end_str, 0, false, {}, (*active)));
			active->get()->children.push_back(std::move(ds));
			cout << "Created new dir entry named " << end_str << "\n";
		} else {
			// Line started with number -> file with size
			auto before_name_pos = line.find_last_of(" ");
			auto start_str = line.substr(0, before_name_pos);
			auto end_str = line.substr(before_name_pos + 1);
			// Add as child leaf with size
			std::shared_ptr<dirtree_entry> ds (new dirtree_entry(end_str, std::stol(start_str), true, {}, (*active)));
			active->get()->children.push_back(std::move(ds));
			cout << "Created new file entry named " << end_str << "\n";
		}
	}
}

inline std::shared_ptr<dirtree_entry> create_fs_tree(const std::filesystem::path& path) {
	std::ifstream instr_file{path, std::ios_base::in};
	std::shared_ptr<dirtree_entry> root (new dirtree_entry({"/", 0, false, {}, NULL}));
	std::shared_ptr<dirtree_entry>* active = &root;
	parse_mode p = parse_mode::parsing_command;
	
	if (instr_file.is_open()) {
		std::string line {};
		
		while (std::getline(instr_file, line)) {
			if (line.empty()) {
				continue;
			}
			//parse_command_line_v2(line, active, root, p);
			active = parse_command_line_v2(line, active, root, p);
		}
	}
	
	return root;
}

inline void insert_dirs_above_threshold_size(const std::shared_ptr<dirtree_entry>& dir, std::vector<ull_>& sizes, ull_ size) {
	if (dir->is_leaf) {
		return;
	}
	if (dir->data_size > size) {
		sizes.push_back(dir->data_size);
		for (const auto& child : dir->children) {
			if (!child->is_leaf) {
				insert_dirs_above_threshold_size(child, sizes, size);
			}
		}
	}
}

inline void print_dirs_below_threshold_size(const std::shared_ptr<dirtree_entry>& dir, ull_ size) {
	if (dir->is_leaf) {
		return;
	}
	if (dir->data_size < size) {
		cout << "dir " << dir->id << " below " << size << " in size (" << dir->data_size << ").\n";
		total_size += dir->data_size;
	}
	for (const auto& child : dir->children) {
		if (!child->is_leaf) {
			print_dirs_below_threshold_size(child, size);
		}
	}
}

inline void print_dir_closest_to_target_size(std::shared_ptr<dirtree_entry>& dir, ull_ size) {
	cout << "Size to search above to: " << size << "\n";
	std::vector<ull_> sizes {};
	insert_dirs_above_threshold_size(dir, sizes, size);
	std::sort(sizes.begin(), sizes.end());
	cout << "Sizes above the threshold: \n";
	for (auto elem : sizes) {
		cout << "- " << elem << "\n";
	}
	
}

int main(int argc, char **argv){
	//auto ret = create_fs_tree("/home/bernhard/Projects/adventofcode-2022/src_day7/input_example.txt");
	auto ret = create_fs_tree("/home/bernhard/Projects/adventofcode-2022/src_day7/input.txt");
	cout << "-------------\n";
	get_and_set_dir_size(ret);
	print_dir(ret, 0);
	cout << "-------------\n";
	//print_dirs_below_threshold_size(ret, 100000);
	//cout << "TOTAL SIZE: " << total_size << "\n";
	cout << "-------------\n";
	print_dir_closest_to_target_size(ret, 30000000 - (70000000 - ret->data_size));
	return 0;
}
