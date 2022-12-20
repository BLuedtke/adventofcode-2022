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
#include <map>
#include <sstream>
#include <omp.h>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <future>
#include <atomic>
#include <set>

using namespace std::literals;
using std::cout; //NOLINT
using std::get; //NOLINT
using namespace std::chrono;
using namespace std::chrono_literals;

std::mutex result_sync_mutex {};

std::map<std::string, std::map<std::string, size_t>> distance_table {};
ssize_t max_value {0};
std::atomic_uint64_t max_value_at {0};
std::string max_value_game_path {};
std::atomic_uint64_t game_step_calls_at {0};
ssize_t rounds_total{0};

typedef struct valve_t {
	std::string id {};
	size_t flow_rate {0};
	std::vector<std::string> next_valves {};
} valve_t;

typedef struct valve_ext {
	std::string id {};
	size_t flow_rate {0};
	bool opened {false};
	bool targeted {false};
	std::vector<std::string> next_valves {};
} valve_ext;

typedef struct info_valve {
	size_t valve_flow_rate {0};
	std::string valve_id;
	size_t distance {0};
	ssize_t value {0};
} info_valve;


enum class player_status {
	travelling,
	available,
	idle
};

using player_status::travelling;
using player_status::available;
using player_status::idle;


// Flow, Valve, distance, value
using valve_greatest_search_type = std::tuple<ssize_t,valve_t,ssize_t,ssize_t>;

// From https://stackoverflow.com/a/25829233
inline std::string& ltrim(std::string& s, const char* t = " \t\n\r\f\v") {
	s.erase(0, s.find_first_not_of(t));
	return s;
}

inline void print_valve(const valve_t& valve) {
	cout << "Valve " << valve.id << ", flow " << valve.flow_rate << "; next valves: ";
	for (const auto& v_id : valve.next_valves) {
		cout << v_id;
		if (v_id != valve.next_valves.back()) {
			cout << ", ";
		}
	}
	cout << "\n";
}

size_t distance_from_to_valve(const std::map<std::string,valve_t>& valve_map_p, const std::string& start_valve, const std::string& end_valve, size_t current_cost, const std::set<std::string>& known) {
	auto& nv = valve_map_p.at(start_valve).next_valves;
	auto pos = std::find(nv.cbegin(), nv.cend(), end_valve);
	if (pos != std::end(nv)) {
		return current_cost + 1;
	}
	auto known_local {known};
	known_local.insert(start_valve);
	// end_valve is not contained in list of next_valves of current valve. -> depth first call to next_valves
	size_t min_cost {9999};
	for (const auto& n_candidate : valve_map_p.at(start_valve).next_valves) {
		if (!known_local.contains(n_candidate)) {
			auto cost = distance_from_to_valve(valve_map_p, n_candidate, end_valve, current_cost+1, known_local);
			if (cost < min_cost) {
				min_cost = cost;
			}
		}
	}
	return min_cost;
}

size_t distance_from_to_valve_start(const std::map<std::string,valve_t>& valve_map_p, const std::string& start_valve, const std::string& end_valve, size_t current_cost) {
	auto& nv = valve_map_p.at(start_valve).next_valves;
	auto pos = std::find(nv.cbegin(), nv.cend(), end_valve);
	if (pos != std::end(nv)) {
		return current_cost + 1;
	}
	// end_valve is not contained in list of next_valves of current valve. -> depth first call to next_valves
	std::set<std::string> known_nodes {};
	known_nodes.insert(start_valve);
	size_t min_cost {9999};
	for (const auto& n_candidate : valve_map_p.at(start_valve).next_valves) {
		auto cost = distance_from_to_valve(valve_map_p, n_candidate, end_valve, current_cost+1, known_nodes);
		if (cost < min_cost) {
			min_cost = cost;
		}
	}
	return min_cost;
}

