#include <opencv2/core/core.hpp>

#include "socket.h"
#include "docopt.h"

#include "face_matcher.h"


std::string joinArray(int argc,  char *argv[]) {
    std::stringstream stream;
    for (int i = 0; i != argc; ++i) {
        stream << argv[i];
        if (i != argc - 1) {
            stream << " ";
        }
    }
    return stream.str();
}


int main(int argc, char *argv[]) {

//    bool train = false;
//    if (train) {
//        FaceMatcher matcher;
//        cout << "Red" << endl;
//        matcher.train("/Users/tim/dev/cpp/2face/imgs/");
//        matcher.save();
//        cout << "Data set trained" << endl;
//    } else {
////        FaceMatcher matcher = FaceMatcher::load();
//        cout << "Loaded" << endl;
////        int person = matcher.predict("/Users/tim/dev/cpp/2face/imgs/5_1_.jpg");
////        cout << "Persion: " << person << endl;
//    }
//    FaceMatcher matcher = FaceMatcher::load();
//    cout << "Loaded" << endl;
//    int person = matcher.predict("/Users/tim/dev/cpp/2face/imgs/5_1_.jpg");
//    cout << "Persion: " << person << endl;

    std::map<std::string, docopt::value> args = get_docopt_args(argc, argv);
    if (args["-s"] == docopt::value(true)) {
        std::cout << "Launching server..." << std::endl;
        Server server;
        server.listenSocket();
    } else {
        std::cout << sendMessage(joinArray(argc, argv)) << std::endl;
    }
    return 0;
}
