#include "helper.h"
#include <libgen.h>
#include <sys/stat.h>

using namespace std;

void Helper::loadImages(string path, vector<Mat> &images, vector<int> &labels, string exclude) {
    vector<string> imageFiles = Helper::listImagesInPath(path);
    for (int i = 0; i != imageFiles.size(); ++i) {
        string image_filename = imageFiles[i];
        if (image_filename != exclude) {
            images.push_back(Helper::readImage(image_filename));
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

Mat Helper::findFace(Mat frame) {
    CascadeClassifier face_cascade;
    if (!face_cascade.load("data/haarcascade_frontalface_alt.xml")) {
        cerr << "Error Haar cascade not found!\n";
        return Mat();
    };

    std::vector<Rect> faces;
    equalizeHist(frame, frame);

    //Find the faces
    face_cascade.detectMultiScale(frame, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));
    Mat face = frame.clone();
    if (faces.size() >= 1) {
        int r = max(faces[0].width, faces[0].height);
        Rect rect = cvRect(faces[0].x, faces[0].y, r, r);
        face = frame(rect);
    }
    Size size(300, 300);
    Mat dst;
    resize(face, dst, size);
    return dst;
}

Mat Helper::readImage(std::string path) {
    std::string filename;
    if (regex_match(path, std::regex("(.*)\\.gif"))) {
        filename = convertGif(path);
    } else {
        filename = path;
    }
    Mat src = imread(filename, 0);
    Mat dst = src.clone();
    return dst;
}


vector<Mat> Helper::blurImages(vector<Mat> srcs) {
    vector<Mat> dsts;
    for (int i = 0; i != srcs.size(); ++i) {
        Mat dst;
        blur(srcs[i], dst, Size(3, 3));
        dsts.push_back(dst);
    }
    return dsts;
}


Mat Helper::transformImage(Mat src) {
    return findFace(src.clone());
}


vector<Mat> Helper::transformImages(vector<Mat> srcs) {
    vector<Mat> dsts;
    for (int i = 0; i != srcs.size(); ++i) {
        dsts.push_back(transformImage(srcs[i]));
    }
    return dsts;
}

std::string Helper::convertGif(std::string path) {
    std::string file = std::string(dirname((char *) path.c_str())) + "/" +
            std::string(basename((char *) path.c_str())) + ".jpg";
    std::string command = "convert " + path + " " + file;
    system(command.c_str());
    return file;
}

bool Helper::fileExists(const std::string &name) {
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}
