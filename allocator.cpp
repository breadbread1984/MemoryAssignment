#include <stdexcept>
#include <limits>
#include <algorithm>
#include "allocator.hpp"

using namespace std;

Allocator::Allocator() {
}

Allocator::~Allocator() {
}

const Task & Allocator::get_task(const map<int, vector<Task> > & tasks, int time_index, int task_index) {
  map<int,vector<Task> >::const_iterator which_time;
  if ((which_time = tasks.find(time_index)) == tasks.end()) {
    throw logic_error("invalid time index in disposition object!");
  }
  const vector<Task> & tasks_of_this_time = which_time->second;
  if (task_index >= tasks_of_this_time.size()) {
    throw logic_error("invalid task index in disposition object!");
  }
  return tasks_of_this_time[task_index];
}

int Allocator::temporal_intersect(int left1, int right1, int left2, int right2) {
  int left = max(left1, left2);
  int right = min(right1, right2);
  return max(0, right - left);
}

int Allocator::cost(const Assignment & assignment, const map<int, vector<Task> > & tasks) {
  int max_value = 0;
  for (auto itr = assignment.begin() ; itr != assignment.end() ; itr++) {
    const Disposition & disposition = *itr;
    const Task & task = get_task(tasks, disposition.time_index, disposition.task_index);
    int address = disposition.address;
    if (address + task.size > max_value) {
      max_value = address + task.size;
    }
  }
  return max_value;
}

std::tuple<int,int> Allocator::bound(const Assignment & assignment, const map<int, vector<Task> > & tasks) {
  // 1) find the max address for every temporal step
  map<int, int> max_addresses; // time->max_address
  for (auto itr = assignment.begin() ; itr != assignment.end() ; itr++) {
    const Disposition & disposition = *itr;
    const Task & task = get_task(tasks, disposition.time_index, disposition.task_index);
    int address = disposition.address;
    for (int e = 0 ; e < task.elapse ; e++) {
      if (max_addresses.find(disposition.time_index + e) == max_addresses.end()) {
        max_addresses[disposition.time_index + e] = 0;
      }
      if (address + task.size > max_addresses[disposition.time_index + e]) {
        max_addresses[disposition.time_index + e] = address + task.size;
      }
    }
  }
  // 2) find the max temporal step the tasks last
  int max_temporal = 0;
  for (map<int,vector<Task> >::const_iterator itr = tasks.cbegin() ; itr != tasks.cend() ; itr++) {
    for (int i = 0 ; i < itr->second.size() ; i++) {
      if (max_temporal < itr->first + itr->second[i].elapse) {
        max_temporal = itr->first + itr->second[i].elapse;
      }
    }
  }
  // NOTE: if the key set of max_addresses doesnt cover the full temporal set, there max be some temporal step has no assignment.
  int min_address = (max_addresses.size() < max_temporal)?0:min_element(max_addresses.begin(), max_addresses.end(),
                                                                        [](const pair<int, int> & a, const pair<int, int> & b) {
                                                                          return a.second < b.second;
                                                                        })->second;
  int max_address = max_element(max_addresses.begin(), max_addresses.end(),
                                [](const pair<int, int> & a, const pair<int, int> & b) {
                                  return a.second < b.second;
                                })->second;
  return make_tuple(min_address, max_address);
}

std::tuple<int,int> Allocator::heuristic(remaining_tasks tids, const map<int, vector<Task> > & tasks) {
  int total_size = 0;
  int max_size = 0;
  for (auto& tid: tids) {
    const Task & task = get_task(tasks, get<0>(tid), get<1>(tid));
    total_size += task.size;
    if (task.size > max_size) {
      max_size = task.size;
    }
  }
  return make_tuple(max_size, total_size);
}

