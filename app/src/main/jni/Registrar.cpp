//
// Created by xae18 on 11/29/16.
//

#include "Registrar.h"

#include <android/log.h>
#define APPNAME "Fotopriv"

Registrar::Registrar(string filename, string path, Ptr<FaceRecognizer> model):csv_file_(filename),
    model_(model), storage_path_(path) {
        fotopriv_model_ = storage_path_ + "/fotopriv.yml";
        __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "%s", fotopriv_model_.c_str());
    };

void Registrar::read_csv() {
    char separator = ';';
    string csv_path = storage_path_ + "/" + csv_file_;
    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "%s", csv_path.c_str());
    std::ifstream file(csv_path.c_str(), ifstream::in);

    if (!file) {
        string error_message = "No valid input file was given: " + csv_path;
        CV_Error(CV_StsBadArg, error_message);
    }

    string line, path, classlabel;

    while (getline(file, line)) {

        stringstream liness(line);
        getline(liness, path, separator);
        getline(liness, classlabel);

        if(!path.empty() && !classlabel.empty()) {
            Mat img = imread(path, 0);
            FaceProcessor *fp = new FaceProcessor(storage_path_, model_);
            Mat processed_img = fp->process_face(img, fp->detect_face(path));
            //resize(img, img, Size(100, 100));
            if(!processed_img.empty()) {
                //string write_path = string("/sdcard/aligned/p-") + img_counter2 + img_counter  + ".jpg";
                //imwrite(write_path, processed_img);

                /*
                if(img_counter == 57) {
                    img_counter = 48;
                    img_counter2++;
                }
                else {
                    img_counter++;
                }
                */
                images_.push_back(processed_img);
                labels_.push_back(atoi(classlabel.c_str()));
            }
        }
    }
}

void Registrar::train_model() {

    // Read in the data. This can fail if no valid
    // input filename is given.
    try {
        read_csv();
    } catch (cv::Exception& e) {
        string error_message =  "Error opening file";
        // nothing more we can do
        CV_Error(CV_StsBadArg, error_message);
        //exit(1);
    }
    // Quit if there are not enough images for this demo.
    if(images_.size() <= 1) {
        string error_message = "At least 2 images to recognize a face.";
        CV_Error(CV_StsError, error_message);
    }
    model_->train(images_, labels_);
    model_->save(fotopriv_model_);
}

void Registrar::register_user() {
    train_model();
}