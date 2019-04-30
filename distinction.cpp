#include "distinction.h"
#define DEBUG 1

//ラベルからそれが何番目の五線譜かを与える staff_no = ...
int label_to_staff(int target_label, int staff_num, Mat& initial_label)
{
    for(int i=0; i<staff_num; i++){
        if(target_label < initial_label.at<int>(i,0)){
            return i;
        }
    }
    return staff_num;
}

//黒符頭を検出したら1、しなかったら0
int find_black_head(int w_run, int b_run, Mat& position, Mat& label_mat, Mat& label_position, int target_label, int staff_no, int *center_x, int *center_y, int *pitch){
    int xmin = label_position.at<int>(target_label-1,0);
    int xmax = label_position.at<int>(target_label-1,1);
    int ymin = label_position.at<int>(target_label-1,2);
    int ymax = label_position.at<int>(target_label-1,3);
    
    //黒符頭を楕円と仮定
    double lrad = w_run * 0.8;//黒符頭長半径(実測0.8)
    double srad = w_run * 0.5;//黒符頭短半径(実測0.5)
    double area = lrad*srad*M_PI;//黒符頭面積
    double theta = -0.178*M_PI;//黒符頭傾き(実測)
    int b_max = 0;//黒画素数の最大値
    
#if DEBUG
    //cout << "area:" << area << " ";
#endif
    
    for(int i=-3; i<8; i++){
        int pos;//黒線最上点
        if(i>=0 && i<=4){
            pos = position.at<int>(staff_no-1,i);
        }
        else if(i<0){//上加線
            pos = position.at<int>(staff_no-1,0) + i * (w_run + b_run);
        }
        else{//下加線
            pos = position.at<int>(staff_no-1,4) + (i-4) * (w_run + b_run);
        }
        for(int j=0; j<2; j++){
            int cent_y = 0;//黒符頭の中心y座標
            if(j == 0){
                cent_y = pos + b_run / 2 - 1;//黒線の中央
            }
            else{
                cent_y = pos + b_run + w_run / 2 - 1;//黒線間の中央
            }
            if(cent_y >= ymin && cent_y <= ymax){//cent_yが範囲内のときのみ
                for(int cent_x=xmin; cent_x < xmax+1; cent_x++){//cent_xは黒符頭の中心x座標
                    int b_count = 0;//楕円内にある黒画素のカウント
                    for(int k=xmin; k<xmax+1; k++){
                        for(int l=ymin; l<ymax+1; l++){
                            //回転変換
                            double rev_x = (k-cent_x)*cos(theta) + (l-cent_y)*sin(theta);
                            double rev_y = (l-cent_y)*cos(theta) - (k-cent_x)*sin(theta);
                            double dist = (rev_x*rev_x)/(lrad*lrad) + (rev_y*rev_y)/(srad*srad);
                            if(dist <= 1){//楕円内
                                if(label_mat.at<int>(l,k) == target_label){
                                    b_count++;
                                }
                            }
                        }
                    }
                    if(b_count > b_max){
                        b_max = b_count;
                        *center_x = cent_x;
                        *center_y = cent_y;
                        *pitch = 2*i + j;
                    }
                }
            }
        }
    }
    
#if DEBUG
    //cout << "black_head_para:" << b_max << " ";
#endif
    
    if(b_max > area * 0.90){//面積の90%以上なら黒符頭ありと判断
        return 1;
    }
    else{
        return 0;
    }
}

