#ifndef ALLOCATOR_HPP
#define ALLOCATOR_HPP

#include <vector>
#include <tuple>
#include <queue>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>

using namespace std;
using namespace boost;
using namespace boost::multi_index;

struct Task {
  int size;
  int elapse;
};

struct Disposition {
  int index; // index to task
  int address;
  int time;
};

struct IndexedByAddress {};
struct IndexedByTime {};

using Assignment = multi_index_container<
  Disposition,
  indexed_by<
    ordered_non_unique<
      tag<IndexedByAddress>,
      member<Disposition, int, &Disposition::address>
    >,
    ordered_non_unique<
      tag<IndexedByTime>,
      member<Disposition, int, &Disposition::time>
    >
  >
>;

class Allocator {
  using Queue = queue<Assignment>;
protected:
  int cost(const Assignment & assignment, const vector<Task> & tasks);
  std::tuple<int, int> bound(const Assignment & assignment, const Task & task);
public:
  Allocator();
  virtual ~Allocator();
  Assignment solve(const vector<Task> & tasks);
};

#endif