std::map<std::string, std::map<std::string, size_t>> build_distance_table(const std::map<std::string,valve_t>& valve_map_p) {
	std::map<std::string, std::map<std::string, size_t>> ret {};
	// Optimization potential: since graph here is bidirectional,
	// can re-use e.g. distance from AA->BB for entry BB->AA
	for (const auto& kv : valve_map_p) {
		for (const auto& kv_inner : valve_map_p) {
			if (kv.first != kv_inner.first) {
				if (ret.contains(kv_inner.first) && ret.at(kv_inner.first).contains(kv.first)) {
					ret[kv.first][kv_inner.first] = ret[kv_inner.first][kv.first];
				} else {
					auto d = distance_from_to_valve_start(valve_map_p, kv.first, kv_inner.first, 0);
					ret[kv.first].insert({kv_inner.first, d});
				}
			}
			
		}
	}
	return ret;
}


inline std::vector<info_valve> build_search_info_safe(const std::map<std::string,valve_ext>& valve_map, const std::string& current_valve, ssize_t rounds_left) {
	std::vector<info_valve> search_vec {};
	// For every valve in valve_map_p that is not targeted or already open, add info_valve entry
	for (const auto& kv : valve_map) {
		if (!kv.second.targeted && !kv.second.opened && kv.first != current_valve && kv.second.flow_rate > 0) {
			info_valve i_valve {kv.second.flow_rate, kv.first, distance_table.at(current_valve).at(kv.first), 0};
			if (i_valve.distance + 1 <= rounds_left) {
				i_valve.value = (rounds_left - i_valve.distance - 1) * kv.second.flow_rate;
			}
			search_vec.push_back(std::move(i_valve));
		}
	}
	return search_vec;
}

inline std::vector<info_valve> build_search_info_aggr_prune(const std::map<std::string,valve_ext>& valve_map, const std::string& current_valve, ssize_t rounds_left) {
	std::vector<info_valve> search_vec {};
	// For every valve in valve_map_p that is not targeted or already open, add info_valve entry
	for (const auto& kv : valve_map) {
		if (!kv.second.targeted && !kv.second.opened && kv.first != current_valve && kv.second.flow_rate > 0) {
			info_valve i_valve {kv.second.flow_rate, kv.first, distance_table.at(current_valve).at(kv.first), 0};
			if (i_valve.distance + 1 <= rounds_left) {
				i_valve.value = (rounds_left - i_valve.distance - 1) * kv.second.flow_rate;
				search_vec.push_back(std::move(i_valve));
			}
		}
	}
	if (search_vec.size() > 6) {
		size_t size_save = search_vec.size();
		struct {
			bool operator()(const info_valve& a, const info_valve& b) const { 
				return a.value > b.value;
			}
		} customGreaterByValveValue;
		std::sort(search_vec.begin(), search_vec.end(), customGreaterByValveValue);
		search_vec.erase(search_vec.begin() + (search_vec.size()/4)*3, search_vec.end());
		//cout << "Size Pre: " << size_save << "; size now: " << search_vec.size() << "\n";
	}
	return search_vec;
}

inline std::vector<std::pair<info_valve, info_valve>> combine_player_search_info_v1(std::vector<info_valve>&& _info_p1, std::vector<info_valve>&& _info_p2) {
	std::vector<std::pair<info_valve, info_valve>> ret {};
	const auto info_p1 {std::move(_info_p1)};
	const auto info_p2 {std::move(_info_p2)};
	if (info_p1.size() == 1 && info_p2.size() == 1) {
		if (info_p1.front().value > info_p2.front().value) {
			ret.push_back({info_p1.front(), {}});
		} else {
			ret.push_back({{}, info_p2.front()});
		}
	} else {
		for (const auto& elem_info_p1 : info_p1) {
			for (const auto& elem_info_p2 : info_p2) {
				if (elem_info_p1.valve_id != elem_info_p2.valve_id && (elem_info_p1.value > 0 || elem_info_p2.value > 0)) {
					ret.push_back({elem_info_p1,elem_info_p2});
				}
			}
		}
	}
	return ret;
}