//二分音符を検出したら1、しなかったら0
int find_half_note(int w_run, int b_run, Mat& position, Mat& label_mat, Mat& label_position, int target_label, int staff_no, int *center_x, int *center_y, int *pitch){
    int xmin = label_position.at<int>(target_label-1,0);
    int xmax = label_position.at<int>(target_label-1,1);
    int ymin = label_position.at<int>(target_label-1,2);
    int ymax = label_position.at<int>(target_label-1,3);
    
    //白符頭を楕円と仮定
    //パラメーターについては、ex_lradとin_lradを近く(同じ)にすることが大事なよう(端っこは二値化でずれやすいので)
    double ex_lrad = w_run * 0.8;//黒部分外長半径(実測0.8)
    double ex_srad = w_run * 0.5;//黒部分外短半径(実測0.5)
    double in_lrad = w_run * 0.8;//黒部分内長半径(実測0.7) 少し大きめに
    double in_srad = w_run * 0.3;//黒部分内短半径(実測0.22) 少し大きめに
    double w_lrad = w_run * 0.68;//白部分長半径(実測0.7) 少し小さめに
    double w_srad = w_run * 0.2;//白部分半径(実測0.22) 少し小さめに
    double b_area = (ex_lrad*ex_srad - in_lrad*in_srad)*M_PI;//黒部分面積
    double w_area = w_lrad*w_srad*M_PI;//白部分面積
    double theta = -0.178*M_PI;//白符頭傾き(実測)
    int b_max = 0;//黒部分(楕円環)の黒画素数の最大値
    
#if DEBUG
    //cout << "area:[" << b_area << "," << w_area << "] ";
#endif
    
    for(int i=-3; i<8; i++){
        int pos;//黒線最上点
        if(i>=0 && i<=4){
            pos = position.at<int>(staff_no-1,i);
        }
        else if(i<0){//上加線
            pos = position.at<int>(staff_no-1,0) + i * (w_run + b_run);
        }
        else{//下加線
            pos = position.at<int>(staff_no-1,4) + (i-4) * (w_run + b_run);
        }
        for(int j=0; j<2; j++){
            int cent_y = 0;//白符頭の中心y座標
            if(j == 0){
                cent_y = pos + b_run / 2 - 1;//黒線の中央
            }
            else{
                cent_y = pos + b_run + w_run / 2 - 1;//黒線間の中央
            }
            if(cent_y >= ymin && cent_y <= ymax){//cent_yが範囲内のときのみ
                for(int cent_x=xmin; cent_x < xmax+1; cent_x++){//cent_xは白符頭の中心x座標
                    int b_count = 0;//黒部分の黒画素数
                    int w_count = 0;//白部分の白画素数
                    for(int k=xmin; k<xmax+1; k++){
                        for(int l=ymin; l<ymax+1; l++){
                            //回転変換
                            double rev_x = (k-cent_x)*cos(theta) + (l-cent_y)*sin(theta);
                            double rev_y = (l-cent_y)*cos(theta) - (k-cent_x)*sin(theta);
                            double ex_dist = (rev_x*rev_x)/(ex_lrad*ex_lrad) + (rev_y*rev_y)/(ex_srad*ex_srad);
                            double in_dist = (rev_x*rev_x)/(in_lrad*in_lrad) + (rev_y*rev_y)/(in_srad*in_srad);
                            double w_dist = (rev_x*rev_x)/(w_lrad*w_lrad) + (rev_y*rev_y)/(w_srad*w_srad);
                            if(ex_dist <= 1 && in_dist >= 1){//黒部分
                                if(label_mat.at<int>(l,k) == target_label){
                                    b_count++;
                                }
                            }
                            if(w_dist <= 1){//白部分
                                if(label_mat.at<int>(l,k) == -1){
                                    w_count++;
                                }
                            }
                        }
                    }
                    if(b_count > b_max && w_count > w_area * 0.5){//白部分が50%以上なら更新、基準が高すぎると加線が貫く二分音符は厳しい
                        b_max = b_count;
                        *center_x = cent_x;
                        *center_y = cent_y;
                        *pitch = 2*i + j;
                    }
                }
            }
        }
    }
    
#if DEBUG
    //cout << "half_note_para:" << b_max << " ";
#endif
    
    if(b_max > b_area * 0.80){//黒部分が80%以上なら二分音符ありと判断、基準は高すぎると低画素では無理
        return 1;
    }
    else{
        return 0;
    }
}

//全音符を検出したら1、しなかったら0
int find_whole_note(int w_run, int b_run, Mat& position, Mat& label_mat, Mat& label_position, int target_label, int staff_no, int *center_x, int *center_y, int *pitch){
    int xmin = label_position.at<int>(target_label-1,0);
    int xmax = label_position.at<int>(target_label-1,1);
    int ymin = label_position.at<int>(target_label-1,2);
    int ymax = label_position.at<int>(target_label-1,3);
    
    //白符頭を楕円と仮定
    double ex_lrad = w_run * 1.1;//黒部分外長半径(実測1.1)
    double ex_srad = w_run * 0.6;//黒部分外短半径(実測0.6)
    double in_lrad = w_run * 0.55;//黒部分内長半径(実測0.55)
    double in_srad = w_run * 0.41;//黒部分内短半径(実測0.41)
    double w_lrad = w_run * 0.5;//白部分長半径(実測0.55) 少し小さめに
    double w_srad = w_run * 0.35;//白部分半径(実測0.41) 少し小さめに
    double b_area = (ex_lrad*ex_srad - in_lrad*in_srad)*M_PI;//黒部分面積
    double w_area = w_lrad*w_srad*M_PI;//白部分面積
    double theta = 0.302*M_PI;//白部分傾き(実測)
    int b_max = 0;//黒部分の黒画素数の最大値
    
#if DEBUG
    //cout << "area:[" << b_area << "," << w_area << "] ";
#endif
    
    for(int i=-3; i<8; i++){
        int pos;//黒線最上点
        if(i>=0 && i<=4){
            pos = position.at<int>(staff_no-1,i);
        }
        else if(i<0){//上加線
            pos = position.at<int>(staff_no-1,0) + i * (w_run + b_run);
        }
        else{//下加線
            pos = position.at<int>(staff_no-1,4) + (i-4) * (w_run + b_run);
        }
        for(int j=0; j<2; j++){
            int cent_y = 0;//白符頭の中心y座標
            if(j == 0){
                cent_y = pos + b_run / 2 - 1;//黒線の中央
            }
            else{
                cent_y = pos + b_run + w_run / 2 - 1;//黒線間の中央
            }
            if(cent_y >= ymin && cent_y <= ymax){//cent_yが範囲内のときのみ
                for(int cent_x=xmin; cent_x < xmax+1; cent_x++){//cent_xは白符頭の中心x座標
                    int b_count = 0;//黒部分の黒画素数
                    int w_count = 0;//白部分の白画素数
                    for(int k=xmin; k<xmax+1; k++){
                        for(int l=ymin; l<ymax+1; l++){
                            double ex_dist = ((k-cent_x)*(k-cent_x))/(ex_lrad*ex_lrad) + ((l-cent_y)*(l-cent_y))/(ex_srad*ex_srad);
                            //回転変換
                            double rev_x = (k-cent_x)*cos(theta) + (l-cent_y)*sin(theta);
                            double rev_y = (l-cent_y)*cos(theta) - (k-cent_x)*sin(theta);
                            double in_dist = (rev_x*rev_x)/(in_lrad*in_lrad) + (rev_y*rev_y)/(in_srad*in_srad);
                            double w_dist = (rev_x*rev_x)/(w_lrad*w_lrad) + (rev_y*rev_y)/(w_srad*w_srad);
                            if(ex_dist <= 1 && in_dist >= 1){//黒部分
                                if(label_mat.at<int>(l,k) == target_label){
                                    b_count++;
                                }
                            }
                            if(w_dist <= 1){//白部分
                                if(label_mat.at<int>(l,k) == -1){
                                    w_count++;
                                }
                            }
                        }
                    }
                    if(b_count > b_max && w_count > w_area * 0.5){//白部分が50%以上なら更新、基準が高すぎると加線が貫く全音符は厳しい
                        b_max = b_count;
                        *center_x = cent_x;
                        *center_y = cent_y;
                        *pitch = 2*i + j;
                    }
                }
            }
        }
    }
    
#if DEBUG
    //cout << "whole_note_para:" << b_max << " ";
#endif
    
    if(b_max > b_area * 0.80){//黒部分が80%以上なら全音符ありと判断、基準は高すぎると低画素では無理
        return 1;
    }
    else{
        return 0;
    }
}

