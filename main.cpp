#include <cstdlib>
#include <iostream>
#include <vector>
#include <tuple>
#include "memory.hpp"

using namespace std;
namespace ublas = boost::numeric::ublas;

int main() {
  Memory memory(8);
  vector<vector<tuple<int,int> > > tasks{
    {make_tuple(1,2),make_tuple(1,1),make_tuple(1,3)},
    {make_tuple(2,1),make_tuple(1,2),make_tuple(1,5)},
    {make_tuple(2,1),make_tuple(1,4),make_tuple(1,5)},
    {make_tuple(1,4),make_tuple(1,2)},
    {make_tuple(2,1),make_tuple(1,2)},
    {make_tuple(1,2)}
  };
  Assignments assignments = memory.assign(tasks);
  return EXIT_SUCCESS;
}
