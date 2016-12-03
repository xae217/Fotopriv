//
// Created by xae18 on 11/29/16.
//

#include "Registrar.h"

Registrar::Registrar(string filename, string path, Ptr<FaceRecognizer> model):csv_path_(filename),
    model_(model), storage_path_(path) {
        fotopriv_model_ = storage_path_ + "fotopriv.yml";
    };

void Registrar::read_csv() {
    char separator = ';';
    std::ifstream file(csv_path_.c_str(), ifstream::in);

    if (!file) {
        string error_message = "No valid input file was given, please check the given filename.";
        CV_Error(CV_StsBadArg, error_message);
    }

    string line, path, classlabel;

    while (getline(file, line)) {

        stringstream liness(line);
        getline(liness, path, separator);
        getline(liness, classlabel);

        if(!path.empty() && !classlabel.empty()) {
            Mat img = imread(path, 0);
            //TODO: process images (detect, crop and align)
            FaceProcessor *fp = new FaceProcessor(path, model_);
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
        cerr << "Error opening file \"" << csv_path_ << "\". Reason: " << e.msg << endl;
        // nothing more we can do
        exit(1);
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