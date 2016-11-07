//
// Created by xae18 on 11/6/16.
//

#ifndef FOTOPRIV_FACEPROCESSING_H
#define FOTOPRIV_FACEPROCESSING_H

/*
 * Copyright (c) 2013. Alberto Fernandez Villan <alberto[dot]fernandez[at]fundacionctic[dot]org>.
 * Released to public domain under terms of the BSD Simplified license.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the organization nor the names of its contributors
 *     may be used to endorse or promote products derived from this software
 *     without specific prior written permission.
 *
 *   See <http://www.opensource.org/licenses/bsd-license>
 */

#include "flandmark_detector.h"
#include "linreg.h"
#include <opencv2/objdetect.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;



/** enum with all landmarks (0-7)--> flandmarks and 8,9 added */
enum landmark_pos {
    FACE_CENTER = 0,
    LEFT_EYE_INNER = 1,
    RIGHT_EYE_INNER = 2,
    MOUTH_LEFT = 3,
    MOUTH_RIGHT = 4,
    LEFT_EYE_OUTER = 5,
    RIGHT_EYE_OUTER = 6,
    NOSE_CENTER = 7,
    LEFT_EYE_ALIGN = 8,
    RIGHT_EYE_ALIGN = 9
};

/** detect biggest face */
Rect detect_face(const Mat& image, CascadeClassifier cascade);

/** rotate points based on rot_mat */
void get_rotated_points(const std::vector<cv::Point2d> &points, std::vector<cv::Point2d> &dst_points, const cv::Mat &rot_mat);

/** show landmarks in an image */
void show_landmarks(const std::vector<cv::Point2d> &landmarks, const cv::Mat& image, const string &named_window);


/** aligns the face based on the recalculated positions of the eyes and aligns also the landmarks*/
double align(const Mat &image, Mat &dst_image, vector<Point2d> &landmarks, vector<Point2d> &dst_landmarks);

/** detects landmarks using flandmakrs and add two more landmakrs to be used to alignt the face*/
vector<cv::Point2d> detectLandmarks(FLANDMARK_Model* model, const Mat & image, const Rect & face);

#endif //FOTOPRIV_FACEPROCESSING_H
