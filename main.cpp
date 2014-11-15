#include <opencv2/core/core.hpp>
#include "opencv2/contrib/contrib.hpp"

#include <fstream>
#include <iostream>
#include <regex>
#include "face_matcher.h"
#include "include/helper.h"
#include "include/fisher_matcher.h"
#include <sys/time.h>

using namespace cv;
using namespace std;


int main(int argc, const char *argv[]) {

    bool train = false;
    if (train) {
        FaceMatcher matcher;
        matcher.train("/Users/tim/dev/cpp/2face/imgs/");
        matcher.save();
        cout << "Data set trained" << endl;
    } else {
        FaceMatcher matcher = FaceMatcher::load();
        cout << "Loaded" << endl;
        int person = matcher.predict("/Users/tim/dev/cpp/2face/imgs/5_1_.jpg");
        cout << "Persion: " << person << endl;
    }

    return 0;
}
