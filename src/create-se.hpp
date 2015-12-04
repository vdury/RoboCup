#include <opencv2/opencv.hpp>

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