//二分休符なら1,全休符なら2,それ以外なら0を返す
int find_half_and_whole_rest(int w_run, Mat& position, Mat& label_mat, Mat& label_position, int target_label, int staff_no)
{
    int xmin = label_position.at<int>(target_label-1,0);
    int xmax = label_position.at<int>(target_label-1,1);
    int ymin = label_position.at<int>(target_label-1,2);
    int ymax = label_position.at<int>(target_label-1,3);
    
    if(xmax-xmin < w_run || ymax-ymin < (w_run / 2)){//小さすぎたら雑音として排除
        return 0;
    }
    
    //ラベルの最大最小値で囲まれる四角形が全て黒なら休符
    for(int i=ymin+1; i<ymax; i++){//端は見ない
        for(int j=xmin+1; j<xmax; j++){//端は見ない
            if(label_mat.at<int>(i,j) != target_label){
                return 0;//1画素でも違ったらout
            }
        }
    }
    
    if(ymax > position.at<int>(staff_no-1, 2)){//下付きだったら二分休符
        return 1;
    }
    else{
        return 2;
    }
}

//相違度を返す
double compare_result(Mat& roi_img, int roi_height, int roi_width, string symbol_name)
{
    string filename = "symbols/";
    filename += symbol_name + "_roi.png";
    Mat symbol_roi = imread(filename,CV_LOAD_IMAGE_GRAYSCALE);//テンプレート記号画像
    //threshold(symbol_roi, symbol_roi, 0, 255, THRESH_BINARY|THRESH_OTSU);
    //テンプレート記号画像がjpgの場合、保存時に二値化が崩れるので再度二値化の必要がある
    
    double height_amp = symbol_roi.rows / (double)roi_height;//縮尺(縦)
    double width_amp = symbol_roi.cols / (double)roi_width;//縮尺(横)
    Mat resized_roi;
    resize(roi_img, resized_roi, Size(), width_amp, height_amp);
    threshold(resized_roi, resized_roi, 0, 255, THRESH_BINARY|THRESH_OTSU);//resize処理をすると二値化が崩れるので、再度二値化の必要がある
    
    int diff=0;
    for(int i=0; i<symbol_roi.rows; i++){
        for(int j=0; j<symbol_roi.cols; j++){
            if(resized_roi.at<unsigned char>(i,j) != symbol_roi.at<unsigned char>(i,j)){
                diff++;//異なれば+1
            }
        }
    }
    double result = diff / (double)(symbol_roi.rows * symbol_roi.cols);//正規化
    
#if DEBUG
    //cout << symbol_name <<  "_compare_result = " << result << endl;
#endif
    
    return (result);
}