inline std::vector<std::pair<info_valve, info_valve>> combine_player_search_info_v2(std::vector<info_valve>&& _info_p1, std::vector<info_valve>&& _info_p2) {
	//cout << "2 Players at same time\n";
	std::vector<std::pair<info_valve, info_valve>> pre_ret {};
	const auto info_p1 {std::move(_info_p1)};
	const auto info_p2 {std::move(_info_p2)};
	if (info_p1.size() == 1 && info_p2.size() == 1) {
		if (info_p1.front().value > info_p2.front().value) {
			pre_ret.push_back({info_p1.front(), {}});
		} else {
			pre_ret.push_back({{}, info_p2.front()});
		}
		return pre_ret;
	}
	for (const auto& elem_info_p1 : info_p1) {
		for (const auto& elem_info_p2 : info_p2) {
			if (elem_info_p1.valve_id != elem_info_p2.valve_id && (elem_info_p1.value > 0 || elem_info_p2.value > 0)) {
				pre_ret.push_back({elem_info_p1,elem_info_p2});
			}
		}
	}
	if (pre_ret.size() <= 2) {
		return pre_ret;
	}
	//return pre_ret;
	std::vector<std::pair<info_valve, info_valve>> ret {};
	std::set<std::pair<std::string,std::string>> already_added {};
	for (const auto& pair_opt : pre_ret) {
		if (!already_added.contains({pair_opt.first.valve_id,pair_opt.second.valve_id}) && !already_added.contains({pair_opt.second.valve_id,pair_opt.first.valve_id})) {
			bool pushed = false;
			for (const auto& in_pair_opt : pre_ret) {
				if (pair_opt.first.valve_id == in_pair_opt.second.valve_id && pair_opt.second.valve_id == in_pair_opt.first.valve_id) {
					pushed = true;
					ssize_t sum_value_1 = pair_opt.first.value + pair_opt.second.value;
					ssize_t sum_value_2 = in_pair_opt.first.value + in_pair_opt.second.value;
					if (sum_value_1 >= sum_value_2) {
						ret.push_back(pair_opt);
						already_added.insert({pair_opt.first.valve_id,pair_opt.second.valve_id});
					} else {
						ret.push_back(in_pair_opt);
						already_added.insert({in_pair_opt.first.valve_id,in_pair_opt.second.valve_id});
						//cout << "Second option better, values " << sum_value_1 << " vs " << sum_value_2 << "\n";
					}
					break;
				}
			}
			if (!pushed) {
				ret.push_back(pair_opt);
				already_added.insert({pair_opt.first.valve_id,pair_opt.second.valve_id});
			}
		}
	}
	//cout << "Returned size " << ret.size() << "; pre_ret size " << pre_ret.size() << "\n";
	return ret;
}

inline player_status calc_player_status_and_updates (std::map<std::string,valve_ext>& valve_map, ssize_t& sleep, std::string& exec_string, const std::string& valve_id, const std::string& player_name) {
	player_status player_status {idle};
	if (sleep > 1) {
		// player is travelling to valve player1_valve
		player_status = travelling;
		valve_map.at(valve_id).targeted = true;
		sleep--;
		exec_string += player_name + " on way to " + valve_id + "\n";
	} else if (sleep == 1) {
		// player is opening valve player1_valve
		player_status = available;
		sleep--;
		valve_map.at(valve_id).opened = true;
		exec_string += player_name + " opens " + valve_id + "\n";
	} else if (sleep == -1) {
		// player is idling
		player_status = idle;
		exec_string += player_name + " idle\n";
	} else if (sleep == -2) {
		// initialization round
		player_status = available;
	}
	return player_status;
}

inline std::string update_flow(const std::map<std::string,valve_ext>& valve_map, ssize_t& io_total_flow) {
	std::string local_exec_str {"Valves "};
	ssize_t local_flow {0};
	for (const auto& kv : valve_map) {
		if (kv.second.opened) {
			local_flow += kv.second.flow_rate;
			local_exec_str += kv.second.id;
			local_exec_str += ", ";
		}
	}
	io_total_flow += local_flow;
	local_exec_str += " open: " + std::to_string(local_flow) + " pressure. Total: " + std::to_string(io_total_flow) + "\n";
	return local_exec_str;
}


