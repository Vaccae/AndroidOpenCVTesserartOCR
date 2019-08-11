//
// Created by 36574 on 2019-06-25.
//

#ifndef TESSERACTOCR_TESTCV_H
#define TESSERACTOCR_TESTCV_H

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

class testcv {
private:
    static bool VerifySize(RotatedRect candidate);
public:
    static vector<Mat> getrectdetector(Mat &src);
};


#endif //TESSERACTOCR_TESTCV_H
