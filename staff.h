#include <cv.h>
#include <highgui.h>
#include <iostream>
#include <stdio.h>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

/***************************
  五線検出 
  bin_img 元画像(二値)...given
  width 元画像幅...given
  height 元画像高さ...given
  staff_num 五線譜の数
  w_run 線間隔
  b_run 黒線の太さ
  position.at<int>(i,j) i+1番目の五線譜の、上からj+1番目の黒線の位置(最上点)
***************************/
void find_staff(Mat&, int, int, int *, int *, int *, Mat&);


/***************************
  五線消去 
  bin_img 元画像(二値)...given
  width 元画像幅...given
  height 元画像高さ...given
  staff_num 五線譜の数...given
  w_run 線間隔...given
  b_run 黒線の太さ...given
  position.at<int>(i,j) i+1番目の五線譜の、上からj+1番目の黒線の位置(最上点)...given
***************************/
void delete_staff(Mat&, int, int, int, int, int, Mat&);


/***************************
  小節線検出＆消去 
  bin_img 元画像(二値)...given
  width 元画像幅...given
  height 元画像高さ...given
  staff_num 五線譜の数...given
  b_run 黒線の太さ...given
  position.at<int>(i,j) i+1番目の五線譜の、上からj+1番目の黒線の位置(最上点)...given
  barline_label.at<int>(i,j) i+1番目の五線譜の、j列の小節線ラベル(非小節線は0)
  barline_position.at<int>(i,j) i+1番目の五線譜の、jラベルの位置(最左点) j=0は小節線の数
***************************/
void barline(Mat&, int, int, int, int, Mat&, Mat&, Mat&);
