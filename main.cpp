#include <cstdlib>
#include <iostream>
#include <vector>
#include <tuple>
#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <opencv2/opencv.hpp>
#include "memory.hpp"

using namespace std;
using namespace boost::random;
using namespace cv;

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
  // visualization
  random_device rng;
  boost::random::uniform_int_distribution<> dist(0, 255);
  Mat img = Mat::zeros(8 * 30, (6 + 4) * 40, CV_8UC3);
  auto & detail1 = assignments.get();
  for (int t = 0 ; t < detail1.size() ; t++) {
    auto & detail2 = detail1[t].get();
    Scalar color(dist(rng), dist(rng), dist(rng));
    int x = t * 40;
    for (map<int, tuple<int,int> >::iterator itr = detail2.begin() ; itr != detail2.end() ; itr++) {
      int y = itr->first * 30;
      int h = get<0>(itr->second) * 30;
      int w = get<1>(itr->second) * 40;
      rectangle(img, Point(x,y), Point(x + w, y + h), color, -1);
      cout<<x<<","<<y<<","<<x+w<<","<<y+h<<endl;
    }
  }
  namedWindow("result");
  imshow("result", img);
  waitKey();
  return EXIT_SUCCESS;
}
