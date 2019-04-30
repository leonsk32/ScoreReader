#include <cv.h>
#include <highgui.h>
#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <math.h>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

int label_to_staff(int, int, Mat&);
int find_black_head(int, int, Mat&, Mat&, Mat&, int, int, int *, int *, int *);
int find_half_note(int, int, Mat&, Mat&, Mat&, int, int, int *, int *, int *);
int find_whole_note(int, int, Mat&, Mat&, Mat&, int, int, int *, int *, int *);
int find_half_and_whole_rest(int, Mat&, Mat&, Mat&, int, int);
double compare_result(Mat&, int, int, string);
int find_other_symbols(int, int, Mat&, Mat&, Mat&, int, int, string, int *);
void pitch_to_pitchname(int, char*, string, int *, int, int *, int *);
string judge_key_signature(int *, int);

/***************************
  記号判別
  imagename 楽譜名...given
  staff_num 五線譜の数...given 
  w_run 線間隔...given
  b_run 黒線の太さ...given
  position.at<int>(i,j) i+1番目の五線譜の、上からj+1番目の黒線の位置(最上点)...given
  label_mat.at<int>(i,j) (i,j)のラベル、ラベルは1はじまり、未走査は0、走査済白画素は-1...given
  label_position.at<int>(i,j) ラベルi+1の画像の最大最小座標 j:0=xmin, 1=xmax, 2=ymin, 3=ymax, (x,y) = (列,行)であることに注意...given
  label_num ラベルの数...given
  initial_label.at<int>(i,0) i+1番目の五線譜の、先頭のラベル番号...given
***************************/
void distinction(string, int, int, int, Mat&, Mat&, Mat&, int, Mat&);
