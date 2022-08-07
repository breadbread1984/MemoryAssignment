#include <stdexcept>
#include <limits>
#include <algorithm>
#include "allocator.hpp"

using namespace std;

Allocator::Allocator() {
}

Allocator::~Allocator() {
}

int Allocator::cost(const Assignment & assignment, const vector<Task> & tasks) {
  int min_value = numeric_limits<int>::max();
  for (auto itr = assignment.begin() ; itr != assignment.end() ; itr++) {
    if (itr->index >= tasks.size()) {
      throw logic_error("invalid task id!");
    }
    int address = itr->address;
    const Task & task = tasks[itr->index];
    if (address + task.size < min_value) {
      min_value = address + task.size;
    }
  }
  return min_value;
}

std::tuple<int, int> Allocator::bound(const Assignment & assignment, const Task & task) {
}

Assignment Allocator::solve(const vector<Task> & tasks) {
}