//その他の記号
int find_other_symbols(int w_run, int b_run, Mat& position, Mat& label_mat, Mat& label_position, int target_label, int staff_no, string pre_symbol, int *key_sig, int *key_type)
{
    int xmin = label_position.at<int>(target_label-1,0);
    int xmax = label_position.at<int>(target_label-1,1);
    int ymin = label_position.at<int>(target_label-1,2);
    int ymax = label_position.at<int>(target_label-1,3);
    
    //拡大縮小で単純なマッチング
    int roi_height = ymax-ymin+1;
    int roi_width = xmax-xmin+1;
    Mat roi_img = Mat::zeros(roi_height, roi_width, CV_8UC1);//グレースケール(CV_32SC1ではresize時にコアダンプする)、label_matを用いてbin_imgを使わないのはROI内に別ラベルの黒画素が入っていたら困るから
    for(int i=ymin; i<ymax+1; i++){
        for(int j=xmin; j<xmax+1; j++){
            if(label_mat.at<int>(i,j) != target_label){
                roi_img.at<unsigned char>(i-ymin,j-xmin) = 255;
            }
        }
    }
    
    string symbol_name;
    
    //四分休符の判定
    if(ymin < position.at<int>(staff_no-1, 1) && ymax > position.at<int>(staff_no-1, 3)){//大体のy方向位置を限定
        if(roi_width > w_run * 0.6){//x方向が狭すぎるものは除外
            symbol_name = "quarter_rest";
            if(compare_result(roi_img, roi_height, roi_width, symbol_name) < 0.2){
                return 1;
            }
        }
    }
    
    //ト音記号の判定
    if(ymin < position.at<int>(staff_no-1, 0) && ymax > position.at<int>(staff_no-1, 4)){//五線譜をはみ出していなければ除外
        if(roi_width > w_run){//x方向が狭すぎるものは除外
            symbol_name = "G_clef";
            if(compare_result(roi_img, roi_height, roi_width, symbol_name) < 0.2){
                return 2;
            }
        }
    }
    
    //4/4拍子記号の判定
    if(ymin < position.at<int>(staff_no-1, 1) && ymax > position.at<int>(staff_no-1, 3)){//大体のy方向位置を限定
        if(roi_width > w_run){//x方向が狭すぎるものは除外
            symbol_name = "44_time_signature";
            if(compare_result(roi_img, roi_height, roi_width, symbol_name) < 0.2){
                return 3;
            }
        }
    }
    
    //臨時記号の判定
    if(roi_width > w_run/2 && roi_height > w_run){//小さすぎるものは除外
        symbol_name = "sharp";
        if(compare_result(roi_img, roi_height, roi_width, symbol_name) < 0.2){
            if(pre_symbol != "g" && pre_symbol != "s"){
                return 4;//臨時記号としてのシャープ
            }
            else{//調号を示すものだった場合
                *key_type = 1;
                int sharp_pos = (ymax + ymin)/2;
                int diff;
                int diff_min = 10 * w_run;
                int pitch;
                for(int i=-1; i<3; i++){
                    int pos;//黒線最上点
                    if(i>=0){
                        pos = position.at<int>(staff_no-1,i);
                    }
                    else{//上加線
                        pos = position.at<int>(staff_no-1,0) + i * (w_run + b_run);
                    }
                    for(int j=0; j<2; j++){
                        int cent_y = 0;//中心y座標
                        if(j == 0){
                            cent_y = pos + b_run / 2 - 1;//黒線の中央
                        }
                        else{
                            cent_y = pos + b_run + w_run / 2 - 1;//黒線間の中央
                        }
                        
                        diff = (cent_y - sharp_pos);
                        if(diff_min > diff){
                            diff = diff_min;
                            pitch = 2*i + j;
                        }
                    }
                }
                
                for(int j=0; j<7; j++){
                    if(key_sig[j] == 100){
                        key_sig[j] = pitch;
                        break;
                    }
                    else if(key_sig[j] == pitch){
                        break;
                    }
                }
                return 7;
            }
        }
        
        symbol_name = "flat";
        if(compare_result(roi_img, roi_height, roi_width, symbol_name) < 0.2){
            if(pre_symbol != "g" && pre_symbol != "f"){
                return 5;//臨時記号としてのフラット
            }
            else{//調号を示すものだった場合
                *key_type = -1;
                int flat_pos = ymax;
                int diff;
                int diff_min = 10 * w_run;
                int pitch;
                for(int i=-1; i<3; i++){
                    int pos;//黒線最上点
                    if(i>=0){
                        pos = position.at<int>(staff_no-1,i);
                    }
                    else{//上加線
                        pos = position.at<int>(staff_no-1,0) + i * (w_run + b_run);
                    }
                    for(int j=0; j<2; j++){
                        int cent_y = 0;//中心y座標
                        if(j == 0){
                            cent_y = pos + b_run / 2 - 1;//黒線の中央
                        }
                        else{
                            cent_y = pos + b_run + w_run / 2 - 1;//黒線間の中央
                        }
                        
                        diff = (cent_y - flat_pos);
                        if(diff_min > diff){
                            diff = diff_min;
                            pitch = 2*i + j - 1;
                        }
                    }
                }
                
                for(int j=0; j<7; j++){
                    if(key_sig[j] == 100){
                        key_sig[j] = pitch;
                        break;
                    }
                    else if(key_sig[j] == pitch){
                        break;
                    }
                }
                return 8;
            }
        }
        symbol_name = "natural";
        if(compare_result(roi_img, roi_height, roi_width, symbol_name) < 0.2){
            return 6;
        }
    }
    return 0;
    
    /*HuMomentを用いる方法。拡大縮小、回転、反転に対して不変であるHuMomentを用いて形状を比較...全然ダメ
     Mat symbol_roi = imread("symbols/<name of symbol>_roi.png",CV_LOAD_IMAGE_GRAYSCALE);
     IplImage symbol_roi_ipl = symbol_roi;//Ipl型に変換
     IplImage roi_img_ipl = roi_img;//Ipl型に変換
     double result = cvMatchShapes(&roi_img_ipl, &symbol_roi_ipl, CV_CONTOURS_MATCH_I1, 1);//Mat型では駄目なようなのでIplに変換してから代入
     cout << "result = " << result << endl;
     */
}

