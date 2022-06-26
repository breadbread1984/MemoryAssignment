#include <iostream>
#include <vector>
#include <boost/numeric/ublas/io.hpp>
#include "memory.hpp"

using namespace std;
namespace ublas = boost::numeric::ublas;

int main() {
  Memory memory(8);
  vector<vector<int> > tasks{{2,1,3},{1,1,2,5},{2,1,1},{1,1},{2,1},{1}};
  ublas::matrix<int> assignments = memory.assign(tasks);

}
