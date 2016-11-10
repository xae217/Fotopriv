//
// Created by xae18 on 10/17/16
//

#include <com_xae18_fotopriv_NativeClass.h>
#include <opencv2/dnn.hpp>
#include <opencv2/dnn/blob.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "opencv2/core.hpp"
#include "opencv2/face.hpp"
#include <opencv2/objdetect.hpp>

#include <flandmark_detector.h>
#include <linreg.h>

#include "FaceProcessing.h"

#include <fstream>
#include <iostream>
#include <cstdlib>
#include <sstream>

using namespace cv;
using namespace cv::dnn;
using namespace cv::face;
using namespace std;

/** Global variables */
String face_cascade_name = "/sdcard/haar/haarcascade_frontalface_alt.xml";
String eyes_cascade_name = "/sdcard/haar/haarcascade_eye_tree_eyeglasses.xml";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
RNG rng(12345); // Not used yet //TODO: make sure to use this or delete it

//TODO: remove these when done with testing
char img_counter = 49; // Counter for images.
char img_counter2 = 48;

//Prototype
//TODO: create header file and refactor code
Mat processFace(Mat frame);

//face recognition model
Ptr<FaceRecognizer> model = createLBPHFaceRecognizer(1,8,8,8,82.0);

const char* fotopriv_model = "/sdcard/saved-model/fotopriv.yml";

Mat norm_0_255(InputArray _src) {
    Mat src = _src.getMat();
    // Create and return normalized image:
    Mat dst;
    switch(src.channels()) {
    case 1:
        cv::normalize(_src, dst, 0, 255, NORM_MINMAX, CV_8UC1);
        break;
    case 3:
        cv::normalize(_src, dst, 0, 255, NORM_MINMAX, CV_8UC3);
        break;
    default:
        src.copyTo(dst);
        break;
    }
    return dst;
}


void read_csv(const string& filename, vector<Mat>& images, vector<int>& labels, char separator = ';') {
    std::ifstream file(filename.c_str(), ifstream::in);

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
            Mat processed_img = processFace(img);
            //resize(img, img, Size(100, 100));
            if(!processed_img.empty()) {
                String write_path = string("/sdcard/aligned/p-") + img_counter2 + img_counter  + ".jpg";
                //imwrite(write_path, processed_img);

                if(img_counter == 57) {
                    img_counter = 48;
                    img_counter2++;
                }
                else {
                    img_counter++;
                }

                images.push_back(processed_img);
                labels.push_back(atoi(classlabel.c_str()));
            }
        }
    }
}


/* Find best class for the blob (i. e. class with maximal probability) */
void getMaxClass(dnn::Blob &probBlob, int *classId, double *classProb)
{
    Mat probMat = probBlob.matRefConst().reshape(1, 1); //reshape the blob to 1x1000 matrix
    Point classNumber;

    minMaxLoc(probMat, NULL, classProb, NULL, &classNumber);
    *classId = classNumber.x;
}


std::vector<String> readClassNames(const char *path) {
    char filename[1000];
    strcpy(filename, path);
    strcat(filename, "/synset_words.txt");
    std::vector<String> classNames;

    std::ifstream fp(filename);
    if (!fp.is_open()) {
        std::cerr << "File with classes labels not found: " << filename << std::endl;
        exit(-1);
    }

    std::string name;
    while (!fp.eof()) {
        std::getline(fp, name);
        if (name.length()) {
            classNames.push_back(name.substr(name.find(' ')+1));
        }
    }

    fp.close();
    return classNames;
}


