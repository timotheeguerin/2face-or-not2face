#include "face_matcher.h"

using namespace std;

string FaceMatcher::FACE_DATA_FILE = "save-face.xml";
string FaceMatcher::FULL_DATA_FILE = "save-full.xml";

FaceMatcher::FaceMatcher() {
    matcher = new FisherMatcher(4000);
    fullMatcher = new FisherMatcher(4100);
}

FaceMatcher::~FaceMatcher() {
    delete matcher;
    delete fullMatcher;
}

void FaceMatcher::train(string file_path, string exclude) {
    cout << "Training started!" << endl;
    vector<Mat> images;
    vector<int> persons;
    Helper::loadImages(file_path, images, persons, exclude);
    printf("Training %d images\n", (int) images.size());

    matcher->train(Helper::blurImages(Helper::transformImages(images)), std::vector<int>(persons));
    fullMatcher->train(Helper::blurImages(images), std::vector<int>(persons));
    cout << "Training completed!" << endl;
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
    cout << "Saving data: " << endl;
    printf("\t %s\n", FaceMatcher::FACE_DATA_FILE.c_str());
    printf("\t %s\n", FaceMatcher::FULL_DATA_FILE.c_str());
    matcher->save(FaceMatcher::FACE_DATA_FILE);
    matcher->save(FaceMatcher::FULL_DATA_FILE);
    cout << "Saved!" << endl;
}

void FaceMatcher::load() {
    matcher->load(FaceMatcher::FACE_DATA_FILE);
    matcher->load(FaceMatcher::FULL_DATA_FILE);
}
