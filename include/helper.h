#pragma once

#include <opencv2/core/core.hpp>
#include "opencv2/contrib/contrib.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <fstream>
#include <iostream>
#include <dirent.h>
#include <regex>

using namespace cv;

class Helper {
public:
    static Mat norm_0_255(InputArray _src);

    static Mat asRowMatrix(InputArrayOfArrays src, int rtype, double alpha = 1, double beta = 0);

    static std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);

    static void loadImages(string path, vector<Mat> &images, vector<int> &labels, string testImage);

    static vector<string> listImagesInPath(string path);

    static int getPersonFromFileName(string filename);
};

// Removes duplicate elements in a given vector.
template<typename _Tp>
inline vector<_Tp> remove_dups(const vector<_Tp> &src) {
    typedef typename std::set<_Tp>::const_iterator constSetIterator;
    typedef typename vector<_Tp>::const_iterator constVecIterator;
    std::set<_Tp> set_elems;
    for (constVecIterator it = src.begin(); it != src.end(); ++it)
        set_elems.insert(*it);
    vector<_Tp> elems;
    for (constSetIterator it = set_elems.begin(); it != set_elems.end(); ++it)
        elems.push_back(*it);
    return elems;
}

// Reads a sequence from a FileNode::SEQ with type _Tp into a result vector.
template<typename _Tp>
inline void readFileNodeList(const FileNode &fn, vector<_Tp> &result) {
    if (fn.type() == FileNode::SEQ) {
        for (FileNodeIterator it = fn.begin(); it != fn.end();) {
            _Tp item;
            it >> item;
            result.push_back(item);
        }
    }
}

// Writes the a list of given items to a cv::FileStorage.
template<typename _Tp>
inline void writeFileNodeList(FileStorage &fs, const string &name,
        const vector<_Tp> &items) {
    // typedefs
    typedef typename vector<_Tp>::const_iterator constVecIterator;
    // write the elements in item to fs
    fs << name << "[";
    for (constVecIterator it = items.begin(); it != items.end(); ++it) {
        fs << *it;
    }
    fs << "]";
}