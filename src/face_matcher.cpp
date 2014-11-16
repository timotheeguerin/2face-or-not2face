#include "face_matcher.h"

using namespace std;

string FaceMatcher::DATA_FILE = "save.xml";

FaceMatcher::FaceMatcher() {
    matcher = new FisherMatcher(4000);
    fullMatcher = new FisherMatcher(4100);
}

FaceMatcher::~FaceMatcher() {
    delete matcher;
    delete fullMatcher;
}

void FaceMatcher::train(string file_path, string exclude) {
    vector<Mat> images;
    vector<int> persons;
    Helper::loadImages(file_path, images, persons, exclude);

    matcher->train(Helper::blurImages(Helper::transformImages(images)), std::vector<int>(persons));
    fullMatcher->train(Helper::blurImages(images), std::vector<int>(persons));
}

int FaceMatcher::predict(string image_filename) {
    Mat testImage = Helper::readImage(image_filename);
    int result = fullMatcher->predict(testImage);
    if (result == -1) {
        matcher->predict(Helper::transformImage(testImage.clone()));
        return matcher->predict(Helper::transformImage(testImage.clone()));
    } else {
        return result;
    }
}

void FaceMatcher::save() {
    cout << "de: " << FaceMatcher::DATA_FILE << endl;
    matcher->save(FaceMatcher::DATA_FILE);
}

void FaceMatcher::load() {
    matcher->load(FaceMatcher::DATA_FILE);
}
