#include "helper.h"


void Helper::loadImages(string path, vector<Mat> &images, vector<int> &labels, string exclude) {
    vector<string> imageFiles = Helper::listImagesInPath(path);
    for (int i = 0; i != imageFiles.size(); ++i) {
        string image_filename = imageFiles[i];
        if (image_filename != exclude) {
            images.push_back(imread(image_filename, 0));
            labels.push_back(Helper::getPersonFromFileName(image_filename));
        }
    }
}

vector<string> Helper::listImagesInPath(string path) {
    DIR *dir;
    vector<string> images;
    struct dirent *ent;
    if ((dir = opendir(path.c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            string full_name = path + ent->d_name;
            if (regex_match(ent->d_name, std::regex("(.*)\\.jpg"))) {
                images.push_back(full_name);
            }
        }
        closedir(dir);
    } else {
/* could not open directory */
        perror("");
    }
    return images;
}

int Helper::getPersonFromFileName(string filename) {
    vector<string> segment;
    vector<string> array;
    split(filename, '/', segment);
    split(segment[segment.size() - 1], '_', array);
    return atoi(array[0].c_str());
}

std::vector<std::string> &Helper::split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


Mat Helper::asRowMatrix(InputArrayOfArrays src, int rtype, double alpha, double beta) {
    // make sure the input data is a vector of matrices or vector of vector
    if (src.kind() != _InputArray::STD_VECTOR_MAT && src.kind() != _InputArray::STD_VECTOR_VECTOR) {
        string error_message = "The data is expected as InputArray::STD_VECTOR_MAT (a std::vector<Mat>) or _InputArray::STD_VECTOR_VECTOR (a std::vector< vector<...> >).";
        CV_Error(CV_StsBadArg, error_message);
    }
    // number of samples
    size_t n = src.total();
    // return empty matrix if no matrices given
    if (n == 0)
        return Mat();
    // dimensionality of (reshaped) samples
    size_t d = src.getMat(0).total();
    // create data matrix
    Mat data((int) n, (int) d, rtype);
    // now copy data
    for (unsigned int i = 0; i < n; i++) {
        // make sure data can be reshaped, throw exception if not!
        if (src.getMat(i).total() != d) {
            string error_message = format("Wrong number of elements in matrix #%d! Expected %d was %d.", i, d, src.getMat(i).total());
            CV_Error(CV_StsBadArg, error_message);
        }
        // get a hold of the current row
        Mat xi = data.row(i);
        // make reshape happy by cloning for non-continuous matrices
        if (src.getMat(i).isContinuous()) {
            src.getMat(i).reshape(1, 1).convertTo(xi, rtype, alpha, beta);
        } else {
            src.getMat(i).clone().reshape(1, 1).convertTo(xi, rtype, alpha, beta);
        }
    }
    return data;
}

Mat Helper::norm_0_255(InputArray _src) {
    Mat src = _src.getMat();
    // Create and return normalized image:
    Mat dst;
    switch (src.channels()) {
        case 1:
            cv::normalize(_src, dst, 0, 255, NORM_MINMAX, CV_8UC1);
            break;
        case 3:
            cv::normalize(_src, dst, 0, 255, NORM_MINMAX, CV_8UC3);
            break;
        default:
            src.copyTo(dst);
            break;
    }
    return dst;
}