void game_step_v1(std::map<std::string,valve_ext>&& _valve_map_p, ssize_t rounds_left, ssize_t sleep_1, ssize_t sleep_2, ssize_t _value_sum_p, const std::string& player1_valve, const std::string& player2_valve, std::string&& exec_str);
void game_step_v1(const std::map<std::string,valve_ext>& _valve_map_p, ssize_t rounds_left, ssize_t sleep_1, ssize_t sleep_2, ssize_t _value_sum_p, const std::string& player1_valve, const std::string& player2_valve, const std::string& exec_str);

inline void perform_single_player_next_step(std::map<std::string,valve_ext>&& _valve_map_p, const std::string& player_active_valve, const std::string& player_inactive_valve, ssize_t player_inactive_sleep, ssize_t rounds_left, ssize_t value_sum, std::string&& _exec_str_p, bool is_player_1) {
	auto valve_map {std::move(_valve_map_p)};
	const auto options = build_search_info_aggr_prune(valve_map, player_active_valve, rounds_left);
	//const auto options = build_search_info_v1(valve_map, player_active_valve, rounds_left);
	std::atomic_bool attempted {false};
	auto exec_str {_exec_str_p};
	#pragma omp parallel for schedule(dynamic) proc_bind(close)
	for (const auto& option : options) {
		if (option.value > 0) {
			attempted = true;
			const ssize_t sleep_n = option.distance + 1;
			const std::string player_valve_n = option.valve_id;
			if (is_player_1) {
				game_step_v1(valve_map, rounds_left-1, sleep_n, player_inactive_sleep, value_sum, player_valve_n, player_inactive_valve, exec_str);
			} else {
				game_step_v1(valve_map, rounds_left-1, player_inactive_sleep, sleep_n, value_sum, player_inactive_valve, player_valve_n, exec_str);
			}
		}
	}
	if (!attempted) {
		// Player has no valves left to open, and other player is busy or already finished -> set player to idle for next round
		if (is_player_1) {
			game_step_v1(std::move(valve_map), rounds_left-1, -1, player_inactive_sleep, value_sum, "", player_inactive_valve, std::move(exec_str));
		} else {
			game_step_v1(std::move(valve_map), rounds_left-1, player_inactive_sleep, -1, value_sum, player_inactive_valve, "", std::move(exec_str));
		}
	}
}

inline void perform_double_player_next_step(std::map<std::string,valve_ext>&& _valve_map_p, const std::string& player1_valve, const std::string& player2_valve,
        ssize_t rounds_left, ssize_t value_sum, std::string&& _exec_str_p) {
	auto valve_map {std::move(_valve_map_p)};
	auto exec_str {_exec_str_p};
	const auto pair_options = combine_player_search_info_v2(build_search_info_safe(valve_map, player1_valve, rounds_left), build_search_info_safe(valve_map, player2_valve, rounds_left));
	//const auto pair_options = combine_player_search_info_v1(build_search_info_v1(valve_map, player1_valve, rounds_left), build_search_info_v1(valve_map, player2_valve, rounds_left));
	//#pragma omp parallel for schedule(static) proc_bind(spread)
	std::atomic_bool attempted {false};
	#pragma omp parallel for schedule(dynamic) proc_bind(close)
	for (const auto& option : pair_options) {
		if (option.first.value > 0 && option.second.value > 0) {
			attempted = true;
			const ssize_t sleep_1_n = option.first.distance + 1;
			const ssize_t sleep_2_n = option.second.distance + 1;
			const auto player1_valve_n = option.first.valve_id;
			const auto player2_valve_n = option.second.valve_id;
			game_step_v1(valve_map, rounds_left-1, sleep_1_n, sleep_2_n, value_sum, player1_valve_n, player2_valve_n, exec_str);
		}
	}
	if (!attempted) {
		for (const auto& option : pair_options) {
			if (option.first.value > 0 && option.second.value <= 0) {
				attempted = true;
				const auto sleep_1 = option.first.distance + 1;
				const auto player1_valve_n = option.first.valve_id;
				game_step_v1(valve_map, rounds_left-1, sleep_1, -1, value_sum, player1_valve_n, "", exec_str);
			} else if (option.first.value <= 0 && option.second.value > 0) {
				attempted = true;
				const auto sleep_2 = option.second.distance + 1;
				const auto player2_valve_n = option.second.valve_id;
				game_step_v1(valve_map, rounds_left-1, -1, sleep_2, value_sum, "", player2_valve_n, exec_str);
			}
		}
	}
	if (!attempted) {
		game_step_v1(std::move(valve_map), rounds_left-1, -1, -1, value_sum, "", "", std::move(exec_str));
	}
}

