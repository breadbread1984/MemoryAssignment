#include <limits>
#include <stdexcept>
#include <algorithm>
#include <boost/lexical_cast.hpp>
#include "memory.hpp"

using namespace std;
using namespace boost;

Counter::Counter(int base, int digits): _base(base), _digits(digits), _counter(_digits, 0) {
}

Counter::Counter(const Counter& counter): _base(counter._base), _digits(counter._digits), _counter(counter._counter) {
}

Counter::~Counter() {
}

void Counter::next() {
  int carry = 1;
  bool last = false;
  for(auto itr = _counter.rbegin() ; itr != _counter.rend() ; itr++) {
    int new_value = (*itr + carry) % _base;
    if ((*itr + carry) / _base) {
      carry = 1;
      if (itr + 1 == _counter.rend()) {
        last = true;
      }
    } else {
      carry = 0;
    }
    *itr = new_value;
  }
  if (last) {
    _counter = vector<int>();
  }
}

Counter& Counter::operator++() {
  next();
  return *this;
}

Counter Counter::operator++(int) {
  Counter counter(*this);
  next();
  return counter;
}

bool Counter::operator==(const Counter& rhs) const {
  if (_counter.size() != rhs._counter.size()) {
    return false;
  } else {
    for (int i = 0 ; i < _counter.size() ; i++) {
      if (_counter[i] != rhs._counter[i]) {
        return false;
      }
    }
    return true;
  }
}

bool Counter::operator!=(const Counter& rhs) const {
  return !((*this)==rhs);
}

vector<int> & Counter::operator*() {
  return _counter;
}

Assignment::Assignment(int size): _size(size) {
}

Assignment& Assignment::operator=(const Assignment& a) {
  _size = a._size;
  assignment = a.assignment;
  return *this;
}

Assignment::~Assignment() {
}

bool Assignment::append(int address, int size, int elapse) {
  // detect whether the task is over the address range
  if (address + size > _size) {
    return false;
  }
  // detect whether the task is overlap with other tasks
  for (map<int, tuple<int,int> >::iterator itr = assignment.begin() ; itr != assignment.end() ; itr++) {
    int left_y1 = itr->first;
    int left_y2 = itr->first + std::get<0>(itr->second);
    int right_y1 = address;
    int right_y2 = address + size;
    int intersec_y1 = max(left_y1, right_y1);
    int intersec_y2 = min(left_y2, right_y2);
    int intersec_h = max(intersec_y2 - intersec_y1, 0);
    if (intersec_h) {
      return false;
    }
  }
  assignment[address] = make_tuple(size, elapse);
  return true;
}

tuple<int,int> Assignment::address_range() {
  if (0 == assignment.size()) {
    return make_tuple(0,0);
  } else {
    int lower = assignment.begin()->first;
    int higher = assignment.rbegin()->first + std::get<0>(assignment.rbegin()->second); //start_address + size
    return make_tuple(lower, higher);
  }
}

map<int,tuple<int,int> > & Assignment::get() {
  return assignment;
}

Assignments::Assignments() {
}

Assignments::Assignments(const Assignments& a): assignments(a.assignments) {
}

Assignments& Assignments::operator=(const Assignments& a) {
  assignments = a.assignments;
  return *this;
}

Assignments::~Assignments() {
}

bool Assignments::append(Assignment assignment) {
  map<int,tuple<int,int> > & detail = assignment.get();
  int time = assignments.size();
  for (map<int,tuple<int,int> >::iterator itr = detail.begin() ; itr != detail.end() ; itr++) {
    int position = itr->first;
    int size = std::get<0>(itr->second);
    int elapse = std::get<1>(itr->second);
    if (collide(size, elapse, position, time)) {
      return false;
    }
  }
  assignments.push_back(assignment);
  return true;
}

bool Assignments::collide(int size, int elapse, int position, int time) {
  // FIXME: use better collision detection algorithm
  for (int t = 0 ; t < assignments.size() ; t++) {
    // for every timestamp
    auto & detail = assignments[t].get();
    for (map<int,tuple<int,int> >::iterator itr = detail.begin() ; itr != detail.end() ; itr++) {
      // for every tasks in this timestamp
      // detect collision with the given assignment position
      int left_x1 = t;
      int left_y1 = itr->first;
      int left_h = std::get<0>(itr->second);
      int left_w = std::get<1>(itr->second);
      int left_x2 = left_x1 + left_w;
      int left_y2 = left_y1 + left_h;
      int right_x1 = time;
      int right_y1 = position;
      int right_h = size;
      int right_w = elapse;
      int right_x2 = right_x1 + right_w;
      int right_y2 = right_y1 + right_h;
      int intersec_x1 = max(left_x1, right_x1);
      int intersec_x2 = min(left_x2, right_x2);
      int intersec_y1 = max(left_y1, right_y1);
      int intersec_y2 = min(left_y2, right_y2);
      int w = max(intersec_x2 - intersec_x1, 0);
      int h = max(intersec_y2 - intersec_y1, 0);
      int intersec_area = h * w;
      if (intersec_area) {
        return true;
      }
    }
  }
  return false;
}

tuple<int,int> Assignments::address_range() {
  vector<int> lowers, highers;
  for (auto & assignment : assignments) {
    int lower, higher;
    tie(lower, higher) = assignment.address_range();
    lowers.push_back(lower);
    highers.push_back(higher);
  }
  if (0 == assignments.size()) {
    return make_tuple(0,0);
  } else {
    int lower = *min_element(lowers.begin(), lowers.end());
    int higher = *max_element(highers.begin(), highers.end());
    return make_tuple(lower, higher);
  }
}

vector<Assignment> & Assignments::get() {
  return assignments;
}

Memory::Memory(int size): _size(size) {
}

Memory::~Memory() {
}

Assignments Memory::assign(const vector<vector<tuple<int,int> > > & tasks, int step) {
  assert(tasks.size());
  if (step == -1) {
    // default value means recursive from the last step
    step = tasks.size() - 1;
  }
  int task_num = tasks[step].size();
  Assignments assignments;
  if (step != 0) {
    assignments = assign(tasks, step - 1); // assignments.shape = (_size, step - 1)
  }
  int minimum_higher = numeric_limits<int>::max();
  Assignments best_assignments;
  for (auto itr = Counter(_size, task_num) ; (*itr).size() != 0 ; itr++) {
    // for every candidate assignment
    // 1) check whether tasks of current timestamp collide with each other
    vector<int> positions = *itr;
    Assignment assignment(_size);
    bool collide = false;
    for (int t = 0 ; t < tasks[step].size() ; t++) {
      if (false == assignment.append(positions[t], std::get<0>(tasks[step][t]), std::get<1>(tasks[step][t]))) {
        // collision of any task will skip current candidate assignment
        collide = true;
        break;
      }
    }
    if (collide) {
      continue;
    }
    // 2) check whether tasks of current timestamp collide with tasks of past timestamps
    Assignments new_assignments(assignments);
    if (false == new_assignments.append(assignment)) {
      continue;
    }
    // 3) select assignments with the minimum higher address
    int new_lower = 0, new_higher = 0;
    tie(new_lower, new_higher) = new_assignments.address_range();
    if (new_higher < minimum_higher) {
      best_assignments = new_assignments;
      minimum_higher = new_higher;
    }
  }
  if (minimum_higher == numeric_limits<int>::max()) {
    throw runtime_error("no candidate solution available at step " + lexical_cast<string>(step));
  }
  return best_assignments;
}
