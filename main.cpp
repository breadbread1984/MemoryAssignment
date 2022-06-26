#include <iostream>
#include <vector>
#include "memory.hpp"

using namespace std;

int main() {
  Memory memory(8);
  vector<vector<int> > tasks{{2,1,3},{2,2,5},{2,1,1},{1,1},{2,1},{1}};
  ublas::matrix<int> assignments = memory.assign(tasks);
}
