#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <iostream>
#include <vector>
#include <map>
#include <iterator>
#include <tuple>

using namespace std;
namespace ublas = boost::numeric::ublas;

class Counter: public iterator<forward_iterator_tag, vector<int>> {
  int _base, _digits;
  vector<int> _counter;
protected:
  virtual void next();
public:
  Counter(int base, int digits);
  Counter(const Counter& counter);
  virtual ~Counter();
  Counter& operator++();
  Counter operator++(int);
  bool operator==(const Counter& rhs) const;
  bool operator!=(const Counter& rhs) const;
  vector<int> & operator*();
};

class Assignment {
  int _size;
  map<int,tuple<int,int> > assignment; // address->(size, time)
public:
  Assignment(int size);
  Assignment& operator=(const Assignment& a);
  virtual ~Assignment();
  bool append(int address, int size, int elapse);
  tuple<int,int> address_range();
  map<int,tuple<int,int> > & get();
};

class Assignments {
  vector<Assignment> assignments;
protected:
  bool collide(int size, int elapse, int position, int time);
public:
  Assignments();
  Assignments(const Assignments& a);
  Assignments& operator=(const Assignments& a);
  virtual ~Assignments();
  bool append(Assignment assignment);
  tuple<int,int> address_range();
  vector<Assignment> & get();
};

class Memory {
  int _size;
public:
  Memory(int size);
  virtual ~Memory();
  Assignments assign(const vector<vector<tuple<int,int> > > & tasks, int step = -1);
};

#endif
