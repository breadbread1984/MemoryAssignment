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
}
