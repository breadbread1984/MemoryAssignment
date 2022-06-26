#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <iostream>
#include <vector>
#include <map>
#include <iterator>
#include <tuple>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/io.hpp>

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
  map<int,tuple<int,int> > assignment; // address->(size, time)
public:
  Assignment(vector<tuple<int,int> > tasks, vector<int> positions);
  virtual ~Assignment();
  tuple<int,int> address_range();
};

class Assignments {
  vector<Assignment> assignments;
public:
  Assignments();
  Assignments(const Assignments& a);
  virtual ~ Assignments();
  void append(Assignment assignment);
  tuple<int,int> address_range();
};

class Memory {
  int _size;
protected:
  tuple<int,int> address_range(Assignments& assignments);
  tuple<int,int> address_range(Assignment& assignment);
public:
  Memory(int size);
  virtual ~Memory();
  ublas::matrix<int> assign(const vector<vector<tuple<int,int> > > & tasks, int step = -1);
};

#endif
