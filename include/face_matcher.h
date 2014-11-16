#pragma once

#include "fisher_matcher.h"

using namespace cv;

class FaceMatcher {
private:
    static std::string DATA_FILE;;
    FisherMatcher *matcher;
public:
    FaceMatcher();

    ~FaceMatcher();

    void train(std::string file_path, std::string exclude = "");

    int predict(std::string image_filename);

    void save();

    void load();
};


