#include <set>
#include <limits>
#include <stdexcept>
#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <boost/numeric/ublas/io.hpp>
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

Assignment::Assignment(vector<tuple<int,int> > tasks, vector<int> positions) {
  assert(tasks.size() == positions.size());
  for (int i = 0 ; i < tasks.size() ; i++) {
    assignment[positions[i]] = tasks[i];
  }
}

Assignment::~Assignment() {
}

tuple<int,int> Assignment::address_range() {
  if (0 == assignment.size()) {
    return make_tuple(0,0);
  } else {
    lower = assignment.begin()->first;
    higher = assignment.rbegin()->first + assignment.rbegin()->second.first; //start_address + size
    return make_tuple(lower, higher);
  }
}

Assignments::Assignments() {
}

Assignments::Assignments(const Assignments& a): assignments(a.assignments) {
}

Assignments::~Assignments() {
}

void Assignments::append(Assignment assignment) {
  assignments.push_back(assignment);
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

vector<int> & Counter::operator*() {
  return _counter;
}

Memory::Memory(int size): _size(size) {
}

Memory::~Memory() {
}

tuple<int,int> Memory::address_range(Assignments& assignments) {
  return assignments.address_range();
}

tuple<int,int> Memory::address_range(Assignment& assignment) {
  return assignment.address_range();
}

ublas::matrix<int> Memory::assign(const vector<vector<tuple<int,int> > > & tasks, int step) {
  assert(tasks.size());
  if (step == -1) {
    // default value means recursive from the last step
    step = tasks.size() - 1;
  }
  int task_num = tasks[step].size();
  Assignment assignments;
  if (step != 0) {
    assignments = assign(tasks, step - 1); // assignments.shape = (_size, step - 1)
#ifndef NDEBUG
    cout<<assignments<<endl;
#endif
  }
  int lower = 0, higher = 0;
  tie(lower, higher) = assignments.address_range();
  int minimum_higher = numeric_limits<int>::max();
  for (auto itr = Counter(_size, task_num) ; (*itr).size() != 0 ; itr++) {
    // for every candidate assignment
    vector<int> positions = *itr;
    set<int> pos_set(positions.begin(), positions.end());
    if (pos_set.size() != task_num) {
      // if any two task are assigned to the same memory position
      // skip current assignment
      continue;
    }
    // checkout collision
    bool collided = false;
    for (int t = 0 ; t < positions.size() ; t++) {
      int j;
      for (j = assignments.size2() - 1 ; j >= 0 ; j--) {
	      if (assignments(positions[t],j)) break;
      }
      if (!(j < 0 || assignments(positions[t],j) <= assignments.size2() - j)) {
	// current task is collided
	collided = true;
	break;
      }
    }
    if (false == collided) {
      // if all tasks are not collided
      int new_lower = 0, new_higher = 0;
      tie(new_lower, new_higher) = address_range(positions);
      if (max(new_higher, higher) < minimum_higher) {
        minimum_higher = max(new_higher, higher);
        right = ublas::zero_matrix<int>(_size, 1);
        for (int t = 0 ; t < positions.size() ; t++) {
          right(positions[t], 0) = tasks[step][t];
        }
      }
    }
  }
  if (minimum_higher == numeric_limits<int>::max()) {
    throw runtime_error("no candidate solution available at step " + lexical_cast<string>(step));
  }
  return new_assignments;
}

