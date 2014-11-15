#include "face_matcher.h"

string FaceMatcher::DATA_FILE = "save.xml";

FaceMatcher::FaceMatcher() {
    matcher = new FisherMatcher();
}

FaceMatcher::~FaceMatcher() {
    delete matcher;
}

void FaceMatcher::train(string file_path, string exclude) {
    vector<Mat> images;
    vector<int> persons;
    Helper::loadImages(file_path, images, persons, exclude);
    matcher->train(images, persons);
}

int FaceMatcher::predict(string image_filename) {
    Mat testImage = imread(image_filename, 0);
    return matcher->predict(testImage);
}

void FaceMatcher::save() {
    cout << "de: " << FaceMatcher::DATA_FILE << endl;
    matcher->save(FaceMatcher::DATA_FILE);
}

FaceMatcher FaceMatcher::load() {
    FaceMatcher face_matcher;
    face_matcher.matcher->load(FaceMatcher::DATA_FILE);
    return face_matcher;
}