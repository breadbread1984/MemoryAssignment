#ifndef ALLOCATOR_HPP
#define ALLOCATOR_HPP

#include <map>
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
  int time_index; // index of time
  int task_index; // index of task
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
  using task_id = std::tuple<int,int>; // time_index, task_index
  using remaining_tasks = vector<task_id>;
  using solution_candidate = std::tuple<Assignment, remaining_tasks>;
  using Queue = queue<solution_candidate>;
protected:
  inline const Task & get_task(const map<int, vector<Task> > & tasks, int time_index, int task_index);
  int cost(const Assignment & assignment, const map<int, vector<Task> > & tasks);
  Disposition assign(const Assignment & assignment, const map<int, vector<Task> > & tasks, task_id tid);
public:
  Allocator();
  virtual ~Allocator();
  Assignment solve(const map<int, vector<Task> > & tasks);
};

#endif
