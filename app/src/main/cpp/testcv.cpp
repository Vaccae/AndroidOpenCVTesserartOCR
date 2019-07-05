//
// Created by 36574 on 2019-06-25.
//

#include "testcv.h"


bool testcv::VerifySize(RotatedRect candidate) {
    float error = 0.2; //20%的误差范围
    float aspect = 4.7272;//宽高比例
    int min = 15 * aspect * 15; //最小像素为15
    int max = 125 * aspect * 125;//最大像素为125
    float rmin = aspect - aspect * error;//最小误差
    float rmax = aspect + aspect * error;//最大误差
    int area = candidate.size.height * candidate.size.width;//求面积
    float r = (float) candidate.size.width / (float) candidate.size.height;//长宽比
    if (r < 1) r = 1 / r;
    if (area < min || area > max || r < rmin || r > rmax
        || abs(candidate.angle) > 10 || candidate.size.width < candidate.size.height) {
        return false;
    } else {
        return true;
    }
}

//获取多个截取的矩形
std::vector<Mat> testcv::getrectdetector(Mat &src) {
    Mat gray, imgsobel, dst;
    //转为灰度图
    cvtColor(src, gray, cv::COLOR_BGRA2GRAY);
    //高斯模糊
    GaussianBlur(gray, gray, Size(5, 5), 0.5, 0.5);
    //利用sobel滤波，对x进行求导，就是强调Y方向
    Sobel(gray, imgsobel, CV_8U, 1, 0, 3);
    //二值化
    threshold(imgsobel, imgsobel, 0, 255, THRESH_BINARY | THRESH_OTSU);
    //闭操作  这个Size很重要
    Mat element = getStructuringElement(MORPH_RECT, Size(21, 5));
    morphologyEx(imgsobel, imgsobel, MORPH_CLOSE, element);

    //提取轮廓
    std::vector<std::vector<cv::Point>> contours;
    findContours(imgsobel, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

    //用来存放旋转矩形的容器
    std::vector<RotatedRect> Rotatedrects;

    //判断图像
    for (size_t i = 0; i < contours.size(); i++) {
        //用来存放旋转矩形4个点
        Point2f Vertices[4];
        //寻找最小矩形
        RotatedRect currentrect = minAreaRect(Mat(contours[i]));
        //判断是不是要找的区域，如果是画线
        if (VerifySize(currentrect)) {
            currentrect.points(Vertices);
            //在源图上画四点的线
            for (size_t j = 0; j < 4; j++) {
                line(src, Vertices[j], Vertices[(j + 1) % 4], Scalar(0, 0, 255),
                     3);
            }
            //将符合的矩形存放到容器里
            Rotatedrects.push_back(currentrect);
        }
    }

    //用于存放识别到的图像
    std::vector<Mat>output;
    for (size_t i = 0; i < Rotatedrects.size(); i++) {
        Mat dst_warp;
        Mat dst_warp_rotate;
        Mat rotMat(2, 3, CV_32FC1);
        dst_warp = Mat::zeros(src.size(), src.type());
        float r = (float)Rotatedrects[i].size.width / (float)Rotatedrects[i].size.height;
        float  angle = Rotatedrects[i].angle;
        if (r < 1)
            angle = angle + 90;

        //其中的angle参数，正值表示逆时针旋转，关于旋转矩形的角度，以为哪个是长哪个是宽，在下面会说到
        rotMat = getRotationMatrix2D(Rotatedrects[i].center,angle, 1);
        //将矩形通过仿射变换修正回来
        warpAffine(src, dst_warp_rotate, rotMat, dst_warp.size());

        Size rect_size = Rotatedrects[i].size;
        if (r < 1)
            swap(rect_size.width, rect_size.height);

        //定义输出的图像
        Mat dst(Rotatedrects[i].size, CV_8U);
        //裁剪矩形,下面的函数只支持CV_8U 或者CV_32F格式的图像输入输出。
        //所以要先转换图像将RGBA改为RGB
        cvtColor(dst_warp_rotate, dst_warp_rotate, CV_RGBA2RGB);
        //裁剪矩形
        getRectSubPix(dst_warp_rotate, rect_size, Rotatedrects[i].center, dst);

        //将裁减到的矩形设置为相同大小，并且提高对比度
        Mat resultResized;
        resultResized.create(33, 144, CV_8UC3);
        resize(dst, resultResized, resultResized.size(), 0, 0, INTER_CUBIC);
        Mat grayResult;
        cvtColor(resultResized, grayResult, CV_BGR2GRAY);
        blur(grayResult, grayResult, Size(3, 3));
        //均值化提高对比度
        equalizeHist(grayResult, grayResult);

        //最终生成的矩形存放进vector<Mat>中
        output.push_back(grayResult);
    }

    return output;
}
