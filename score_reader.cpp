/*************************
 main
 ************************/
#include <cv.h>
#include <highgui.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

//楽譜は(スキャン画像ではなく)印字されたものを想定
//四分、二分、全の音符、休符のみ
//調号は判定する
//ト音記号のみ（ヘ音記号はラベルが分離する）、4/4のみ、まずはこの二つの記号は無視
//150dpiでは誤認識大、300dpiなら十分

#include "staff.h"
#include "labeling.h"
#include "distinction.h"
#define DEBUG 1

int main(int argc, char **argv)
{
  string imagename = argv[1];//画像名
  string filename = imagename + "." + argv[2];//画像ファイル名(拡張子込み)
  Mat cimg = imread(filename);//元画像
  //imshow("original",cimg);
  imwrite("original.png",cimg);
  Mat gimg;//grayスケール
  cvtColor(cimg, gimg, CV_BGR2GRAY);
  Mat bin_img;//二値化
  threshold(gimg, bin_img, 0, 255, THRESH_BINARY|THRESH_OTSU);
  //imshow("bin_img", bin_img);
  imwrite("bin_img.png", bin_img);

  int staff_num=0;//五線譜の数
  int w_run;//黒線間隔
  int b_run;//黒線幅
  Mat position = Mat::zeros(30, 5, CV_32SC1);//五線譜位置
  int height = bin_img.rows;
  int width = bin_img.cols;  

  Mat barline_label = Mat::zeros(30, width, CV_32SC1);//小節線ラベル
  Mat barline_position = Mat::zeros(30, 30, CV_32SC1);//小節線位置

  Mat label_mat = Mat::zeros(height, width, CV_32SC1);//ラベル
  Mat label_position = Mat::zeros(3000, 4, CV_32SC1);//ラベル毎の図形の最大最小座標
  int label_num = 0;//ラベルの数
  Mat initial_label = Mat::zeros(30, 1, CV_32SC1);//各五線譜先頭のラベル

  find_staff(bin_img, width, height, &staff_num, &w_run, &b_run, position);
  delete_staff(bin_img, width, height, staff_num, w_run, b_run, position);
  barline(bin_img, width, height, staff_num, b_run, position, barline_label, barline_position);
  labeling(bin_img, width, height, staff_num, w_run, b_run, position, label_mat, label_position, &label_num, initial_label);

  if(argc == 5){//argc == 5のときは、記号認識は行わず、記号テンプレート画像作成を行う
    int making_label = atoi(argv[3]);//argv[3]のラベルがついた記号を対象とする
    int xmin = label_position.at<int>(making_label-1,0);
    int xmax = label_position.at<int>(making_label-1,1);
    int ymin = label_position.at<int>(making_label-1,2);
    int ymax = label_position.at<int>(making_label-1,3);
    Mat symbol_img(bin_img, cv::Rect(xmin, ymin, xmax-xmin+1, ymax-ymin+1));
    string symbol_file_name = "symbols/";
    symbol_file_name += argv[4];//argv[4]には記号名、jpgでは保存時に二値化が崩れるMat roi_img(src_img, cv::Rect(200, 200, 100, 100));
    symbol_file_name += "_roi.png";
    imwrite(symbol_file_name, symbol_img);
  }
  else{
    distinction(imagename, staff_num, w_run, b_run, position, label_mat, label_position, label_num, initial_label);
    
    cout << "filename:" << filename << endl;
    cout << "width:" << width << endl;
    cout << "height:" << height << endl;
    cout << "staff_num:" << staff_num << endl;
    cout << "w_run:" << w_run << endl;
    cout << "b_run:" << b_run << endl;
    cout << "label_num:" << label_num << endl;
  }

  cvWaitKey(0);
  
  return 0;
}
