#ifndef ALLOCATOR_HPP
#define ALLOCATOR_HPP

#include <map>
#include <vector>
#include <tuple>
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
      member<Disposition, int, &Disposition::time_index>
    >
  >
>;

class Allocator {
  using task_id = std::tuple<int,int>; //time_index, task_index
  using remaining_tasks = vector<task_id>;
protected:
  inline const Task & get_task(const map<int, vector<Task> > & tasks, int time_index, int task_index);
  inline int temporal_intersect(int left1, int right1, int left2, int right2);
  inline int intersect(int left1, int right1, int up1, int down1, int left2, int right2, int up2, int down2);
  int cost(const Assignment & assignment, const map<int, vector<Task> > & tasks);
  Disposition assign(const Assignment & assignment, const map<int, vector<Task> > & tasks, task_id tid);
public:
  Allocator();
  virtual ~Allocator();
  Assignment solve(const map<int, vector<Task> > & tasks);
  int size(const Assignment & assignment, const map<int, vector<Task> > & tasks);
  int elapse(const map<int, vector<Task> > & tasks);
};

#endif
