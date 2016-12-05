//
// Created by xae18 on 11/27/16.
//
#include "FaceProcessor.h"

using namespace cv;
using namespace cv::face;
using namespace std;

FaceProcessor::FaceProcessor(string path, Ptr<FaceRecognizer> model):storage_path_(path), fr_model_(model) {
    //load cascades
    string face_cascade_path = storage_path_ + "haarcascade_frontalface_alt.xml";
    string eyes_cascade_path = storage_path_ + "haarcascade_eye_tree_eyeglasses.xml";
    string flandmark_path = storage_path_ + "flandmark_model.dat";

    face_cascade_.load(face_cascade_path);
    eyes_cascade_.load(eyes_cascade_path);
    flandmark_model_ = flandmark_init(flandmark_path.c_str());

    fotopriv_model_ = storage_path_ + "/fotopriv.yml";
}

/* Returns empty Mat if failed to detect */
vector<Rect> FaceProcessor::detect_face(string image) {
    Mat frame = imread(image, 0);
    std::vector<Rect> faces;

    if (!frame.empty()) {
        face_cascade_.detectMultiScale(frame, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));
    }

    return faces;
}

Mat FaceProcessor::process_face(Mat frame, vector<Rect> faces) {
    Mat processed_face = crop_face(align_face(frame, faces));
    return processed_face;
}


/* Returns original frame if alignment failed. */
Mat FaceProcessor::align_face(Mat frame, vector<Rect> faces) {
    Mat res;
    vector<cv::Point2d> landmarks;

    if(!faces.empty()) {
        landmarks = detectLandmarks(flandmark_model_, frame, faces[0]);
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

/* Returns original frame if cropping failed. */
Mat FaceProcessor::crop_face(Mat aligned) {
    //std::vector<Rect> faces;
    Mat crop;
    //Mat aligned;
    Mat res;

    //TODO: test equalized vs non-equalized
    //equalizeHist(frame, frame);

    //aligned = alignFace(frame);

    if (!aligned.empty()) {
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
    return aligned; //original image
    //imwrite("/sdcard/aligned/test2.jpg", frame);
    //return crop;
}


bool FaceProcessor::recognize_face(Mat processed_face) {

    fr_model_->load(fotopriv_model_);
    int predicted_label = -1;
    double confidence = 0.0;

    if(!processed_face.empty()) {
        fr_model_->predict(processed_face, predicted_label, confidence);
    }
    else {
        //return "Was not able to detect face.";
        return false;
    }

    //TODO: remember to use confidence
    string result_message;
    //result_message = format("Label is %d. Confidence: %f", predictedLabel, confidence);
    //std::cout << result_message << std::endl;

    // Label 0 is the user
    return predicted_label == 0;
}


