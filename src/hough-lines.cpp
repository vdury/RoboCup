#include <iostream>
#include <cstdlib>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

#include "create-se.hpp"

void process(const char* ims_name, const char* se_name, const unsigned char thresh){
  volatile const int max_BINARY_value = 255;
  Mat ims = imread(ims_name, CV_LOAD_IMAGE_COLOR);
  Mat se  = imread(se_name, CV_LOAD_IMAGE_GRAYSCALE);
  if(!ims.data || !se.data) {
    cerr<<"Given file must be an image"<<endl;
    exit(EXIT_FAILURE);
  }
  //  pyrDown(ims, ims);
  
  Mat imgycrcb;
  cvtColor(ims, imgycrcb, CV_BGR2YCrCb);
  Mat ycrcb[3];
  split(imgycrcb, ycrcb);
  
  Mat lum = ycrcb[0];
  Mat cb = ycrcb[1];
  Mat cr = ycrcb[2];


  Mat d;
  Mat th;
  imwrite("cr.png", cr);

  //  dilate(cr, d, se);
  morphologyEx(cr, d, MORPH_CLOSE, se);
  d.copyTo(th);
  threshold(th, th, thresh, max_BINARY_value, THRESH_BINARY_INV);
  
  while(1){
    imshow("cr", cr);
    imshow("d", d);
    imshow("th", th);
    if(waitKey(30) == 27) break;
  }
}

void usage(const char *s){
  cerr<<"Usage: "<<s<<" ims_name se_name"<<endl;
  exit(EXIT_FAILURE);
}

int main(int argc, char **argv){
  if(argc != 4)
    usage(argv[0]);
  process(argv[1], argv[2], atoi(argv[3]));

  return EXIT_SUCCESS;
}
