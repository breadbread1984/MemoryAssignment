#include <set>
#include <limits>
#include <stdexcept>
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

vector<int> & Counter::operator*() {
  return _counter;
}

Memory::Memory(int size): _size(size) {
}

Memory::~Memory() {
}

tuple<int,int> Memory::address_range(ublas::matrix<int> assignments) {
  ublas::scalar_matrix<int> ones(assignments.size2(), 1);
  ublas::matrix<int> row_sum = prod(assignments, ones);
  int lower = 0, higher = 0;
  for (lower = 0 ; lower < row_sum.size1() && row_sum(lower, 0) == 0 ; lower++);
  lower = (lower == row_sum.size1())?-1:lower;
  for (higher = row_sum.size1() - 1 ; higher >= 0 && row_sum(higher, 0) == 0 ; higher--);
  return make_tuple(lower, higher);
}

tuple<int,int> Memory::address_range(vector<int> positions) {
  int lower = 0, higher = 0;
  for (lower = 0 ; lower < positions.size() && positions[lower] == 0 ; lower++);
  lower = (lower == positions.size())?-1:lower;
  for (higher = positions.size() - 1 ; higher >= 0 && positions[higher] == 0 ; higher--);
  return make_tuple(lower, higher);
}

ublas::matrix<int> Memory::assign(const vector<vector<int> > & tasks, int step) {
  assert(tasks.size());
  if (step == -1) {
    // default value means recursive from the last step
    step = tasks.size() - 1;
  }
  int task_num = tasks[step].size();
  ublas::matrix<int> assignments;
  if (step != 0) {
    assignments = assign(tasks, step - 1); // assignments.shape = (_size, step - 1)
#ifndef NDEBUG
    cout<<assignments<<endl;
#endif
  } else {
    assignments = ublas::zero_matrix<int>(_size, 0);
  }
  ublas::matrix<int> new_assignments = ublas::zero_matrix<int>(_size, step + 1);
  ublas::matrix_range<ublas::matrix<int> > left(new_assignments, ublas::range(0, _size), ublas::range(0, step));
  ublas::matrix_range<ublas::matrix<int> > right(new_assignments, ublas::range(0, _size), ublas::range(step, step + 1));
  left = assignments;
  int lower = 0, higher = 0;
  tie(lower, higher) = address_range(left);
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
    for (int i = 0 ; i < task_num ; i++) {
      int j;
      for (j = assignments.size2() - 1 ; j >= 0 ; j--) {
	if (assignments(i,j)) break;
      }
      if (!(j < 0 || assignments(i,j) <= assignments.size2() - j)) {
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

