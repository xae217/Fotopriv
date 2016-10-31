//
// Created by xae18 on 10/21/16.
//
#include "opencv2/core/core.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>
#include <fstream>
#include <sstream>


#ifndef FOTOPRIV_USERRECOGNIZER_H
#define FOTOPRIV_USERRECOGNIZER_H
class UserRecognizer {
    private:
    string _csvfilename
    string _image;
    vector<Mat> images;
    vector<int> labels;
    void readCsv(char separator);
    Mat normalizeImage(InputArray _src);

    public:
    UserRecognizer(string image);

    void registerUser();
    void recognize();
    void updateUser();
}
#endif //FOTOPRIV_USERRECOGNIZER_H
