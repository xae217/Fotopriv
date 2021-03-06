/**
    Fotopriv - Native Class (JNI)
    NativeClass.cpp
    Purpose: Native class for JNI calls. It handles image processing and user registration.

    @author Xavier Escobar
    @version 1.1
*/

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
#include "face_alignment.h"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include "FaceProcessor.h"
#include "Registrar.h"
#include <android/log.h>

#define APPNAME "Fotopriv"

using namespace cv;
using namespace cv::dnn;
using namespace cv::face;
using namespace std;

// Face recognition model
Ptr<FaceRecognizer> model = createLBPHFaceRecognizer(1,8,8,8,82.0);

/**
 * Find best class for the blob (i. e. class with maximal probability)
 */
void getMaxClass(dnn::Blob &probBlob, int *classId, double *classProb) {
    Mat probMat = probBlob.matRefConst().reshape(1, 1); //reshape the blob to 1x1000 matrix
    Point classNumber;

    minMaxLoc(probMat, NULL, classProb, NULL, &classNumber);
    *classId = classNumber.x;
}

/**
 * Read class names from file
 */
std::vector<string> readClassNames(const char *path) {
    char filename[1000];
    strcpy(filename, path);
    strcat(filename, "/synset_words.txt");
    std::vector<string> classNames;

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

/**
 * Perform detection using fine-tuned GoogLeNet
 * @param path path to internal storage
 * @return predicted class name
 */
string recognizeGoogLenet(const char* path) {
    string modelTxt = string(path) + "bvlc_googlenet.prototxt.txt";
    string modelBin = string(path) + "bvlc_googlenet.caffemodel";
    string imageFile = "/data/data/com.xae18.fotopriv/cache/image.jpg";

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
    resize(img, img, Size(224, 224));       //GoogLeNet accepts only 224x224 RGB images
    dnn::Blob inputBlob = dnn::Blob::fromImages(img);   //Convert Mat to dnn::Blob image batch

    net.setBlob(".data", inputBlob);  //set input blob
    net.forward();
    dnn::Blob prob = net.getBlob("prob");
    int classId;
    double classProb;

    getMaxClass(prob, &classId, &classProb); //find best class

    std::stringstream sstr;
    sstr << classId;
    std::string str1 = sstr.str();

    std::vector<string> classNames = readClassNames(path);
    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "%d: %f", classId, classProb);

    return classNames.at(classId);
}

/**
 * Perform privacy detection using both GoogLeNet and Face Recognition
 * @param path path to internal storage.
 * @param enable_fr enable face recognition (is user registered?).
 * @return analysis result.
 */
string analyze_image(string storage_path, bool enable_fr) {
    string report = "";
    string image_file = "/data/data/com.xae18.fotopriv/cache/image.jpg";
    Mat frame = imread(image_file, 0);
    FaceProcessor *fp = new FaceProcessor(storage_path, model);
    vector<Rect> faces = fp->detect_face(image_file);

    if (!faces.empty()) {
        report = report + "Face found|";
        Mat face = fp->process_face(frame, faces);
        if (enable_fr) {
            if(fp->recognize_face(face)) {
                report = report + "You are in this image|";
            }
            else {
                report = report + "You are not in this image|";
            }
        }
    }
    else {
        report = report + "Face not found|";
    }

    report =  report + recognizeGoogLenet(storage_path.c_str());
    return report;
}

JNIEXPORT jstring JNICALL Java_com_xae18_fotopriv_NativeClass_getStringFromNative
        (JNIEnv * env, jobject obj, jint selection, jstring path){

    const char *storagePath = env->GetStringUTFChars(path, 0);
    bool  enable_fr = selection != 0;
    return env->NewStringUTF(analyze_image(storagePath, enable_fr).c_str());

    env->ReleaseStringUTFChars(path, storagePath);
}

JNIEXPORT jint JNICALL Java_com_xae18_fotopriv_NativeClass_registerUser
    (JNIEnv * env, jobject obj, jstring csvpath, jstring storagepath) {

    const char *csv_path = env->GetStringUTFChars(csvpath, 0);
    const char *storage_path = env->GetStringUTFChars(storagepath, 0);

    Registrar *reg = new Registrar(csv_path, storage_path,  model);
    reg->register_user();
    return 1;
}

