#include <jni.h>
#include <string>
#include "opencv2/opencv.hpp"
#include <android/log.h>
#include <vector>
#include "cv_helper.h"
#include "opencv2/opencv_modules.hpp"
#include "Android_Define.h"


using namespace cv;
using namespace std;






void filterSimilarArea(vector<vector<Point>> &temp,
                      unordered_map<int, vector<vector<Point>>> &similarArea){

    if(temp.size() <= 1){
        return;
    }

    //每100一个区间
    int compare = 0;
    int index = 0;
    int i = 0;
    int split = 0;

    while(i < temp.size()){

        double  c1 = contourArea(temp[compare]);
        double c2 = contourArea(temp[i]);

        if(c1 + 100> c2&&c1-100<c2){
            vector<vector<Point>> t = similarArea[compare];
            t.push_back(temp[i]);
            similarArea[compare] = t;
        }else{
            compare = i;
        }

        i++;
    }



}

double get_angle(Point p1, Point p2, Point center)
{
    double theta = atan2(p1.x - center.x, p1.y - center.y) - atan2(p2.x - center.x,
            p2.y - center.y);
    if (theta > M_PI)
        theta -= 2 * M_PI;
    if (theta < -M_PI)
        theta += 2 * M_PI;

    theta = abs(theta * 180.0 / M_PI);
    return theta;
}

double get_distance (Point pointO,Point pointA){

    double distance;
    distance = powf((pointO.x - pointA.x), 2) + powf((pointO.y - pointA.y), 2);

    distance = sqrtf(distance);

    return distance;
}


void filter_by_angle(unordered_map<int, vector<vector<Point>>> &similarArea,
                     unordered_map<int, vector<vector<Point>>> &similarAngle){

    for(auto it = similarArea.begin();it != similarArea.end();it++){
        vector<vector<Point>> temp = it->second;
        if(temp.size() < 3){
            continue;
        }
        unordered_map<int,vector<int>> hasAdd;
        for(int i = 0;i<temp.size();i++){
            for(int j = 0;j<temp.size();j++){
                for(int k = 0;k<temp.size();k++){
                    //找三个点的夹角
                    //保证三点不一致
                    if(i ==j||i==k||j==k){
                        continue;
                    }
                    double angle = get_angle(temp[i].at(0),temp[j].at(0),temp[k].at(0));

                    if(angle > 85&&angle< 95){
                        double line1 = get_distance(temp[i].at(0),temp[k].at(0));
                        double line2 = get_distance(temp[j].at(0),temp[k].at(0));


                        //并且两边相似长度
                        double diff = (int)line1 >> 4;
                        if(abs(line1 - line2) < (diff)){
                            //以中心夹角为核心记录，vector第一个点为中心夹角点
                            //添加之前遍历循环一次
                            //可能有一点问题，此时如果找到第二个90度的夹角中心是同一个点就会覆盖掉原来的区域
                            //概率低，可以改进，记录下来
                            int index = k;
                            int size = temp.size();
                            bool hasSameAngle = false;
                            //模仿跳表
                            //终止条件是hasAdd到达了末尾,以及hasAdd中找到了该夹角已经加入缓存中
                            while(hasAdd.find(index) != hasAdd.end()){
                                if(hasAdd.find(index)!=hasAdd.end()){
                                    if((hasAdd[index].at(0) == i&&hasAdd[index].at(1)==j)
                                       ||(hasAdd[index].at(0) == j&&hasAdd[index].at(1)==i)){
                                        //不断的查找沿路上所有的夹角
                                        hasSameAngle = true;
                                        //找到了则推出循环,不需要添加到列表中
                                        break;
                                    }
                                }
                                index += k+size;
                            }

                            if(hasAdd.find(index)!=hasAdd.end()){
                                if(hasSameAngle){
                                    continue;
                                }else{
                                    //尽量不去覆盖
                                    similarAngle[index].push_back(temp[k]);
                                    similarAngle[index].push_back(temp[j]);
                                    similarAngle[index].push_back(temp[i]);
                                    hasAdd[index].push_back(j);
                                    hasAdd[index].push_back(i);
                                }
                            }else{
                                //没添加过的夹角
                                similarAngle[index].push_back(temp[k]);
                                similarAngle[index].push_back(temp[j]);
                                similarAngle[index].push_back(temp[i]);
                                hasAdd[index].push_back(j);
                                hasAdd[index].push_back(i);
                            }

                        }

                    }
                }
            }
        }

    }
}








bool com(const vector<Point> &value1,const vector<Point> &value2){
    return contourArea(value1) < contourArea(value2);
}

