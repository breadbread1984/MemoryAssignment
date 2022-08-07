#include <stdexcept>
#include <algorithm>
#include "allocator.hpp"

using namespace std;

Allocator::Allocator() {
}

Allocator::~Allocator() {
}

int Allocator::cost(const Assignment & assignment, const vector<Task> & tasks) {
  for (auto itr = assignment.begin() ; itr != assignment.end() ; itr++) {
    if (itr->index >= tasks.size()) {
      throw logic_error("invalid task id!");
    }
  }
}

std::tuple<int, int> Allocator::bound(const Assignment & assignment, const Task & task) {
}

Assignment Allocator::solve(const vector<Task> & tasks) {
}
