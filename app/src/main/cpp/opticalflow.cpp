//
// Created by 36574 on 2019-08-11.
//

#include "opticalflow.h"

//定义前一帧灰度图
Mat prev_gray;
//定义数据
Mat flowdata;

//绘制结果的函数
void opticalflow::drawcalcFlowHF(Mat &flowdata, Mat &image, int step) {
    for (size_t row = 0; row < flowdata.rows; row++) {
        for (size_t col = 0; col < flowdata.cols; col++) {
            const Point2f fxy = flowdata.at<Point2f>(row, col);
            //判断大于输入的step后进入
            if (fxy.x > step || fxy.y > step) {
                //用绿色画线点到前一帧的差
                line(image, Point(col, row),
                     Point(cvRound(col + fxy.x), cvRound(row + fxy.y)),
                     Scalar(0, 255, 0));
                //用红色画点为当前的点
                circle(image, Point(col, row), 2, Scalar(255, 0, 0), -1);
            }
        }
    }
}

//稠密光流
vector<Mat> opticalflow::dealOpticalFlow(Mat &src) {
    Mat gray;
    //转为灰度图
    cvtColor(src, gray, COLOR_BGRA2GRAY);
    //判断是否有前一帧图像
    if (!prev_gray.empty()) {
        //存在前一帧进行稠密光流操作
        calcOpticalFlowFarneback(prev_gray, gray, flowdata,
                                 0.5, 3, 15, 3, 5, 1.2, 0);

        //将结果绘画出来，这里参数为20就是变化大的才显示出来
        drawcalcFlowHF(flowdata, src, 20);
    }
    //将前一帧图像存放到
    gray.copyTo(prev_gray);

    return vector<Mat>();
}