void game_step_v1(std::map<std::string,valve_ext>&& _valve_map_p, ssize_t rounds_left, ssize_t sleep_1, ssize_t sleep_2, ssize_t _value_sum_p, const std::string& player1_valve, const std::string& player2_valve, std::string&& exec_str) {
	++game_step_calls_at;
	
	if (rounds_left == 0)  {
		// Ending with Value value_sum
		std::unique_lock result_sync_lock (result_sync_mutex);
		if (_value_sum_p > max_value) {
			max_value = _value_sum_p;
			max_value_game_path = exec_str;
		}
		return;
	}
	auto local_exec_str {std::move(exec_str)};
	local_exec_str += "\n== Min " + std::to_string(rounds_total - rounds_left) + " ==\n";
	auto value_sum {_value_sum_p};
	auto valve_map {std::move(_valve_map_p)};
	local_exec_str += update_flow(valve_map, value_sum);
	
	const player_status p1_status = calc_player_status_and_updates(valve_map, sleep_1, local_exec_str, player1_valve, "P1");
	const player_status p2_status = calc_player_status_and_updates(valve_map, sleep_2, local_exec_str, player2_valve, "P2");
	
	if ((p1_status == idle) && (p2_status == idle)) {
		//game_step_v1(std::move(valve_map), rounds_left-1, -1, -1, value_sum, "", "", std::move(local_exec_str));
		local_exec_str += "\nSumming up rest of game.\n";
		auto diff = (value_sum - _value_sum_p);
		for (size_t i = 1; i < rounds_left; ++i) {
			value_sum += diff;
		}
		{
			std::unique_lock result_sync_lock (result_sync_mutex);
			if (value_sum > max_value) {
				max_value = value_sum;
				max_value_game_path = local_exec_str;
			}
		}
		return;
	} else if ((p1_status == travelling || p1_status == idle) && (p2_status == travelling || p2_status == idle)) {
		game_step_v1(std::move(valve_map), rounds_left-1, sleep_1, sleep_2, value_sum, player1_valve, player2_valve, std::move(local_exec_str));
	} else if (p1_status == available && (p2_status == travelling || p2_status == idle)) {
		perform_single_player_next_step(std::move(valve_map), player1_valve, player2_valve, sleep_2, rounds_left, value_sum, std::move(local_exec_str), true);
	} else if ((p1_status == travelling || p1_status == idle) && p2_status == available) {
		perform_single_player_next_step(std::move(valve_map), player2_valve, player1_valve, sleep_1, rounds_left, value_sum, std::move(local_exec_str), false);
	} else if (p1_status == available && p2_status == available) {
		perform_double_player_next_step(std::move(valve_map), player1_valve, player2_valve, rounds_left, value_sum, std::move(local_exec_str));
	}
}

