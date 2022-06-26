#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <iostream>
#include <vector>
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

class Memory {
  int _size;
public:
  Memory(int size);
  virtual ~Memory();
  tuple<int,int> address_range(ublas::matrix<int> assignments);
  tuple<int,int> address_range(vector<int> positions);
  ublas::matrix<int> assign(const vector<vector<int> > & tasks, int step = -1);
};

#endif
