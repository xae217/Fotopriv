//
// Created by xae18 on 10/17/16.
//

#include <com_xae18_fotopriv_NativeClass.h>
#include <opencv2/dnn.hpp>
#include <opencv2/dnn/blob.hpp>

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "opencv2/core.hpp"
#include "opencv2/face.hpp"
using namespace cv;
using namespace cv::dnn;
using namespace cv::face;

#include <fstream>
#include <iostream>
#include <cstdlib>

#include <sstream>
using namespace std;


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
            resize(img, img, Size(100, 100));
            images.push_back(img);
            labels.push_back(atoi(classlabel.c_str()));
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




std::vector<String> readClassNames(const char *filename = "/sdcard/android-opencv/synset_words.txt")
{
    std::vector<String> classNames;

    std::ifstream fp(filename);
    if (!fp.is_open())
    {
        std::cerr << "File with classes labels not found: " << filename << std::endl;
        exit(-1);
    }

    std::string name;
    while (!fp.eof())
    {
        std::getline(fp, name);
        if (name.length())
            classNames.push_back( name.substr(name.find(' ')+1) );
    }

    fp.close();
    return classNames;
}




JNIEXPORT jstring JNICALL Java_com_xae18_fotopriv_NativeClass_getStringFromNative
        (JNIEnv * env, jobject obj){

    String modelTxt = "/sdcard/android-opencv/bvlc_googlenet.prototxt.txt";
    String modelBin = "/sdcard/android-opencv/bvlc_googlenet.caffemodel";
    String imageFile = "/data/data/com.xae18.fotopriv/cache/image.jpg";


    String filename = "/sdcard/aligned-images/jimmy-fallon/";

    /******************************* FACE-RECOG ****************************************/
    string fn_csv = "/sdcard/csv/faces.csv";
    // These vectors hold the images and corresponding labels.
    std::vector<Mat> images;
    std::vector<int> labels;
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
        string error_message = "This demo needs at least 2 images to work. Please add more images to your data set!";
        CV_Error(CV_StsError, error_message);
    }
    // Get the height from the first image. We'll need this
    // later in code to reshape the images to their original
    // size:
    int height = images[0].rows;
    // Set test to input image
    Mat testSample = imread(imageFile, 0);
    resize(testSample, testSample, Size(100, 100));
    //images.pop_back();
    //labels.pop_back();

    Ptr<FaceRecognizer> model = createLBPHFaceRecognizer(1,8,8,8,86.0);
    model->train(images, labels);

    int predictedLabel = -1;
    double confidence = 0.0;
    model->predict(testSample, predictedLabel, confidence);
    string result_message = format("Confidence is: %f. Label is %d", confidence, predictedLabel);
    std::cout << result_message << std::endl;

    /******************************* GOOGLENET ******************************************/

    /*
    //! [Create the importer of Caffe model]
    Ptr<dnn::Importer> importer;
    try                                     //Try to import Caffe GoogleNet model
    {
        importer = dnn::createCaffeImporter(modelTxt, modelBin);
    }
    catch (const cv::Exception &err)        //Importer can throw errors, we will catch them
    {
        std::cerr << err.msg << std::endl;
    }
    //! [Create the importer of Caffe model]

    if (!importer)
    {
        std::cerr << "Can't load network by using the following files: " << std::endl;
        std::cerr << "prototxt:   " << modelTxt << std::endl;
        std::cerr << "caffemodel: " << modelBin << std::endl;
        std::cerr << "bvlc_googlenet.caffemodel can be downloaded here:" << std::endl;
        std::cerr << "http://dl.caffe.berkeleyvision.org/bvlc_googlenet.caffemodel" << std::endl;
        exit(-1);
    }

    //! [Initialize network]
    dnn::Net net;
    importer->populateNet(net);
    importer.release();                     //We don't need importer anymore
    //! [Initialize network]

    //! [Prepare blob]
    Mat img = imread(imageFile);
    if (img.empty())
    {
        std::cerr << "Can't read image from the file: " << imageFile << std::endl;
        exit(-1);
    }

    resize(img, img, Size(224, 224));       //GoogLeNet accepts only 224x224 RGB-images
    dnn::Blob inputBlob = dnn::Blob::fromImages(img);   //Convert Mat to dnn::Blob image batch
    //! [Prepare blob]

    //! [Set input blob]
    net.setBlob(".data", inputBlob);        //set the network input
    //! [Set input blob]

    //! [Make forward pass]
    net.forward();                          //compute output
    //! [Make forward pass]

    //! [Gather output]
    dnn::Blob prob = net.getBlob("prob");   //gather output of "prob" layer

    int classId;
    double classProb;
    getMaxClass(prob, &classId, &classProb);//find the best class
    //! [Gather output]

    std::stringstream sstr;
    sstr << classId;
    std::string str1 = sstr.str();


    std::vector<String> classNames = readClassNames();
    std::cout << "Best class: #" << classId << " '" << classNames.at(classId) << "'" << std::endl;
    std::cout << "Probability: " << classProb * 100 << "%" << std::endl;
    */

    return env->NewStringUTF(result_message.c_str());
    //return env->NewStringUTF(classNames.at(classId).c_str());


}
