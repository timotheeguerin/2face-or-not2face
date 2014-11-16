#pragma once

#include "vendor/docopt.cpp/docopt.h"

static const char USAGE[] =
        R"(facerecognition.

            Usage:
                facerecognition [--gallery=<gal>] <picture>
                facerecognition (-h | --help)
                facerecognition (-s | --server) [<gallery_path>]
                facerecognition (-t | --test) <gallery_path> <test_path>
                facerecognition --version

            Options:
              -h --help        Show this screen.
              --version        Show version.
              --gallery=<gal>  Manually specify gallery to train on.
              -s --server      Starts the training on the specified gallery.
              -t --test        Run the test on the specified folder.
        )";

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <string>

#include <cstdlib>
#include <cstring>

bool _isQuote(char c);

bool _isEscape(char c);

bool _isEscape(char c);

bool _isWhitespace(char c);

std::vector<std::string> parse(const std::string &args);

void stringToArgcArgv(const std::string &str, int *argc, char ***argv);

/*
 * Usage:
 * int argc;
 * char** argv;
 * stringToArgcArgv("foo bar", &argc, &argv);
 */
void stringToArgcArgv(const std::string &str, int *argc, char ***argv) {
    std::vector<std::string> args = parse(str);

    *argv = (char **) std::malloc(args.size() * sizeof(char *));

    int i = 0;
    for (std::vector<std::string>::iterator it = args.begin();
         it != args.end();
         ++it, ++i) {
        std::string arg = *it;
        (*argv)[i] = (char *) std::malloc((arg.length() + 1) * sizeof(char));
        std::strcpy((*argv)[i], arg.c_str());
    }

    *argc = args.size();
}

std::vector<std::string> parse(const std::string &args) {
    std::stringstream ain(args);    // used to iterate over input string
    ain >> std::noskipws;           // do not skip white spaces
    std::vector<std::string> oargs; // output list of arguments

    std::stringstream currentArg("");
    currentArg >> std::noskipws;

    // current state
    enum State {
        InArg,      // currently scanning an argument
        InArgQuote, // currently scanning an argument (which started with quotes)
        OutOfArg    // currently not scanning an argument
    };
    State currentState = OutOfArg;

    char currentQuoteChar = '\0'; // to distinguish between ' and " quotations
    // this allows to use "foo'bar"

    char c;
    while (!ain.eof() && (ain >> c)) { // iterate char by char

        if (_isQuote(c)) {
            switch (currentState) {
                case OutOfArg:
                    currentArg.str(std::string());
                case InArg:
                    currentState = InArgQuote;
                    currentQuoteChar = c;
                    break;

                case InArgQuote:
                    if (c == currentQuoteChar)
                        currentState = InArg;
                    else
                        currentArg << c;
                    break;
            }

        }
        else if (_isWhitespace(c)) {
            switch (currentState) {
                case InArg:
                    oargs.push_back(currentArg.str());
                    currentState = OutOfArg;
                    break;
                case InArgQuote:
                    currentArg << c;
                    break;
                case OutOfArg:
                    // nothing
                    break;
            }
        }
        else if (_isEscape(c)) {
            switch (currentState) {
                case OutOfArg:
                    currentArg.str(std::string());
                    currentState = InArg;
                case InArg:
                case InArgQuote:
                    if (ain.eof()) {
#ifdef WIN32
            // Windows doesn't care about an escape character at the end.
            // It just adds \ to the arg.
            currentArg << c;
#else
                        throw(std::runtime_error("Found Escape Character at end of file."));
#endif
                    }
                    else {
#ifdef WIN32
            // Windows only escapes the " character.
            // Every other character is just printed and the \ is added itself.
            char c1 = c;
            ain >> c;
            if(c != '\"')
              currentArg << c1; // only ignore \ when next char is "
            ain.unget();
#else
                        ain >> c;
                        currentArg << c;
#endif
                    }
                    break;
            }
        }
        else {
            switch (currentState) {
                case InArg:
                case InArgQuote:
                    currentArg << c;
                    break;

                case OutOfArg:
                    currentArg.str(std::string());
                    currentArg << c;
                    currentState = InArg;
                    break;
            }
        }
    }

    if (currentState == InArg)
        oargs.push_back(currentArg.str());
    else if (currentState == InArgQuote)
        throw(std::runtime_error("Starting quote has no ending quote."));

    return oargs;
}

bool _isQuote(char c) {
    if (c == '\"')
        return true;
    else if (c == '\'')
        return true;

    return false;
}

bool _isEscape(char c) {
    if (c == '\\')
        return true;

    return false;
}

bool _isWhitespace(char c) {
    if (c == ' ')
        return true;
    else if (c == '\t')
        return true;

    return false;
}
std::map<std::string, docopt::value> get_docopt_args(int argc, char *argv[]) {
    return docopt::docopt(USAGE,
            {argv + 1, argv + argc},
            true,               // show help if requested
            "facerecognition 0.0.1");  // version string
}