void filterContours(unordered_map<int, vector<vector<Point>>> &resultSet,
        vector<vector<Point>> &contoursRes){
    //可能存在多个二维码。
    //要做的是，找到三个相似的体积，并且三个标识位的间距接近，且夹角接近为90
    //做一次排序，先排除掉面积过小的

    vector<vector<Point>> temp;
    for(int i = 0;i<contoursRes.size();i++){
        if(contourArea(contoursRes[i]) < 20){
            continue;
        }

        temp.push_back(contoursRes[i]);
    }
    std::sort(temp.begin(),temp.end(),com);


    //先模板匹配，找到一致的
    vector<vector<Point>> similar;

    unordered_map<int, vector<vector<Point>>> similarArea;
    //查找面积接近相似的区域
    filterSimilarArea(temp,similarArea);

    //查找角度接近90
    filter_by_angle(similarArea,resultSet);

}





extern "C"
JNIEXPORT jobject JNICALL
Java_com_yjy_qrfinder_QRFinder_findQR(JNIEnv *env, jclass type, jobject bitmap) {

    // TODO
    Mat src;
    //转化为mat
    cv_helper::bitmapToMat(env,bitmap,src);

    //灰度
    Mat gray;
    cvtColor(src,gray,COLOR_RGB2GRAY);

    //二值化
    Mat binary;
    //只存在两种像素
    //根据直方均衡进行二值化
    threshold(gray,binary,0,255,THRESH_BINARY|THRESH_OTSU);


    //筛选轮廓
    vector<vector<Point>> contours;
    vector<vector<Point>> contoursRes;
    vector<Vec4i> hierarchy;



    //先膨胀，再腐蚀
    int erosion_size = 2;
    Mat element = getStructuringElement( MORPH_RECT,
                                         Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                         Point( erosion_size, erosion_size ) );

    //先膨胀，再腐蚀
    dilate(binary,binary,element);
    erode(binary,binary,element);

    findContours(binary,contours,hierarchy,RETR_TREE,CHAIN_APPROX_NONE, Point(0, 0));

    //找到所有的轮廓
    //一个父轮廓，包含2个小轮廓，三个轮廓的夹角是90度
    //hierarchy元素hierarchy[i][0] ~hierarchy[i][3]，
    // 分别表示后一个轮廓、前一个轮廓、父轮廓、内嵌轮廓的索引编号，如果没有对应项，则该值为负数。

    int tCC = 0; // 临时用来累加的子轮廓计数器
    int pId = -1;// 父轮廓的 index

    for(int i = 0;i<contours.size();i++){
        if(hierarchy[i][2] != -1&&tCC ==0){
            //找到第一个父轮廓
            pId = i;
            tCC++;
        }else if(hierarchy[i][2] != -1){
            //找到父轮廓
            tCC++;
        }else if(hierarchy[i][2] == -1){
            //没有父轮廓
            pId = -1;
            tCC = 0;
        }

        if(tCC >= 2){
            //包含两个子轮廓
            contoursRes.push_back(contours[pId]);
            tCC = 0;
            pId = -1;
        }
    }


    unordered_map<int, vector<vector<Point>>> resultSet;
    if(contoursRes.size() > 3){

        filterContours(resultSet,contoursRes);

        for(auto it = resultSet.begin();it != resultSet.end();it++){
            vector<vector<Point>> temp = it->second;
            for (int i = 0; i < temp.size(); i++) {
                //通过大小，通过左右最小和最大
                drawContours(src,temp,i,Scalar(0,0,255),1);
            }


            for(auto it = resultSet.begin();it != resultSet.end();it++){
                vector<vector<Point>> temp = it->second;

                int maxLeft = INT_MAX, maxTop = INT_MAX,maxRight = 0,maxBottom = 0;
                for(int i = 0;i<temp.size();i++){
                    vector<Point> t = temp[i];
                    for(int j = 0;j<t.size();j++){
                        if(temp[i].at(j).x < maxLeft){
                            maxLeft = temp[i].at(j).x;
                        }

                        if(temp[i].at(j).x > maxRight){
                            maxRight = temp[i].at(j).x;
                        }

                        if(temp[i].at(j).y < maxTop){
                            maxTop = temp[i].at(j).y;
                        }

                        if(temp[i].at(j).y > maxBottom){
                            maxBottom = temp[i].at(j).y;
                        }
                    }
                }

                line(src,Point(maxLeft,maxTop),Point(maxLeft,maxBottom),Scalar(255,0,0),1,LINE_AA);
                line(src,Point(maxLeft,maxBottom),Point(maxRight,maxBottom),Scalar(255,0,0),1,LINE_AA);
                line(src,Point(maxRight,maxBottom),Point(maxRight,maxTop),Scalar(255,0,0),1,LINE_AA);
                line(src,Point(maxRight,maxTop),Point(maxLeft,maxTop),Scalar(255,0,0),1,LINE_AA);
            }



        }
    }else{
        //画到图片上
        for (int i = 0; i < contoursRes.size(); i++) {
            //通过大小，通过左右最小和最大
            drawContours(src,contoursRes,i,Scalar(0,0,255),1);
        }

    }

    cv_helper::matTobitmap(env,bitmap,src,false);

    return bitmap;

}