//pitchから音階名を返す、ハ長調
void pitch_to_pitchname(int pitch, char* pitchname, char* italy_pitch, string pre_symbol, int *key_sig, int key_type, int *acc_sig, int *acc_type)
{
    int key=0;//pitchからのあげさげ
    for(int i=0; i<7; i++){
        if(key_sig[i] != 100){
            for(int j=-21; j<28; j+=7){
                if(key_sig[i] + j == pitch){//調号にあれば
                    key = key_type;
                }
            }
        }
    }
    
    for(int i=0; i<7; i++){
        if(acc_sig[i] != 100){
            for(int j=-21; j<28; j+=7){
                if(acc_sig[i] + j == pitch){//小節内の臨時記号にあれば
                    key = acc_type[i];
                }
            }
        }
    }
    
    if(pre_symbol == "s"){//シャープ
        key = 1;
        for(int i=0; i<7; i++){//acc_sigの更新
            if(acc_sig[i] == 100){
                acc_sig[i] = pitch;
                acc_type[i] = 1;
                break;
            }
            else if(acc_sig[i] == pitch){
                break;
            }
        }
    }
    else if(pre_symbol == "f"){//フラット
        key = -1;
        for(int i=0; i<7; i++){//acc_sigの更新
            if(acc_sig[i] == 100){
                acc_sig[i] = pitch;
                acc_type[i] = -1;
                break;
            }
            else if(acc_sig[i] == pitch){
                break;
            }
        }
    }
    else if(pre_symbol == "n"){//ナチュラル
        key = 0;
        for(int i=0; i<7; i++){//acc_sigの修正
            for(int j=-21; j<28; j+=7){
                if(acc_sig[i] + j == pitch){
                    acc_sig[i] = 100;
                    acc_type[i] = 0;
                }
            }
        }
    }
    
    int tmp = -pitch * 3 + key;
    switch(tmp){
        case 19: sprintf(pitchname, "F6"); sprintf(italy_pitch, " Fa_6"); break;
        case 18: sprintf(pitchname, "E6"); sprintf(italy_pitch, " Mi_6"); break;
        case 17: case 16: sprintf(pitchname, "DS6"); sprintf(italy_pitch, "Re#_6"); break;
        case 15: sprintf(pitchname, "D6"); sprintf(italy_pitch, " Re_6"); break;
        case 14: case 13: sprintf(pitchname, "CS6"); sprintf(italy_pitch, "Do#_6"); break;
        case 12: case 10: sprintf(pitchname, "C6"); sprintf(italy_pitch, " Do_6"); break;
        case 11: case 9: sprintf(pitchname, "H5"); sprintf(italy_pitch, " Si_5"); break;
        case 8: case 7: sprintf(pitchname, "AS5"); sprintf(italy_pitch, "La#_5"); break;
        case 6: sprintf(pitchname, "A5"); sprintf(italy_pitch, " La_5"); break;
        case 5: case 4: sprintf(pitchname, "GS5"); sprintf(italy_pitch, "Sol#_5"); break;
        case 3: sprintf(pitchname, "G5"); sprintf(italy_pitch, " Sol_5"); break;
        case 2: case 1: sprintf(pitchname, "FS5"); sprintf(italy_pitch, "Fa#_5"); break;
        case 0: case -2: sprintf(pitchname, "F5"); sprintf(italy_pitch, " Fa_5"); break;
        case -1: case -3: sprintf(pitchname, "E5"); sprintf(italy_pitch, " Mi_5"); break;
        case -4: case -5: sprintf(pitchname, "DS5"); sprintf(italy_pitch, "Re#_5"); break;
        case -6: sprintf(pitchname, "D5"); sprintf(italy_pitch, " Re_5"); break;
        case -7: case -8: sprintf(pitchname, "CS5"); sprintf(italy_pitch, "Do#_5"); break;
        case -9: case -11: sprintf(pitchname, "C5"); sprintf(italy_pitch, " Do_5"); break;
        case -10: case -12: sprintf(pitchname, "H4"); sprintf(italy_pitch, " Si_4"); break;
        case -13: case -14: sprintf(pitchname, "AS4"); sprintf(italy_pitch, "La#_4"); break;
        case -15: sprintf(pitchname, "A4"); sprintf(italy_pitch, " La_4"); break;
        case -16: case -17: sprintf(pitchname, "GS4"); sprintf(italy_pitch, "Sol#_4"); break;
        case -18: sprintf(pitchname, "G4"); sprintf(italy_pitch, " Sol_4"); break;
        case -19: case -20: sprintf(pitchname, "FS4"); sprintf(italy_pitch, "Fa#_4"); break;
        case -21: case -23: sprintf(pitchname, "F4"); sprintf(italy_pitch, " Fa_4"); break;
        case -22: case -24: sprintf(pitchname, "E4"); sprintf(italy_pitch, " Mi_4"); break;
        case -25: case -26: sprintf(pitchname, "DS4"); sprintf(italy_pitch, " Re#_4"); break;
        case -27: sprintf(pitchname, "D4"); sprintf(italy_pitch, " Re_4"); break;
        case -28: case -29: sprintf(pitchname, "CS4"); sprintf(italy_pitch, "Do#_4"); break;
        case -30: case -32: sprintf(pitchname, "C4"); sprintf(italy_pitch, " Do_4"); break;
        case -31: case -33: sprintf(pitchname, "H3"); sprintf(italy_pitch, " Si_3"); break;
        case -34: case -35: sprintf(pitchname, "AS3"); sprintf(italy_pitch, "La#_3"); break;
        case -36: sprintf(pitchname, "A3"); sprintf(italy_pitch, " La_3"); break;
        case -37: case -38: sprintf(pitchname, "GS3"); sprintf(italy_pitch, "Sol#_3"); break;
        case -39: sprintf(pitchname, "G3"); sprintf(italy_pitch, " Sol_3"); break;
        case -40: case -41: sprintf(pitchname, "FS3"); sprintf(italy_pitch, "Fa#_3"); break;
        case -42: case -44: sprintf(pitchname, "F3"); sprintf(italy_pitch, " Fa_3"); break;
        case -43: sprintf(pitchname, "E3"); sprintf(italy_pitch, " Mi_3"); break;
        default: sprintf(pitchname, "error"); sprintf(italy_pitch, "error"); break;
    }
}

