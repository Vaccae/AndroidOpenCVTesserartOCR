//
// Created by 36574 on 2019-08-11.
//

#ifndef OPENCVTESSERACTOCR_OPTICALFLOW_H
#define OPENCVTESSERACTOCR_OPTICALFLOW_H

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

class opticalflow {
private:
    static void drawcalcFlowHF(Mat &flowdata,Mat &image,int step);
public:
    static vector<Mat> dealOpticalFlow(Mat &src);
};


#endif //OPENCVTESSERACTOCR_OPTICALFLOW_H
