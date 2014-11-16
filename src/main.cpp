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
        int fail_count = 0;
        int error_count = 0;
        int total = 0;
        for (int i = 0; i != images.size(); i++) {
            int real = Helper::getPersonFromFileName(images[i]);
            int pred = matcher.predict(images[i]);
            printf("%i | %-60s | %d == %d\n", (real == pred), images[i].c_str(), pred, real);
            total++;
            if (pred == -1) {
                fail_count++;
            }
            else if (real != pred) {
                error_count++;
            }
        }
        std::cout << "Number of false positive: " << error_count << std::endl;
        std::cout << "Number of fail: " << fail_count << std::endl;
        float precision = ((float) (total - error_count - fail_count) / total) * 100;
        printf("Precision: %.2f%%", precision);

    }
    else {
        std::cout << sendMessage(joinArray(argc, argv)) << std::endl;
    }
    return 0;
}
