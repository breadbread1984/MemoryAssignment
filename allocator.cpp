#include <stdexcept>
#include <limits>
#include <algorithm>
#include "allocator.hpp"

using namespace std;

Allocator::Allocator() {
}

Allocator::~Allocator() {
}

int Allocator::cost(const Assignment & assignment, const map<int, vector<Task> > & tasks) {
  int min_value = numeric_limits<int>::max();
  for (auto itr = assignment.begin() ; itr != assignment.end() ; itr++) {
    map<int,vector<Task> >::const_iterator which_time;
    if ((which_time = tasks.find(itr->time_index)) == tasks.end()) {
      throw logic_error("invalid time index in disposition object!");
    }
    const vector<Task> & tasks = which_time->second;
    if (itr->task_index >= tasks.size()) {
      throw logic_error("invalid task index in disposition object!");
    }
    int address = itr->address;
    const Task & task = tasks[itr->task_index];
    if (address + task.size < min_value) {
      min_value = address + task.size;
    }
  }
  return min_value;
}

std::tuple<int, int> Allocator::bound(const Assignment & assignment, const Task & task) {
  
}

Assignment Allocator::solve(const map<int, vector<Task> > & tasks) {
}
