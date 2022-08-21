#include <stdexcept>
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

int Allocator::intersect(int left1, int right1, int up1, int down1, int left2, int right2, int up2, int down2) {
  int left = max(left1, left2);
  int right = min(right1, right2);
  int down = max(down1, down2);
  int up = min(up1, up2);
  int width = max(0, right - left);
  int height = max(0, up - down);
  return width * height;
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

Disposition Allocator::assign(const Assignment & assignment, const map<int, vector<Task> > & tasks, task_id tid) {
  const Task & task = get_task(tasks, get<0>(tid), get<1>(tid));
  // get temporal overlapped tasks
  vector<Disposition> temporal_overlapped_dispositions;
  for (auto itr = assignment.begin() ; itr != assignment.end() ; itr++) {
    const Disposition & disposition1 = *itr;
    const Task & task1 = get_task(tasks, disposition1.time_index, disposition1.task_index);
    int left1 = disposition1.time_index;
    int right1 = disposition1.time_index + task1.elapse;
    const Task & task2 = task;
    int left2 = get<0>(tid);
    int right2 = get<0>(tid) + task2.elapse;
    int intersect = temporal_intersect(left1, right1, left2, right2);
    if (intersect) {
      temporal_overlapped_dispositions.push_back(disposition1);
    }
  }
  // is no temporal overlapped tasks, assign current task to address 0
  if (0 == temporal_overlapped_dispositions.size()) {
    return Disposition{get<0>(tid), get<1>(tid), 0};
  }
  // get candidate address
  vector<int> candidate_addresses;
  for (const Disposition & d : temporal_overlapped_dispositions) {
    const Task & task = get_task(tasks, d.time_index, d.task_index);
    int address = d.address + task.size;
    candidate_addresses.push_back(address);
  }
  sort(candidate_addresses.begin(), candidate_addresses.end());
  // find the lowest address current task can fit in
  for (int address: candidate_addresses) {
    bool overlap = false;
    for (const Disposition & d : temporal_overlapped_dispositions) {
      const Task & task1 = get_task(tasks, d.time_index, d.task_index);
      int left1 = d.time_index;
      int right1 = d.time_index + task1.elapse;
      int down1 = d.address;
      int up1 = d.address + task1.size;
      const Task & task2 = task;
      int left2 = get<0>(tid);
      int right2 = get<0>(tid) + task2.elapse;
      int down2 = address;
      int up2 = address + task2.elapse;
      int area = intersect(left1, right1, up1, down1, left2, right2, up2, down2);
      if (area) {
        overlap = true;
        break;
      }
    }
    if (false == overlap) {
      return Disposition{get<0>(tid), get<1>(tid), address};
    }
  }
  throw logic_error("must not reach this line of the code!");
}

Assignment Allocator::solve(const map<int, vector<Task> > & tasks) {
  remaining_tasks tasks_to_assign;
  for (map<int, vector<Task> >::const_iterator itr = tasks.cbegin() ; itr != tasks.cend() ; itr++) {
    for (int i = 0 ; i < itr->second.size() ; i++) {
      tasks_to_assign.push_back(make_tuple(itr->first, i));
    }
  }
  // sort in descended order
  sort(tasks_to_assign.begin(), tasks_to_assign.end(), [&](const task_id & a, const task_id & b) {
    const Task & task_a = get_task(tasks, get<0>(a), get<1>(a));
    const Task & task_b = get_task(tasks, get<0>(b), get<1>(b));
    return task_a.size * task_a.elapse > task_b.size * task_b.elapse;
  });
  // assign task in order
  Assignment solution;
  for (task_id & tid : tasks_to_assign) {
    Disposition disposition = assign(solution, tasks, tid);
    solution.insert(disposition);
  }
  return solution;
}

int Allocator::size(const Assignment & assignment, const map<int, vector<Task> > & tasks) {
  return cost(assignment, tasks);
}

int Allocator::elapse(const map<int, vector<Task> > & tasks) {
  int max_temporal = 0;
  for (map<int,vector<Task> >::const_iterator itr = tasks.cbegin() ; itr != tasks.cend() ; itr++) {
    for (int i = 0 ; i < itr->second.size() ; i++) {
      if (max_temporal < itr->first + itr->second[i].elapse) {
        max_temporal = itr->first + itr->second[i].elapse;
      }
    }
  }
  return max_temporal;
}
