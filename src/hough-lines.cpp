#include <iostream>
#include <cstdlib>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

#include "create-se.hpp"
#include <cmath>
#include <iostream>

double get_mean(vector<int> v) {
  double sum = 0.0;
  for(int i = 0; i < v.size(); ++i)
    sum += v[i];
  
  return sum/v.size();
}

double get_variance(vector<int> v) {
  double mean = get_mean(v);
  double temp = 0;
  for(int i = 0; i < v.size(); ++i)
    temp += (mean - v[i]) * (mean - v[i]);
  return temp/v.size();
}

int _find(int p, int* roots) {
  while(roots[p] != p)
    p = roots[p];
  return p;
}

int _union(int r0, int r1, int* roots) {
  if(r0 == r1) return r0;
  if(r0 == -1) return r1;
  if(r1 == -1) return r0;
  if(r0 <  r1){
    roots[r1] = r0;
    return r0;
  }else{
    roots[r0]=r1;
    return r1;  
  }
}

int _add(int p, int r, int* roots) {
  if(r==-1) 
    roots[p]=p;
  else 
    roots[p]=r;
  return roots[p];
}

void labeling(Mat ims) {

  if(!ims.data){
    cerr<<"Image not found, exit"<<endl;
    exit(EXIT_FAILURE);
  }

  //  cvtColor(ims, ims, CV_BGR2GRAY);
 
  int* roots = new int[ims.total()];
  int  rows  = ims.rows;
  int  cols  = ims.cols;
  int p      = 0;
  int r      = -1;
  uchar* ps  = ims.data;

  for(int i=0; i<rows; i++){
    for(int j=0; j<cols; j++){
      r = -1;

      if( j>0 && (*(ps-1)==(*ps)) )
	r = _union( _find(p-1, roots), r, roots);

      if( (i>0 && j>0) && (*(ps-1-cols)==(*ps)) ) 
	r = _union( _find(p-1-cols, roots), r, roots);

      if( i>0 && (*(ps-cols) == (*ps)) && *ps) 
	r = _union(_find(p-cols, roots), r, roots);

      if( (j<(cols-1) && i>0) && (*(ps+1-cols)==(*ps)) )
	r = _union(_find(p+1-cols, roots), r, roots);

      r = _add(p, r, roots);

      p++; 
      ps++; 
    }
  }

  for(p=0; p<rows*cols; p++){ 
    roots[p] = _find(p, roots);
  }
  
  int l=0;
  for(p=0; p<rows*cols; p++){
    if(roots[p]==p)
      roots[p] = l++;
    else
      roots[p] = roots[roots[p]];
  }
  cv::Mat imd(ims.rows, ims.cols, CV_8UC3);
  cv::Vec3b color[l];
  for (int i = 0; i < l; i++)
    color[i] = cv::Vec3b(rand()%256, rand()%256, rand()%256);
  for (int i = 0; i < rows; i++)
    for (int j = 0; j < cols; j++)
      imd.at<cv::Vec3b>(i, j) = color[roots[i*cols+j]];


  vector< vector<Point> > component_list(l);
  for (int i = 0; i < rows; i++)
    for (int j = 0; j < cols; j++) {
      component_list[roots[i*cols+j]].push_back(Point(i,j));
    }
  
  for(int i = 0; i < l; i++) {
    vector<int> right_border(ims.rows);
    vector<int> left_border(ims.rows);
    vector<int> vertical;
    for(vector<Point>::iterator it = component_list[i].begin(); it != component_list[i].end(); it++){
      Point p = *it;
      right_border[p.x] = max(right_border[p.x], p.y);
      left_border[p.x] = min(left_border[p.x], p.y);
      
    }
    for(int j = 0; j < ims.rows; ++j) {
      if(right_border[j] and left_border[j]) {
	vertical.push_back(right_border[j] - left_border[j]); }
    }
    double var = get_variance(vertical);
    double mean = get_mean(vertical);
    cout<<"component["<<i<<"] of color "<<color[i]<<endl
	<<"\t"<<"variance: "<<var<<endl
	<<"\t"<<"mean: "<<mean
	<<endl;
  }

  
  cout<<"labeling: "<< l << " components detected"<<endl;
  cv::imshow("colored image", imd); cv::waitKey(0);

  delete [] roots;
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
  Mat cb = ycrcb[1];
  Mat cr = ycrcb[2];


  Mat d;
  Mat th;
  imwrite("cr.png", cr);

  //  dilate(cr, d, se);
  morphologyEx(cr, d, MORPH_CLOSE, se);

  for(int i = 10; i < 130; i+=5){
    threshold(d, th, i, max_BINARY_value, THRESH_BINARY_INV);
    labeling(th);
  }
  
  while(1){
    imshow("cr", cr);
    imshow("d", d);
    imshow("th", th);
    if(waitKey(30) > 0) break;
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