Disposition Allocator::assign(const Assignment & assignment, const map<int, vector<Task> > & tasks, task_id tid) {
  // place current task according to existing assigned tasks
  const Task & task = get_task(tasks, get<0>(tid), get<1>(tid));
  // get max address of temporal intersected tasks
  int max_value = 0;
  for (auto itr = assignment.begin() ; itr != assignment.end() ; itr++) {
    const Disposition & disposition1 = *itr;
    const Task & task1 = get_task(tasks, disposition1.time_index, disposition1.task_index);
    int left1 = disposition1.time_index;
    int right1 = disposition1.time_index + task1.elapse;
    const Task & task2 = get_task(tasks, get<0>(tid), get<1>(tid));
    int left2 = get<0>(tid);
    int right2 = get<0>(tid) + task2.elapse;
    int intersect = temporal_intersect(left1, right1, left2, right2);
    if (intersect) {
      int down1 = disposition1.address;
      int up1 = disposition1.address + task1.size;
      if (up1 > max_value) {
        max_value = up1;
      }
    }
  }
  // make assignment
  Disposition disposition2{get<0>(tid), get<1>(tid), max_value};
  return disposition2;
}

Assignment Allocator::solve(const map<int, vector<Task> > & tasks) {
  // 1) make the root node
  remaining_tasks tasks_to_assign;
  for (map<int, vector<Task> >::const_iterator itr = tasks.cbegin() ; itr != tasks.cend() ; itr++) {
    for (int i = 0; i < itr->second.size() ; i++) {
      tasks_to_assign.push_back(make_tuple(itr->first, i));
    }
  }
  if (0 == tasks_to_assign.size()) {
    throw logic_error("at leat one task in the tasks container!");
  }
  Assignment best_solution;
  int lowest_cost = numeric_limits<int>::max();
  Queue solutions;
  Assignment origin;
  int address_lower, address_upper;
  std::tie(address_lower, address_upper) = bound(origin, tasks);
  int size_lower, size_upper;
  std::tie(size_lower, size_upper) = heuristic(tasks_to_assign, tasks);
  solutions.push_back(make_tuple(origin, tasks_to_assign, address_lower + size_lower, address_upper + size_upper));
  // 2) start searching
  while(solutions.size()) {
    // find the minimum upper bound
    auto which_solution = min_element(solutions.begin(), solutions.end(), [](const solution_candidate & a, const solution_candidate & b) {
                                                                            int upper_a = get<3>(a);
                                                                            int upper_b = get<3>(b);
                                                                            return upper_a < upper_b;
                                                                          });
    int minimum_upper = get<3>(*which_solution);
    // remove solutions with lower bound higher than than the minimum upper bound
    auto remove_from = remove_if(solutions.begin(), solutions.end(), [&](const solution_candidate & a) {
                                                                       return get<2>(a) > minimum_upper;
                                                                     });
    solutions.erase(remove_from, solutions.end());
    // NOTE: at lease one candidate solution whose minimum upper equals minimum_upper must be in solutions.
    assert (solutions.size());
    // sort with upper address estimation in ascend order
    sort(solutions.begin(), solutions.end(), [](const solution_candidate & a, const solution_candidate & b) {
                                               int upper_a = get<3>(a);
                                               int upper_b = get<3>(b);
                                               return upper_a < upper_b;
                                             });
    // extend the first candidate solution in solutions
    solution_candidate solution = solutions.front();
    solutions.pop_front();
    Assignment & assignment = get<0>(solution);
    remaining_tasks & tasks_to_assign = get<1>(solution);
    for (int i = 0 ; i < tasks_to_assign.size() ; i++) {
      task_id & tid = tasks_to_assign[i];
      Disposition disposition = assign(assignment, tasks, tid);
      Assignment assignment_copy(assignment);
      assignment_copy.insert(disposition);
      remaining_tasks tasks_to_assign_copy(tasks_to_assign);
      tasks_to_assign_copy.erase(tasks_to_assign_copy.begin() + i);
      if (0 == tasks_to_assign_copy.size()) {
        // if this is a full assignment of all tasks, compare with the current best solution
        int c = cost(assignment_copy, tasks);
        if (c < lowest_cost) {
          lowest_cost = c;
          best_solution = assignment_copy;
        }
      } else {
        // if this is not a full assignment of all tasks, put the candidate solution to open list
        int address_lower, address_upper;
        std::tie(address_lower, address_upper) = bound(assignment_copy, tasks);
        int size_lower, size_upper;
        std::tie(size_lower, size_upper) = heuristic(tasks_to_assign_copy, tasks);
        solutions.push_back(make_tuple(assignment_copy, tasks_to_assign_copy, address_lower + size_lower, address_upper + size_upper));
      }
    } // extend all candidate solutions from this candidate solution
  } // while the candidate solutions are not fully explored
  return best_solution;
}
