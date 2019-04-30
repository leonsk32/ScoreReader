#include "staff.h"
#define DEBUG 0

void find_staff(Mat& bin_img, int width, int height, int *staff_num, int *w_run, int *b_run, Mat& position)
{
  //縦方向に走査して、ホワイトランレングスとブラックランレングスの分布をとる。  
  Mat run_voting = Mat::zeros(2,height+1,CV_32SC1);//0行目はwhite,1行目はblack
  int w_count,b_count;

  for(int i=0; i<width; i++){
    w_count = 0;
    b_count = 0;
    for(int j=0; j<height; j++){
      if(bin_img.at<unsigned char>(j,i) == 255){
	w_count++;
	if(b_count){
	  run_voting.at<int>(1,b_count) += 1;
	  b_count = 0;
	}
      }
      else{
	b_count++;
	if(w_count){
	  run_voting.at<int>(0,w_count) += 1;
	  w_count = 0;
	}
      }
    }
  }

#if DEBUG
  cout << "run_voting:" << endl << run_voting << endl;
#endif  
  
  //最頻値が線間隔および黒線の太さ
  Mat idxmat;
  sortIdx(run_voting, idxmat, CV_SORT_EVERY_ROW+CV_SORT_DESCENDING);
  *w_run = idxmat.at<int>(0,0);
  *b_run = idxmat.at<int>(1,0);
  
  //横方向に走査して、黒画素値数をとる。(> width/2)で黒線と判断
  Mat b_row = Mat::zeros(2,height,CV_32SC1);//0行目は黒画素数、1行目はラベル
  int label = 0;//ラベルは1始まり
  int pos[150] = {0};//ラベルごとの最上点
  int n=0;//ラベルごとの要素数
  for(int i=0;i<height;i++){
    for(int j=0;j<width;j++){
      if(bin_img.at<unsigned char>(i,j) == 0){
	b_row.at<int>(0,i) += 1;
      }
    }
    if(i){
      if(b_row.at<int>(0,i) > (width/2)){
	if(b_row.at<int>(1,i-1) == 0){
	  label++;
	}
	b_row.at<int>(1,i) = label;
	n++;
      }
      else if(b_row.at<int>(1,i-1) == label){
	pos[label] = i-n;
	n=0;
      }
    }
  }

#if DEBUG
  cout << "pos:" << endl;
  for(int i=0; i<150; i++){
    cout << pos[i] << ",";
  }
  cout << endl;
#endif

  //線間隔によって五線譜を推定
  *staff_num = 1;
  int line_num = 1;//上から何番目か
  int NOISE = 3;//w_run、b_runの許容誤差
  int interval = *w_run + *b_run;//理想線間隔

  for(int i=1;i<label;i++){
    int diff = pos[i+1] - pos[i];//実際の線間隔
    if(interval-NOISE < diff && diff < interval + NOISE){
      if(line_num == 4){
	position.at<int>(*staff_num-1,3) = pos[i];
	position.at<int>(*staff_num-1,4) = pos[i+1];
	line_num = 1;
	(*staff_num)++;
      }
      else{
	position.at<int>(*staff_num-1,line_num-1) = pos[i];
	line_num++;
      }
    }
  }
  *staff_num = *staff_num - 1;

#if DEBUG
  //cout << "position:" << endl << position << endl;  
  Mat check_img = bin_img.clone();
  for(int i=0;i<*staff_num;i++){
    for(int j=0;j<5;j++){
      for(int k=0;k<width;k++){
	check_img.at<int>(position.at<int>(i,j),k) = 100;
      }
    }
  }  
  imshow("find_staff",check_img);
  imwrite("find_staff.png", check_img);
#endif
}

//上下いずれかに黒画素があれば黒線は消去しない
//記号を分離してしまわないよう、上下片方に黒画素があるが故に消されない黒画素の間が狭い場合、それらの間を補完する。
void delete_staff(Mat& bin_img, int width, int height, int staff_num, int w_run, int b_run, Mat& position)
{
  int amp = 4;//何ブラックレングス分未満だったら間を補完するか
  for(int i=0; i<staff_num; i++){
    for(int j=0; j<5; j++){
      int pos = position.at<int>(i,j);
      int flag; //上下の黒画素の有無 0...無、1...上、2...下、3...両側
      int not_delete_flag = 0; //前回消さなかった時のflag 1...上、2...下、3...両側
      int not_delete = -amp * b_run;//前回上下片方に黒画素があって消さなかった時の黒画素の列
      int diff = 0;//上下片方に黒画素があるが故に消去しない黒画素間距離
      for(int k=0; k<width; k++){
	int upper = (bin_img.at<unsigned char>(pos-1,k) == 0) ? 1 : 0;
	int down = (bin_img.at<unsigned char>(pos+b_run,k) == 0) ? 1 : 0;
	flag = upper + down * 2;
	
	switch(flag){
	case 0: 
	  for(int m=0; m<b_run; m++){
	    bin_img.at<unsigned char>(pos+m,k) = 255;//上下とも白なら黒線消去
	  }
	  break;
	case 1: case 2:
	  if(not_delete_flag == flag){	    
	    diff = k - not_delete;
	    if(diff > 1 && diff < amp*b_run){//消去しない黒画素間が狭かったら間を補完 
	      for(int l=not_delete+1; l<k; l++){
		for(int n=0; n<b_run; n++){
		  bin_img.at<unsigned char>(pos+n,l) = 0;
		}
	      }
	    }
	  }
	  not_delete = k;
	case 3:
	  not_delete_flag = flag;
	default: break;
	}
      }
    }
  }
  //imshow("delete_staff", bin_img);
  imwrite("delete_staff.png", bin_img);
}

void barline(Mat& bin_img, int width, int height, int staff_num, int b_run, Mat& position, Mat& barline_label, Mat& barline_position)
{
  for(int i=0; i<staff_num; i++){
    //五線譜毎に縦向きに走査、全て黒なら小節線と判断する。
    int label = 0;//小節線のラベリング
    int begin = position.at<int>(i,0);//五線譜最上点
    int end = position.at<int>(i,4) + b_run - 1;//五線譜最下点
    for(int j=1; j<width; j++){//左端は無視
      int flag = 1;
      for(int k=begin+1; k<end; k++){//端っこ1画素は白でも許容
	if(bin_img.at<unsigned char>(k,j)){//全て黒のときだけ flag = 1
	  flag = 0; 
	}
      }
      if(flag){
	if(barline_label.at<int>(i,j-1) == 0){
	  label++;
	  barline_position.at<int>(i,label) = j;
	  barline_position.at<int>(i,0)++;
	}
	barline_label.at<int>(i,j) = label;
	for(int k=begin; k<end+1; k++){
	  bin_img.at<unsigned char>(k,j) = 255;//消去
	}
      } 
    }
  }

#if DEBUG
  cout << "barline_position:" << barline_position << endl;
#endif 

  //imshow("barline", bin_img);
  imwrite("barline.png", bin_img);
}      
