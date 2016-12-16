/**
    Fotopriv - Face Processing header file
    FaceProcessor.h
    Purpose: Handles face processing: face detection, alignment, cropping and recogntion.

    @author Xavier Escobar
    @version 1.1
*/
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

    /**
        Aligns face for facial recognition.

        @param frame Mat object image.
        @param faces Rect Vector of found faces.
        @return Returns the aligned face or the original frame if failed to align.
    */
    Mat align_face(Mat frame, vector<Rect> faces);

    /**
        Crops face for facial recognition.

        @param frame Aligned image.
        @return Returns the cropped face or the original frame if failed to crop.
    */
    Mat crop_face(Mat aligned);

public:
   /**
        FaceProcessor constructor.

        @param image path to image.
        @param model facial recognition model.
    */
    FaceProcessor(string image, Ptr<FaceRecognizer> model); // constructor

    /**
        Detects faces in image.

        @param image path to image.
        @return vector of detected faces as Rect objects.
    */
    vector<Rect> detect_face(string imge);
    /**
        Processes the image for face recognition (crop and align).

        @param frame image to be analyzed
        @param faces vector of detected faces (may be empty)
        @return Returns the processed image.
    */
    Mat process_face(Mat frame, vector<Rect> faces);

    /**
        Performs facial recognition on image.

        @param frame processed image
        @return Returns true if face was recognized.
    */
    bool recognize_face(Mat);
};
#endif //FOTOPRIV_FACEPROCESSOR_H
