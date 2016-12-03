//
// Created by xae18 on 11/27/16.
//
#include <opencv2/imgproc.hpp>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/face.hpp"
#include <opencv2/objdetect.hpp>
#include <flandmark_detector.h>
#include <linreg.h>
#include "face_alignment.h"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace cv::face;

#ifndef FOTOPRIV_FACEPROCESSOR_H
#define FOTOPRIV_FACEPROCESSOR_H

class FaceProcessor {

private:
    /* Member berries*/
    string storage_path_;
    string fotopriv_model_;
    CascadeClassifier face_cascade_;
    CascadeClassifier eyes_cascade_;
    FLANDMARK_Model *flandmark_model_;
    Ptr<FaceRecognizer> fr_model_;

    /* Private functions */
    Mat align_face(Mat frame, vector<Rect> faces);
    Mat crop_face(Mat aligned);

public:
    FaceProcessor(string image, Ptr<FaceRecognizer> model); // constructor

    vector<Rect> detect_face(string); // return vector of Rects with face dimension if detected
    Mat process_face(Mat, vector<Rect>);// crops and aligns faces
    bool recognize_face(Mat);
};
#endif //FOTOPRIV_FACEPROCESSOR_H
