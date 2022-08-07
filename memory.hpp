#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <queue>
#include <boost/multi_index_container.hpp>

using namespace std;
using namespace boost;
using namespace boost::multi_index;

struct Task {
  int size;
  int elapse;
};

struct Disposition {
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
      member<Disposition, int, &DispositionL::time>
    >
  >
>;

class Allocator {
  using Queue = queue<Assignment>;
protected:
  int cost(const Assignment & cost);
public:
  Allocator();
  virtual ~Allocator();
};

#endif
