//
// Created by 36574 on 2019-07-11.
//

#ifndef OPENCVTESSERACTOCR_FACEDETECTOR_H
#define OPENCVTESSERACTOCR_FACEDETECTOR_H


#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

class facedetector {
  public:
    static void loadcascade(char * filepath);
    static vector<Mat> detectorface(Mat &src);
};


#endif //OPENCVTESSERACTOCR_FACEDETECTOR_H
