//
// Created by xae18 on 11/29/16.
//
#include <opencv2/imgproc.hpp>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/face.hpp"
#include <opencv2/objdetect.hpp>
#include <flandmark_detector.h>
#include <linreg.h>
#include "face_alignment.h"

#include "FaceProcessor.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace cv::face;


#ifndef FOTOPRIV_REGISTRAR_H
#define FOTOPRIV_REGISTRAR_H

class Registrar {
private:
    Ptr<FaceRecognizer> model_;
    string fotopriv_model_;
    string csv_file_;
    string storage_path_;

    vector<Mat> images_;
    vector<int> labels_;

    void read_csv();
    void train_model();

public:
    Registrar(string, string, Ptr<FaceRecognizer>);
    void register_user();
};

#endif //FOTOPRIV_REGISTRAR_H
