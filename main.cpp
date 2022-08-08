#include <cstdlib>
#include <iostream>
#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <opencv2/opencv.hpp>
#include "allocator.hpp"

using namespace std;
using namespace boost::random;
using namespace cv;

int main() {
  ::Allocator allocator;
  map<int, vector<Task> > tasks{
    {0,{Task{1,2},Task{1,1},Task{1,3}}},
    {1,{Task{2,1},Task{1,2},Task{1,5}}},
    {2,{Task{2,1},Task{1,4},Task{1,5}}},
    {3,{Task{1,4},Task{1,2}}},
    {4,{Task{2,1},Task{1,2}}},
    {5,{Task{1,2}}}
  };
  Assignment solution = allocator.solve(tasks);
  int memory_size = allocator.size(solution, tasks);
  int elapse = allocator.elapse(tasks);
  // visualization
  random_device rng;
  boost::random::uniform_int_distribution<> dist(0, 255);
  Mat img = Mat::zeros(memory_size * 30, elapse * 40, CV_8UC3);
  Scalar border(128,128,128);
  for (auto & disposition: solution) {
    Task & task = tasks[disposition.time_index][disposition.task_index];
    int address = disposition.address;
    Scalar color(dist(rng), dist(rng), dist(rng));
    int x = disposition.time_index * 40;
    int y = address * 30;
    int w = task.elapse * 40;
    int h = task.size * 30;
    rectangle(img, Point(x,y), Point(x + w, y + h), color, -1);
    rectangle(img, Point(x,y), Point(x + w, y + h), border, 1);
    cout<<x<<","<<y<<","<<x+w<<","<<y+h<<endl;
  }
  namedWindow("result");
  imshow("result", img);
  waitKey();

  return EXIT_SUCCESS;
}