// _valve_map_p, rounds_left, rounds_total, sleep_1, sleep_2, _value_sum_p, player1_valve, player2_valve, exec_str
void game_step_v1(const std::map<std::string,valve_ext>& _valve_map_p, ssize_t rounds_left, ssize_t sleep_1, ssize_t sleep_2, ssize_t _value_sum_p, const std::string& player1_valve, const std::string& player2_valve, const std::string& exec_str) {
	++game_step_calls_at;
	if (rounds_left == 0)  {
		// Ending with Value value_sum
		std::unique_lock result_sync_lock (result_sync_mutex);
		if (_value_sum_p > max_value) {
			max_value = _value_sum_p;
			max_value_game_path = exec_str;
		}
		return;
	}
	auto local_exec_str = exec_str + "\n== Min " + std::to_string(rounds_total - rounds_left) + " ==\n";
	auto value_sum {_value_sum_p};
	auto valve_map {_valve_map_p};
	local_exec_str += update_flow(valve_map, value_sum);
	
	const player_status p1_status = calc_player_status_and_updates(valve_map, sleep_1, local_exec_str, player1_valve, "P1");
	const player_status p2_status = calc_player_status_and_updates(valve_map, sleep_2, local_exec_str, player2_valve, "P2");
	
	if ((p1_status == travelling || p1_status == idle) && (p2_status == travelling || p2_status == idle)) {
		game_step_v1(std::move(valve_map), rounds_left-1, sleep_1, sleep_2, value_sum, player1_valve, player2_valve, std::move(local_exec_str));
	} else if (p1_status == available && (p2_status == travelling || p2_status == idle)) {
		perform_single_player_next_step(std::move(valve_map), player1_valve, player2_valve, sleep_2, rounds_left, value_sum, std::move(local_exec_str), true);
	} else if ((p1_status == travelling || p1_status == idle) && p2_status == available) {
		perform_single_player_next_step(std::move(valve_map), player2_valve, player1_valve, sleep_1, rounds_left, value_sum, std::move(local_exec_str), false);
	} else if (p1_status == available && p2_status == available) {
		perform_double_player_next_step(std::move(valve_map), player1_valve, player2_valve, rounds_left, value_sum, std::move(local_exec_str));
	}
}

inline void print_distance_map(const std::map<std::string, std::map<std::string, size_t>>& distance_map) {
	for (const auto& kv_o : distance_map) {
		cout << kv_o.first << ": \n";
		for (const auto& kv_i : kv_o.second) {
			cout << "  --> " << kv_i.first << ": " << kv_i.second << "\n";
		}
	}
}

inline size_t get_total_distance_entries_count(const std::map<std::string, std::map<std::string, size_t>>& distance_map) {
	size_t counter {0};
	for (const auto& kv_o : distance_map) {
		counter += kv_o.second.size();
	}
	counter += distance_map.size();
	return counter;
}

inline std::map<std::string,valve_ext> get_extended_valve_map(const std::map<std::string,valve_t>& simple_valve_map) {
	std::map<std::string,valve_ext> extended_valve_map {};
	for (const auto& kv : simple_valve_map) {
		extended_valve_map[kv.first] = {kv.first, kv.second.flow_rate, false, false, kv.second.next_valves};
	}
	return extended_valve_map;
}

