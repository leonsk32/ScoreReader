#include <cv.h>
#include <highgui.h>
#include <iostream>
#include <stdio.h>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

//実際はサブルーチンをここに書く必要はないけど分かりやすさのため
void sub_labeling(Mat&, int, int, int, int, int, int, Mat&, Mat&);

/***************************
  ラベリング
  bin_img 元画像(五線消去済み)...given
  width 元画像幅...given
  height 元画像高さ...given
  staff_num 五線譜の数...given 
  w_run 線間隔...given
  b_run 黒線の太さ...given
  position.at<int>(i,j) i+1番目の五線譜の、上からj+1番目の黒線の位置(最上点)...given
  label_mat.at<int>(i,j) (i,j)のラベル、ラベルは1はじまり、未走査は0、走査済白画素は-1
  label_position.at<int>(i,j) ラベルi+1の画像の最大最小座標 j:0=xmin, 1=xmax, 2=ymin, 3=ymax, (x,y) = (列,行)であることに注意
  label_num ラベルの数
  initial_label.at<int>(i,0) i+1番目の五線譜の、先頭のラベル番号
***************************/
void labeling(Mat&, int, int, int, int, int, Mat&, Mat&, Mat&, int *, Mat&);
