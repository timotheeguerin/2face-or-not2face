#include <opencv2/core/core.hpp>

#include "socket.h"
#include "face_matcher.h"


std::string joinArray(int argc, const char *argv[]) {
    std::stringstream stream;
    for (int i = 0; i != argc; ++i) {
        stream << argv[i];
        if (i != argc - 1) {
            stream << " ";
        }
    }
    return stream.str();
}

int main(int argc, const char *argv[]) {

    bool server = (argv[1][0] == '1');
    bool train = false;
    if (train) {
        FaceMatcher matcher;
        cout << "Red" << endl;
        matcher.train("/Users/tim/dev/cpp/2face/imgs/");
        matcher.save();
        cout << "Data set trained" << endl;
    } else {
//        FaceMatcher matcher = FaceMatcher::load();
        cout << "Loaded" << endl;
//        int person = matcher.predict("/Users/tim/dev/cpp/2face/imgs/5_1_.jpg");
//        cout << "Persion: " << person << endl;
    }
    FaceMatcher matcher = FaceMatcher::load();
    cout << "Loaded" << endl;
    int person = matcher.predict("/Users/tim/dev/cpp/2face/imgs/5_1_.jpg");
    cout << "Persion: " << person << endl;

    if (server) {
        listenSocket();
    } else {
        sendMessage(joinArray(argc, argv));
    }
    return 0;
}
