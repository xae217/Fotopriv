/**
    Fotopriv - Registrar header file.
    User registration for facial recognition.
    Registrar.h
    Purpose: Trains face recognition model based on user images.

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
    /**
        Constructor.

        @param string path to image.
        @param string path to internal storage.
        @param model face recognition model.
    */
    Registrar(string filename, string path, Ptr<FaceRecognizer> model);

    /**
        Registers a user, that is trains the model and saves it.

        @return nada
    */
    void register_user();
};

#endif //FOTOPRIV_REGISTRAR_H
