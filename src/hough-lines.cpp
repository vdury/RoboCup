#include <iostream>
#include <cstdlib>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;


void drawLine(vector<Vec2f> lines, Mat ims, Scalar color, int angle, int variance) {
  for( size_t i = 0; i < lines.size(); i++ ) {
    float rho = lines[i][0], theta = lines[i][1];
    cout<<"angle: "<<theta * 180 / M_PI<<endl;
    int theta_d = theta * 180 / M_PI;
    if( (theta_d > angle - variance && theta_d < angle + variance) || (theta_d > 180 - angle - variance && theta_d < 180 - angle + variance)) {
      Point pt1, pt2;
      double a = cos(theta), b = sin(theta);
      double x0 = a*rho, y0 = b*rho;
      pt1.x = cvRound(x0 + 1000*(-b));
      pt1.y = cvRound(y0 + 1000*(a));
      pt2.x = cvRound(x0 - 1000*(-b));
      pt2.y = cvRound(y0 - 1000*(a));
      line(ims, pt1, pt2, color, 3, CV_AA);
    }
  }
}

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
  while(1){
    imshow("lum", lum);
    if(waitKey(30) == 27) break;
  }
  Mat imd = ims.clone();
  
  threshold(lum, lum, thresh, max_BINARY_value, THRESH_BINARY);
  while(1){
    imshow("thresh", lum);
    if(waitKey(30) == 27) break;
  }
    
  //Motherfucking opening
  erode(lum, lum, se);
  dilate(lum, lum, se);

  dilate(lum, lum, se);
  erode(lum, lum, se);

  while(1){
    imshow("opened", lum);
    if(waitKey(30) == 27) break;
  }
  vector<Vec2f> lines;
  
  HoughLines(lum, lines, 1, CV_PI/180, 100, 0, 0 );
  drawLine(lines, ims, Scalar(0, 255, 0), 180, 35);
  
  ims.copyTo(imd, lum);
  
  Mat square_se(25,25, CV_8UC1);
  square_se += 255;

  dilate(imd, imd, square_se);
  erode(imd, imd, square_se);
  
  while(1){
    imshow("hough", imd); 
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