Mat alignFace(Mat frame) {
    FLANDMARK_Model* model = flandmark_init("/sdcard/model/flandmark_model.dat");
    Mat res;
    std::vector<Rect> faces;
    vector<cv::Point2d> landmarks;

    face_cascade.detectMultiScale(frame, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

    if(!faces.empty()) {
        landmarks = detectLandmarks(model, frame, faces[0]);
        Mat aligned_image;
        vector<cv::Point2d> aligned_landmarks;

        if(landmarks.size() == 0) {
            cerr << "Can't find landmarks." << endl;
            return frame;
        }

        align(frame, aligned_image, landmarks, aligned_landmarks);

        if(!aligned_image.empty()) {

            resize(aligned_image, res, Size(100, 100), 0, 0, INTER_CUBIC);
            //imwrite("/sdcard/aligned/linear.jpg", res);
            return res;
        }
    }
    return frame;
}


Mat processFace(Mat frame) {
    std::vector<Rect> faces;
    Mat crop;
    Mat aligned;
    Mat res;

    //TODO: test equalized vs non-equalized
    //equalizeHist(frame, frame);

    aligned = alignFace(frame);

    if (!aligned.empty()) {
        /*
        // Detect faces
        face_cascade.detectMultiScale(aligned, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

        // Set Region of Interest
        cv::Rect roi_b;
        cv::Rect roi_c;

        size_t ic = 0; // ic is index of current element
        int ac = 0; // ac is area of current element

        size_t ib = 0; // ib is index of biggest element
        int ab = 0; // ab is area of biggest element

        int reduce = 10;
        for (ic = 0; ic < faces.size(); ic++) { // Iterate through all current elements (detected faces)

            roi_c.x = faces[ic].x + reduce/2;
            roi_c.y = faces[ic].y + reduce * 2;
            roi_c.width = (faces[ic].width - reduce);
            roi_c.height = (faces[ic].height - reduce);

            ac = roi_c.width * roi_c.height; // Get the area of current element (detected face)

            roi_b.x = faces[ib].x + reduce/2;
            roi_b.y = faces[ib].y + reduce * 2;
            roi_b.width = (faces[ib].width - reduce);
            roi_b.height = (faces[ib].height - reduce);

            ab = roi_b.width * roi_b.height; // Get the area of biggest element, at beginning it is same as "current" element

            if (ac > ab) {
                ib = ic;
                roi_b.x = faces[ib].x;
                roi_b.y = faces[ib].y;
                roi_b.width = (faces[ib].width);
                roi_b.height = (faces[ib].height);
            }

            crop = aligned(roi_b);

            //cvtColor(crop, gray, CV_BGR2GRAY); // Convert cropped image to Grayscale
        }
        */
        int offset_x = 20;
        int offset_y = 30;

        cv::Rect roi;
        roi.x = offset_x;
        roi.y = offset_y;
        roi.width = aligned.size().width - (offset_x*2);
        roi.height = aligned.size().height - (offset_y*2 - 20);
        crop = aligned(roi);
        if (!crop.empty()) {
            resize(crop, res, Size(100, 100), 0, 0, INTER_CUBIC);
            //imwrite("/sdcard/aligned/crop.jpg", res);
            return res;
        }
    }
    //imwrite("/sdcard/aligned/crop-aligned.jpg", res);
    return res; //res is empty
    //imwrite("/sdcard/aligned/test2.jpg", frame);
    //return crop;
}


void trainModel() {
    // These vectors hold the images and corresponding labels.
    std::vector<int> labels;
    std::vector<Mat> images;

    string fn_csv = "/sdcard/csv/faces.csv";

    // Read in the data. This can fail if no valid
    // input filename is given.
    try {
        read_csv(fn_csv, images, labels);
    } catch (cv::Exception& e) {
        cerr << "Error opening file \"" << fn_csv << "\". Reason: " << e.msg << endl;
        // nothing more we can do
        exit(1);
    }
    // Quit if there are not enough images for this demo.
    if(images.size() <= 1) {
        string error_message = "At least 2 images to recognize a face.";
        CV_Error(CV_StsError, error_message);
    }
    model->train(images, labels);
    model->save(fotopriv_model);
}


bool model_found() {
    //std::ifstream infile(fotopriv_model);
    //return infile.good();
    return true;
}

string recognizeFace(const char* path) {
    string imageFile = "/data/data/com.xae18.fotopriv/cache/image.jpg";

    Mat frame;
    Mat testimg;

    //Load the cascades
    if(!face_cascade.load(face_cascade_name ) ){
        return "face_cascade file not found.";
    }

    if(!eyes_cascade.load(eyes_cascade_name ) ){
        return "eye_cascade file not found.";
    }

    frame = imread(imageFile, 0);

    if(!frame.empty() ){
        testimg = processFace(frame);
        //resize(testimg, testimg, Size(100, 100));
    }

    if (true) {
        //return "model was found!";
        model->load(fotopriv_model);
    }
    else {
        trainModel();
    }

    //Mat testSample = imread(imageFile, 0); // 0 loads it as grayscale
    //resize(testSample, testSample, Size(100, 100));
    int predictedLabel = -1;
    double confidence = 0.0;
    if(!testimg.empty()) {
        model->predict(testimg, predictedLabel, confidence);
    }
    else {
        return "Was not able to detect face.";
    }

    //TODO: remember to use confidence
    string result_message;
    //result_message = format("Label is %d. Confidence: %f", predictedLabel, confidence);
    if(predictedLabel == 0) {
        result_message = "You are in this picture.";
    }
    else {
        result_message = "I don't know this face.";
    }
    //std::cout << result_message << std::endl;
    return result_message;
}

string recognizeGoogLenet(const char* path) {
    String modelTxt = string(path) + "bvlc_googlenet.prototxt.txt";
    String modelBin = string(path) + "bvlc_googlenet.caffemodel";
    String imageFile = "/data/data/com.xae18.fotopriv/cache/image.jpg";


    //! [Create the importer of Caffe model]
    Ptr<dnn::Importer> importer;
    try {
        importer = dnn::createCaffeImporter(modelTxt, modelBin);
    }
    catch (const cv::Exception &err) {
        std::cerr << err.msg << std::endl;
    }
    // Create the importer of Caffe model

    if (!importer) {
        std::cerr << "Can't load network by using the following files: " << std::endl;
        exit(-1);
    }

    // initialize network
    dnn::Net net;
    importer->populateNet(net);
    importer.release();                     //We don't need importer anymore
    //! [Initialize network]

    Mat img = imread(imageFile);
    if (img.empty()) {
        std::cerr << "Can't read image from the file: " << imageFile << std::endl;
        exit(-1);
    }

    resize(img, img, Size(224, 224));       //GoogLeNet accepts only 224x224 RGB-images
    dnn::Blob inputBlob = dnn::Blob::fromImages(img);   //Convert Mat to dnn::Blob image batch
    //set input blob
    net.setBlob(".data", inputBlob);
    net.forward();
    dnn::Blob prob = net.getBlob("prob");
    int classId;
    double classProb;
    //find best class
    getMaxClass(prob, &classId, &classProb);

    std::stringstream sstr;
    sstr << classId;
    std::string str1 = sstr.str();


    std::vector<String> classNames = readClassNames(path);
    std::cout << "Best class: #" << classId << " '" << classNames.at(classId) << "'" << std::endl;

    return classNames.at(classId);
}

JNIEXPORT jstring JNICALL Java_com_xae18_fotopriv_NativeClass_getStringFromNative
        (JNIEnv * env, jobject obj, jint selection, jstring path){

    const char *storagePath = env->GetStringUTFChars(path, 0);

    if (selection == 1) {
        return env->NewStringUTF(recognizeFace(storagePath).c_str());
    }
    else if (selection == 0) {
        return env->NewStringUTF(recognizeGoogLenet(storagePath).c_str());
    }

    env->ReleaseStringUTFChars(path, storagePath);
}