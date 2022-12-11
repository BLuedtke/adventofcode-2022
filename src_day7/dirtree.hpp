#pragma once

#include <vector>
#include <string>
#include <memory>

class dirtree_entry {
public:
	dirtree_entry() = default;
	dirtree_entry(dirtree_entry&& dir_mv) = default;
	dirtree_entry(const dirtree_entry& dir_mv) = default;
	dirtree_entry(const std::string _id, size_t _data_size, bool _is_leaf,
	              const std::vector<std::shared_ptr<dirtree_entry>>& _children, std::shared_ptr<dirtree_entry> _parent)
	         : id(_id), data_size(_data_size), is_leaf(_is_leaf),
	           children(_children), parent(_parent){};
	~dirtree_entry() = default;
	
	std::string id {};
	bool is_leaf {false};
	size_t data_size = 0;
	std::vector<std::shared_ptr<dirtree_entry>> children {};
	std::shared_ptr<dirtree_entry> parent {};
};