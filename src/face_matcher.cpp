#include "face_matcher.h"

using namespace std;

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

    matcher->train(images, std::vector<int>(persons));
}

int FaceMatcher::predict(string image_filename) {
    Mat testImage = Helper::readImage(image_filename);
    int result = matcher->predict(testImage);
    return result;
}

void FaceMatcher::save() {
    cout << "de: " << FaceMatcher::DATA_FILE << endl;
    matcher->save(FaceMatcher::DATA_FILE);
}

void FaceMatcher::load() {
    matcher->load(FaceMatcher::DATA_FILE);
}
