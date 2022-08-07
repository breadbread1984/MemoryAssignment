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

int Allocator::cost(const Assignment & assignment, const map<int, vector<Task> > & tasks) {
  int min_value = numeric_limits<int>::max();
  for (auto itr = assignment.begin() ; itr != assignment.end() ; itr++) {
    const Disposition & disposition = *itr;
    const Task & task = get_task(tasks, disposition.time_index, disposition.task_index);
    int address = disposition.address;
    if (address + task.size < min_value) {
      min_value = address + task.size;
    }
  }
  return min_value;
}

Disposition Allocator::assign(const Assignment & assignment, const map<int, vector<Task> > & tasks, task_id tid) {
  // place current task according to existing assigned tasks
  const Task & task = get_task(tasks, get<0>(tid), get<1>(tid));
  // get temporal related assigned tasks
  for (auto itr = assignment.begin() ; itr != assignment.end() ; itr++) {
    const Disposition & disposition = *itr;
    const Task & task = get_task(tasks, disposition.time_index, disposition.task_index);
    int down1 = disposition.address;
    int up1 = disposition.address + task.size;
    int left1 = disposition.time_index;
    int right1 = disposition.time_index + task.elapse;
    
  }
}

Assignment Allocator::solve(const map<int, vector<Task> > & tasks) {
}
