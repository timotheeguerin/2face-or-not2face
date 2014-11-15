#pragma once

#include "fisher_matcher.h"

using namespace std;
using namespace cv;

class FaceMatcher {
private:
    static string DATA_FILE;;
    FisherMatcher *matcher;
public:
    FaceMatcher();

    ~FaceMatcher();

    void train(string file_path, string exclude = "");

    int predict(string image_filename);

    void save();

    FaceMatcher static load();

};