void valve_game() {
	
	std::map<std::string,valve_t> valve_map {};
	constexpr bool use_test_data = false;
	constexpr bool use_observer = true;
	if constexpr (use_test_data) {
		valve_map.insert({"AA"s,{"AA"s,0,{"DD"s,"II"s,"BB"s}}});
		valve_map.insert({"BB"s,{"BB"s,13,{"CC"s,"AA"s}}});
		valve_map.insert({"CC"s,{"CC"s,2,{"DD"s,"BB"s}}});
		valve_map.insert({"DD"s,{"DD"s,20,{"CC"s,"AA"s, "EE"s}}});
		valve_map.insert({"EE"s,{"EE"s,3,{"FF"s,"DD"s}}});
		valve_map.insert({"FF"s,{"FF"s,0,{"EE"s,"GG"s}}});
		valve_map.insert({"GG"s,{"GG"s,0,{"FF"s,"HH"s}}});
		valve_map.insert({"HH"s,{"HH"s,22,{"GG"s}}});
		valve_map.insert({"II"s,{"II"s,0,{"AA"s,"JJ"s}}});
		valve_map.insert({"JJ"s,{"JJ"s,21,{"II"s}}});
	} else {
		valve_map.insert({"DJ"s, {"DJ"s, 0, {"ZH", "AA"}}});
		valve_map.insert({"LP"s, {"LP"s, 0, {"AA", "EE"}}});
		valve_map.insert({"GT"s, {"GT"s, 0, {"FJ", "AW"}}});
		valve_map.insert({"RO"s, {"RO"s, 5, {"NO", "FD", "QV", "BV"}}});
		valve_map.insert({"PS"s, {"PS"s, 0, {"FY", "UV"}}});
		valve_map.insert({"QV"s, {"QV"s, 0, {"EB", "RO"}}});
		valve_map.insert({"MV"s, {"MV"s, 0, {"FL", "EB"}}});
		valve_map.insert({"RN"s, {"RN"s, 0, {"AW", "LQ"}}});
		valve_map.insert({"HF"s, {"HF"s, 0, {"QN", "HW"}}});
		valve_map.insert({"PY"s, {"PY"s, 19, {"SN"}}});
		valve_map.insert({"AT"s, {"AT"s, 0, {"YQ", "UY"}}});
		valve_map.insert({"UY"s, {"UY"s, 3, {"KV", "ID", "AT", "PB", "PG"}}});
		valve_map.insert({"YI"s, {"YI"s, 0, {"FL", "FD"}}});
		valve_map.insert({"EB"s, {"EB"s, 8, {"MV", "GQ", "QV"}}});
		valve_map.insert({"ID"s, {"ID"s, 0, {"NO", "UY"}}});
		valve_map.insert({"FY"s, {"FY"s, 15, {"LQ", "PS"}}});
		valve_map.insert({"GQ"s, {"GQ"s, 0, {"EB", "KM"}}});
		valve_map.insert({"HW"s, {"HW"s, 0, {"FJ", "HF"}}});
		valve_map.insert({"CQ"s, {"CQ"s, 17, {"KM", "GO"}}});
		valve_map.insert({"AW"s, {"AW"s, 20, {"RN", "GT", "WH", "MX"}}});
		valve_map.insert({"BV"s, {"BV"s, 0, {"RO", "ZH"}}});
		valve_map.insert({"PB"s, {"PB"s, 0, {"UY", "AA"}}});
		valve_map.insert({"MX"s, {"MX"s, 0, {"AW", "YG"}}});
		valve_map.insert({"DE"s, {"DE"s, 4, {"MM", "PZ", "PG", "DS", "EP"}}});
		valve_map.insert({"AA"s, {"AA"s, 0, {"EP", "PB", "LP", "JT", "DJ"}}});
		valve_map.insert({"QN"s, {"QN"s, 23, {"SN", "HF"}}});
		valve_map.insert({"GO"s, {"GO"s, 0, {"CQ", "MK"}}});
		valve_map.insert({"PZ"s, {"PZ"s, 0, {"IJ", "DE"}}});
		valve_map.insert({"PG"s, {"PG"s, 0, {"UY", "DE"}}});
		valve_map.insert({"FL"s, {"FL"s, 18, {"MV", "YI"}}});
		valve_map.insert({"DS"s, {"DS"s, 0, {"DE", "ZH"}}});
		valve_map.insert({"ZH"s, {"ZH"s, 11, {"YQ", "BV", "DJ", "DS", "SB"}}});
		valve_map.insert({"KV"s, {"KV"s, 0, {"UY", "IJ"}}});
		valve_map.insert({"UV"s, {"UV"s, 9, {"MM", "PS", "YG"}}});
		valve_map.insert({"WH"s, {"WH"s, 0, {"JT", "AW"}}});
		valve_map.insert({"FD"s, {"FD"s, 0, {"YI", "RO"}}});
		valve_map.insert({"FJ"s, {"FJ"s, 24, {"HW", "GT"}}});
		valve_map.insert({"JT"s, {"JT"s, 0, {"AA", "WH"}}});
		valve_map.insert({"SN"s, {"SN"s, 0, {"PY", "QN"}}});
		valve_map.insert({"KM"s, {"KM"s, 0, {"GQ", "CQ"}}});
		valve_map.insert({"LQ"s, {"LQ"s, 0, {"RN", "FY"}}});
		valve_map.insert({"NO"s, {"NO"s, 0, {"ID", "RO"}}});
		valve_map.insert({"SB"s, {"SB"s, 0, {"ZH", "IJ"}}});
		valve_map.insert({"MK"s, {"MK"s, 25, {"GO"}}});
		valve_map.insert({"YG"s, {"YG"s, 0, {"MX", "UV"}}});
		valve_map.insert({"IJ"s, {"IJ"s, 16, {"EE", "KV", "PZ", "SB"}}});
		valve_map.insert({"EP"s, {"EP"s, 0, {"AA", "DE"}}});
		valve_map.insert({"MM"s, {"MM"s, 0, {"UV", "DE"}}});
		valve_map.insert({"YQ"s, {"YQ"s, 0, {"AT", "ZH"}}});
		valve_map.insert({"EE"s, {"EE"s, 0, {"LP", "IJ"}}});
	}
	/**///NOLINT
	
	auto valve_map_extend = get_extended_valve_map(valve_map);
	auto start1 = steady_clock::now();
	distance_table = build_distance_table(valve_map);
	auto end1 = steady_clock::now();
	cout << "TIME1: " << duration_cast<microseconds>(end1 - start1).count() << " mus\n";
	cout << "TIME1: " << duration_cast<milliseconds>(end1 - start1).count() << " millis\n";
	cout << "Distance Table Size: " << get_total_distance_entries_count(distance_table) << "\n";
	//cout << "------------------------------------------------------\n";
	
	
	rounds_total = 27;
	
	std::atomic_bool done {false};
	std::future<void> observer_future;
	//omp_set_max_active_levels(0);
	omp_set_max_active_levels(1);
	omp_set_dynamic(0);
	
	auto start2 = steady_clock::now();
	if constexpr (use_observer) {	
		observer_future = std::async(std::launch::async,  [&done, &start2](){
				unsigned long long milliseconds_passed = 0;
				while (!done) {
					std::this_thread::sleep_for(1s);
					auto end_local = steady_clock::now();
					milliseconds_passed = duration_cast<milliseconds>(end_local - start2).count();
					cout << "Game_step_calls after " << milliseconds_passed/1000 << " seconds: " << game_step_calls_at << "\n";
					/*{
						std::shared_lock obs_sync_lock (observer_sync_mutex);
						cout << "Game_step_calls after " << milliseconds_passed/1000 << " seconds: " << game_step_calls << "\n";
					}*/
				}
			}
		);
	}
	game_step_v1(valve_map_extend, rounds_total, -2, -2, 0, "AA", "AA", "");
	auto end2 = steady_clock::now();
	done = true;
	if constexpr (use_observer) {
		observer_future.get();
	}
	//cout << "Best Game string:\n" << max_value_game_path << "\n\n";
	
	cout << "TIME2: " << duration_cast<microseconds>(end2 - start2).count() << " mus\n";
	cout << "TIME2: " << duration_cast<milliseconds>(end2 - start2).count() << " millis\n";
	cout << "\nTIME12: " << duration_cast<milliseconds>(end2 - start1).count() << " millis\n";
	
	cout << "Max-Value afterwards: " << max_value << "\n";
	cout << "     game_step_calls: " << game_step_calls_at << "\n";
	//cout << "     game_step_calls: " << game_step_calls << "\n";
	
}



int main(int argc, char **argv){
	valve_game();
	cout << "------------------------------------------------------\n";
	return 0;
}
