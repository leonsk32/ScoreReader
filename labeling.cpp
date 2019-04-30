#include "labeling.h"
#define DEBUG 0

//再帰処理部分(周囲8画素)
void sub_labeling(Mat& bin_img, int width, int k, int j, int begin, int end, int label_num, Mat& label_mat, Mat& label_position)
{
  if(label_mat.at<int>(k,j) != 0){//処理済みを無視
    return;
  }
  if(k<=begin || k>=end || j<=0 || j>=width-1){ //範囲外および端は無視
    return;
  }
  if(bin_img.at<unsigned char>(k,j) == 255){
    label_mat.at<int>(k,j) = -1;//走査済み白画素はラベル-1
  }
  else{
    label_mat.at<int>(k,j) = label_num;
    if(label_position.at<int>(label_num-1,0)==0 || label_position.at<int>(label_num-1,0) > j){
    label_position.at<int>(label_num-1,0) = j;
    }
    else if(label_position.at<int>(label_num-1,1) < j){
      label_position.at<int>(label_num-1,1) = j;
    }
    if(label_position.at<int>(label_num-1,2)==0 || label_position.at<int>(label_num-1,2) > k){
      label_position.at<int>(label_num-1,2) = k;
    }
    else if(label_position.at<int>(label_num-1,3) < k){
      label_position.at<int>(label_num-1,3) = k;
    }
    //周囲8画素に対して再帰処理
    for(int i=-1; i<2; i++){
      for(int l=-1; l<2; l++){
	if(i!=0 || l!=0){ 
	  sub_labeling(bin_img, width, k+i, j+l, begin, end, label_num, label_mat, label_position);
	} 
      }
    }
  }  
}

void labeling(Mat& bin_img, int width, int height, int staff_num, int w_run, int b_run, Mat& position, Mat& label_mat, Mat& label_position, int *label_num, Mat& initial_label)
{
  *label_num = 0;
  
  for(int i=0; i<staff_num; i++){
    //走査は譜表ごとに左から縦向きに、上下は第三加線まで見る
    int begin = position.at<int>(i,0) - 3*w_run - 3*b_run;
    int end = position.at<int>(i,4) + 3*w_run + 4*b_run - 1;
    
    initial_label.at<int>(i,0) = (*label_num) + 1;

    for(int j=1; j<width-1; j++){ //端は無視
      for(int k=begin+1; k<end; k++){ //端は無視
 	if(label_mat.at<int>(k,j) == 0){ //未処理の場合のみ
	  if(bin_img.at<unsigned char>(k,j) == 0){
	    (*label_num)++; //未処理な黒画素を見つけたらラベルを新しくする
	  }
	  sub_labeling(bin_img, width, k, j, begin, end, *label_num, label_mat, label_position);
	}
      }
    }
  }  

#if DEBUG
  for(int i=0; i<height; i++){
    for(int j=0; j<width; j++){
      int tmp = label_mat.at<int>(i,j);
      int flag = 0;
      for(int k=0; k<staff_num; k++){
	int disp = initial_label.at<int>(k,0);
	if(tmp == disp+2){
	  flag = 1;
	}
      }
      if(flag)
	bin_img.at<unsigned char>(i,j) = 0;
      else
	bin_img.at<unsigned char>(i,j) = 255;
    }
  }
  imshow("labeling", bin_img);
  imshow("labeling.png", bin_img);

  cout << "label_position:" << endl;
  for(int i=0; i<500; i++){
    cout << label_position.row(i);
  }
#endif

}