//key_sigから調を判定
string judge_key_signature(int *key_sig, int key_type)
{
    int count=0;//調号数
    if(key_type == 0){
        return("C dur ハ長調/ a moll イ短調");
    }
    if(key_type == 1){//シャープ系
        for(int i=0; i<7; i++){
            if(key_sig[i]!=100){
                count++;
            }
        }
        switch(count){
            case 1: return("G dur ト長調/ e moll ホ短調");
            case 2: return("D dur ニ長調/ h moll ロ短調");
            case 3: return("A dur イ長調/ fis moll 嬰ヘ短調");
            case 4: return("E dur ホ長調/ cis moll 嬰ハ短調");
            case 5: return("H dur ロ長調/ gis moll 嬰ト短調");
            case 6: return("Fis dur 嬰ヘ長調/ dis moll 嬰ニ短調");
            case 7: return("Cis dur 嬰ハ長調/ ais moll 嬰イ短調");
            default: return("error");
        }
    }
    
    if(key_type == -1){//フラット系
        for(int i=0; i<7; i++){
            if(key_sig[i]!=100){
                count++;
            }
        }
        switch(count){
            case 1: return("F dur ヘ長調/ d moll ニ短調");
            case 2: return("B dur 変ロ長調/ g moll ト短調");
            case 3: return("Es dur 変ホ長調/ c moll ハ短調");
            case 4: return("As dur 変イ長調/ f moll ヘ短調");
            case 5: return("Des dur 変ニ長調/ b moll 変ロ短調");
            case 6: return("Ges dur 変ト長調/ es moll 変ホ短調");
            case 7: return("Ces dur 変ハ長調/ as moll 変イ短調");
            default: return("error");
        }
    }
}

