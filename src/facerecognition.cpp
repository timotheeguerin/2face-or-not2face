#include "../vendor/docopt.cpp/docopt.h"

#include <iostream>

static const char USAGE[] =
R"(facerecognition.

    Usage:
      facerecognition add gallery <folder>
      facerecognition add person <folder>
      facerecognition purge
      facerecognition [--gallery=<gal>] <picture>
      facerecognition [--smoothing] <picture>
      facerecognition [--verbose] <picture>
      facerecognition (-h | --help)
      facerecognition --version

    Options:
      -h --help        Show this screen.
      --version        Show version.
      --gallery=<gal>  Speed in knots [default: 10].
      --fisher-faces   Use fisher faces algorithm.
      --smoothing      Enable smoothing of the input picture.
      --verbose        Prints more information.
)";

int main(int argc, const char** argv)
{
    std::map<std::string, docopt::value> args = docopt::docopt(USAGE, 
                                                  { argv + 1, argv + argc },
                                                  true,               // show help if requested
                                                  "facerecognition 0.0.1");  // version string

    for(auto const& arg : args) {
        std::cout << arg.first << ": " << arg.second << std::endl;
    }

    if (args["purge"]) {
      std::cout << "Purging...";
    }

    return 0;
}
