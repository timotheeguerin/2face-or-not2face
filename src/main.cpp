#include <opencv2/core/core.hpp>

#include "socket.h"
#include "docopt.h"

std::string joinArray(int argc, char *argv[]) {
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

    std::map<std::string, docopt::value> args = get_docopt_args(argc, argv);

//    for (auto const &arg : args) {
//        std::cout << arg.first << ": " << arg.second << std::endl;
//    }
    if (args["-s"] == docopt::value(true)) {
        std::string gallery_path = "";
        if (args["<gallery_path>"]) {
            gallery_path = args["<gallery_path>"].asString();
        }

        Server server(gallery_path);
        server.listenSocket();
    } else if (args["-t"] == docopt::value(true)) {
        FaceMatcher matcher;
        matcher.train(args["<gallery_path>"].asString());
        std::vector<std::string> images = Helper::listImagesInPath(args["<test_path>"].asString());
        for (int i = 0; i != images.size(); i++) {
            int real = Helper::getPersonFromFileName(images[i]);
            int pred = matcher.predict(images[i]);
            std::cout << "Plan: " << pred << " == " << real << "       | " << (real == pred) << std::endl;
        }
    }
    else {
        std::cout << sendMessage(joinArray(argc, argv)) << std::endl;
    }
    return 0;
}