void distinction(string imagename, int staff_num, int w_run, int b_run, Mat& position, Mat& label_mat, Mat& label_position, int label_num, Mat& initial_label)
{
    string dataname = imagename + ".dat";//楽譜ファイル
    const char *dataname_char = dataname.c_str();
    ofstream lisp_ofs("7dof/lisp_score_data.dat", ios::out);//lisp読み込み用
    ofstream arduino_ofs("sketchbook/mechanical_pianist/score.h", ios::out);//arduino読み込み用
    arduino_ofs << "//周波数一覧\n#define NOTE_H0  31\n#define NOTE_C1  33\n#define NOTE_CS1 35\n#define NOTE_D1  37\n#define NOTE_DS1 39\n#define NOTE_E1  41\n#define NOTE_F1  44\n#define NOTE_FS1 46\n#define NOTE_G1  49\n#define NOTE_GS1 52\n#define NOTE_A1  55\n#define NOTE_AS1 58\n#define NOTE_H1  62\n#define NOTE_C2  65\n#define NOTE_CS2 69\n#define NOTE_D2  73\n#define NOTE_DS2 78\n#define NOTE_E2  82\n#define NOTE_F2  87\n#define NOTE_FS2 93\n#define NOTE_G2  98\n#define NOTE_GS2 104\n#define NOTE_A2  110\n#define NOTE_AS2 117\n#define NOTE_H2  123\n#define NOTE_C3  131\n#define NOTE_CS3 139\n#define NOTE_D3  147\n#define NOTE_DS3 156\n#define NOTE_E3  165\n#define NOTE_F3  175\n#define NOTE_FS3 185\n#define NOTE_G3  196\n#define NOTE_GS3 208\n#define NOTE_A3  220\n#define NOTE_AS3 233\n#define NOTE_H3  247\n#define NOTE_C4  262\n#define NOTE_CS4 277\n#define NOTE_D4  294\n#define NOTE_DS4 311\n#define NOTE_E4  330\n#define NOTE_F4  349\n#define NOTE_FS4 370\n#define NOTE_G4  392\n#define NOTE_GS4 415\n#define NOTE_A4  440\n#define NOTE_AS4 466\n#define NOTE_H4  494\n#define NOTE_C5  523\n#define NOTE_CS5 554\n#define NOTE_D5  587\n#define NOTE_DS5 622\n#define NOTE_E5  659\n#define NOTE_F5  698\n#define NOTE_FS5 740\n#define NOTE_G5  784\n#define NOTE_GS5 831\n#define NOTE_A5  880\n#define NOTE_AS5 932\n#define NOTE_H5  988\n#define NOTE_C6  1047\n#define NOTE_CS6 1109\n#define NOTE_D6  1175\n#define NOTE_DS6 1245\n#define NOTE_E6  1319\n#define NOTE_F6  1397\n#define NOTE_FS6 1480\n#define NOTE_G6  1568\n#define NOTE_GS6 1661\n#define NOTE_A6  1760\n#define NOTE_AS6 1865\n#define NOTE_H6  1976\n#define NOTE_C7  2093\n#define NOTE_CS7 2217\n#define NOTE_D7  2349\n#define NOTE_DS7 2489\n#define NOTE_E7  2637\n#define NOTE_F7  2794\n#define NOTE_FS7 2960\n#define NOTE_G7  3136\n#define NOTE_GS7 3322\n#define NOTE_A7  3520\n#define NOTE_AS7 3729\n#define NOTE_H7  3951\n#define NOTE_C8  4186\n#define NOTE_CS8 4435\n#define NOTE_D8  4699\n#define NOTE_DS8 4978\n#define NOTE_REST 0\n#include <avr/pgmspace.h>\n\nPROGMEM int score_data[]={" << endl;
    
    cout << "\n***********************************\nrecognition result:" << endl;
    
    string pre_symbol; //直前の記号保持用
    int key_sig[7] = {100,100,100,100,100,100,100}; //調号、初期値100で音の高さ表記はfind_black_headに同じ
    int acc_sig[7] = {100,100,100,100,100,100,100}; //臨時記号、小節内でのみ保持される
    int acc_type[7] = {0}; //臨時記号がシャープなら1,フラットなら-1、番号はacc_sigに1対1対応
    int passage_counter = 0;//小節線内の拍数をカウント
    int key_type = 0; //調号がシャープ系なら1,フラット系なら-1,それ以外なら0
    
    for(int target_label=1; target_label<label_num+1; target_label++){
        int center_x, center_y;//符頭中心座標(実際は今はcenter_xは必要ない)
        int pitch;//pitchには何番目なのか(第一線を0、そこから一音毎に±、黒鍵は今は考えない)を格納
        char pitchname[10];//音階名
        char italy_pitch[10];//イタリア式表記
        int staff_no = label_to_staff(target_label, staff_num, initial_label);//何番目の譜表か
        int black_head_flag = find_black_head(w_run, b_run, position, label_mat, label_position, target_label, staff_no,  &center_x, &center_y, &pitch);
        
        if(black_head_flag){
            if(pre_symbol == "k" || pre_symbol == "g"){
                cout << "{" << judge_key_signature(key_sig, key_type) << "}" << endl;
            }
            pitch_to_pitchname(pitch, pitchname, italy_pitch, pre_symbol, key_sig, key_type, acc_sig, acc_type);
            lisp_ofs << "1" << endl << pitchname << endl;
            cout << "1/4 " << pitchname << " " << italy_pitch << endl;
            arduino_ofs << "NOTE_" << pitchname << ", " << "2, ";
            pre_symbol = "b";
            passage_counter += 1;
        }
        else{
            int half_note_flag = find_half_note(w_run, b_run, position, label_mat, label_position, target_label, staff_no,  &center_x, &center_y, &pitch);
            if(half_note_flag){
                if(pre_symbol == "k" || pre_symbol == "g"){
                    cout << "{" << judge_key_signature(key_sig, key_type) << "}" << endl;
                }
                pitch_to_pitchname(pitch, pitchname, italy_pitch, pre_symbol, key_sig, key_type, acc_sig, acc_type);
                lisp_ofs << "2" << endl << pitchname << endl;
                cout << "1/2 " << pitchname << " " << italy_pitch << endl;
                arduino_ofs << "NOTE_" << pitchname << ", " << "4, ";
                pre_symbol = "h";
                passage_counter += 2;
            }
            else{
                int whole_note_flag = find_whole_note(w_run, b_run, position, label_mat, label_position, target_label, staff_no, &center_x, &center_y, &pitch);
                if(whole_note_flag){
                    if(pre_symbol == "k" || pre_symbol == "g"){
                        cout << "{" << judge_key_signature(key_sig, key_type) << "}" << endl;
                    }
                    pitch_to_pitchname(pitch, pitchname, italy_pitch, pre_symbol, key_sig, key_type, acc_sig, acc_type);
                    lisp_ofs << "4" << endl << pitchname << endl;
                    cout << "1/1 " << pitchname << " " << italy_pitch << endl;
                    arduino_ofs << "NOTE_" <<pitchname << ", " << "8, ";
                    pre_symbol = "w";
                    passage_counter += 4;
                }
                else{
                    int half_and_whole_rest_flag = find_half_and_whole_rest(w_run, position, label_mat, label_position, target_label, staff_no);
                    if(half_and_whole_rest_flag){
                        if(pre_symbol == "k" || pre_symbol == "g"){
                            cout << "{" << judge_key_signature(key_sig, key_type) << "}" << endl;
                        }
                        if(half_and_whole_rest_flag == 1){
                            lisp_ofs << "-2" << endl;
                            cout << "1/2 rest" << endl;
                            arduino_ofs << "NOTE_REST, 4, ";
                            passage_counter += 2;
                        }
                        else{
                            lisp_ofs << "-4" << endl;
                            cout << "1/1 rest" << endl;
                            arduino_ofs << "NOTE_REST, 8, ";
                            passage_counter += 4;
                        }
                        pre_symbol = "r";
                    }
                    else{
                        int other_symbols_flag = find_other_symbols(w_run, b_run, position, label_mat, label_position, target_label, staff_no, pre_symbol, key_sig, &key_type);
                        if(other_symbols_flag){
                            switch(other_symbols_flag){
                                case 1:
                                    if(pre_symbol == "k" || pre_symbol == "g"){
                                        cout << "{" << judge_key_signature(key_sig, key_type) << "}" << endl;
                                    }
                                    lisp_ofs << "-1" << endl;
                                    cout << "1/4 rest" << endl;
                                    arduino_ofs << "NOTE_REST, 2, ";
                                    pre_symbol = "r";
                                    passage_counter += 1;
                                    break;
                                case 2:
                                    cout << "(G_clef ト音記号)" << endl;
                                    pre_symbol = "g";
                                    break;
                                case 3:
                                    if(pre_symbol == "k" || pre_symbol == "g"){
                                        cout << "{" << judge_key_signature(key_sig, key_type) << "}" << endl;
                                    }
                                    cout << "(4/4_time_signature 4/4拍子)" << endl;
                                    pre_symbol = "t";
                                    break;
                                case 4:
                                    if(pre_symbol == "k" || pre_symbol == "g"){
                                        cout << "{" << judge_key_signature(key_sig, key_type) << "}" << endl;
                                    }
                                    cout << "(sharp)" << endl;
                                    pre_symbol = "s";
                                    break;
                                case 5:
                                    if(pre_symbol == "k" || pre_symbol == "g"){
                                        cout << "{" << judge_key_signature(key_sig, key_type) << "}" << endl;
                                    }
                                    cout << "(flat)" << endl;
                                    pre_symbol = "f";
                                    break;
                                case 6:
                                    if(pre_symbol == "k" || pre_symbol == "g"){
                                        cout << "{" << judge_key_signature(key_sig, key_type) << "}" << endl;
                                    }
                                    cout << "(natural)" << endl;
                                    pre_symbol = "n";
                                    break;
                                case 7:
                                    cout << "(key_sharp)" << endl;
                                    pre_symbol = "k";
                                case 8:
                                    cout << "(key_flat)" << endl;
                                    pre_symbol = "k";
                                default:
                                    break;
                            }
                        }
                        else{
                            if(pre_symbol == "k" || pre_symbol == "g"){
                                cout << "{" << judge_key_signature(key_sig, key_type) << "}" << endl;
                            }
                            cout << "(unknown)" << endl;
                            pre_symbol = "u";
                        }
                    }
                }
            }
        }
        if(passage_counter == 4){//小節毎に区切り
            cout << "------------" << endl;
            passage_counter = 0;
            for(int i=0; i<7; i++){
                acc_sig[i] = 100;
            }
        }
    }
    
    cout << "\nfinish.\n***********************************" << endl;
    arduino_ofs << "0, 4" << endl << "};" << endl;
}
