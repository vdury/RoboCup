#include <iostream>
#include <cstdlib>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;


enum shapes {SQUARE, DIAMOND, DISK, LINE_V, DIAG_R, LINE_H, DIAG_L, CROSS, PLUS};

Mat create_se (int shape, int halfsize){
  Mat s(2*halfsize + 1, 2*halfsize + 1, CV_8U); 
  s = Scalar(0);
  switch(shape){
  case SQUARE:
    s = Scalar(255);
    break;

  case DIAMOND:
    for(int i = 0; i < s.rows; i++){
      for(int j = 0; j < s.cols; j++){
	if(abs(i - halfsize) + abs(j - halfsize) < halfsize)
	  s.at<uchar>(i,j) = 255;
      }
    }
    break;

  case DISK:
    for(int i = 0; i < s.rows; i++){
      for(int j = 0; j < s.cols; j++){
	if(sqrt(abs(i - halfsize)*abs(i - halfsize) + abs(j - halfsize)*abs(j - halfsize)) < halfsize)
	  s.at<uchar>(i,j) = 255;
      }
    }
    break;

  case LINE_V:
    for(int i = 0; i < s.rows; i++)
      s.at<uchar>(i, halfsize) = 255;
    break;
        
  case DIAG_R:
    for(int i = 0; i < s.rows; i++)
      s.at<uchar>(i, s.cols - i -1) = 255;
    break;

  case LINE_H:
    for(int i = 0; i < s.rows; i++)
      s.at<uchar>(halfsize, i) = 255;
    break;

  case DIAG_L:
    for(int i = 0; i < s.rows; i++)
      s.at<uchar>(i, i) = 255;
    break;

  case CROSS:
    for(int i = 0; i < s.rows; i++){
      s.at<uchar>(i, i) = 255;
      s.at<uchar>(i, s.cols - i -1) = 255;
    }
    break;

  case PLUS:
    for(int i = 0; i < s.rows; i++){
      s.at<uchar>(halfsize, i) = 255;
      s.at<uchar>(i, halfsize) = 255;      
    }
    break;
  }

  return s;
}


void drawLine(vector<Vec2f> lines, Mat ims, Scalar color, int angle, int variance) {
  cout<<"lines.size: "<<lines.size()<<endl;
  for( size_t i = 0; i < lines.size(); i++ ) {
    float rho = lines[i][0], theta = lines[i][1];
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

void drawRegressions(vector<Vec2f> lines, Mat ims, Scalar color, int angle, int variance) {
  if(!lines.size())
    return;

  float rho_avg = 0; float theta_avg_r = 0; float theta_avg_l = 0;
  int n = 0; int n_left = 0; int n_right = 0;
  float rho_avg_l = 0; float rho_avg_r = 0;
  
  for( size_t i = 0; i < lines.size(); i++ ) {
    float theta_d = lines[i][1] * 180 / M_PI;
    float rho = lines[i][0];
    if( (theta_d > angle - variance && theta_d < angle + variance) || (theta_d > 180 - angle - variance && theta_d < 180 - angle + variance) ) {
      n++;
      rho_avg += rho;
    }
  }
  rho_avg /= n;
  cout<<"rho_avg: "<<rho_avg<<endl;

  for( size_t i = 0; i < lines.size(); i++ ) {
    float theta = lines[i][1]; float rho = lines[i][0];
    float theta_d = theta * 180 / M_PI;
    if( (theta_d > angle - variance && theta_d < angle + variance) || (theta_d > 180 - angle - variance && theta_d < 180 - angle + variance)) {
      if(rho <= rho_avg) {
	theta_avg_l += theta;
	rho_avg_l += rho;
	n_left ++;
      } else {
	theta_avg_r += theta;
	rho_avg_r += rho;
	n_right ++;
      }
    }
  }

  theta_avg_r /= n_right;
  rho_avg_r /= n_right;

  theta_avg_l /= n_left;
  rho_avg_l /= n_left;


  for(int i = 0; i < 2; i++) {
    float theta = 0; float rho = 0;
    if( i == 0 ) {
      theta = theta_avg_l;
      rho = rho_avg_l;
    } else if( i == 1) {
      theta = theta_avg_r;
      rho = rho_avg_r;
    }
    cout<<"theta: "<<theta * 180 / M_PI<<" rho: "<<rho<<endl;
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


void process(const char* ims_name, const char* se_name, const unsigned char thresh){
  volatile const int max_BINARY_value = 255;
  Mat ims = imread(ims_name, CV_LOAD_IMAGE_COLOR);
  Mat ims2 = imread(ims_name, CV_LOAD_IMAGE_COLOR);

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
  while(1){
    imshow("lum", lum);
    imshow("cr", cr);
    if(waitKey(30) == 27) break;
  }
  Mat imd = ims.clone();
  //  adaptiveThreshold(cr, cr, 255.0, ADAPTIVE_THRESH_GAUSSIAN_C,  THRESH_BINARY, 17, 0);  
  threshold(cr, cr, thresh, max_BINARY_value, THRESH_BINARY_INV);
  while(1){
    imshow("thresh", cr);
    if(waitKey(30) == 27) break;
  }
  
  // //Motherfucking opening
  // erode(cr, cr, se);
  // dilate(cr, cr, se);

  // //Motherfucking closing
  // dilate(cr, cr, se);
  // erode(cr, cr, se);

  morphologyEx(cr, cr, MORPH_OPEN, se);
  morphologyEx(cr, cr, MORPH_CLOSE, se);
  
  while(1){
    imshow("opened", cr);
    if(waitKey(30) == 27) break;
  }
  vector<Vec2f> lines;
  
  HoughLines(cr, lines, 1, CV_PI/180, 100, 0, 0 );
  //drawLine(lines, ims2, Scalar(255, 0, 0), 180, 20);
  drawLine(lines, ims, Scalar(0, 0, 255), 180, 20);
  
  ims2.copyTo(imd, cr);
  
  // Mat square_se(5,5, CV_8UC1);
  // square_se += 255;
  // Mat square_se = create_se(DISK, 8);
  
  // erode(imd, imd, square_se);
  // dilate(imd, imd, square_se);

  while(1){
    imshow("hough regression", ims); 
    imshow("hough masked", imd); 
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
