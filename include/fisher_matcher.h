#pragma once

#include <opencv2/core/core.hpp>
#include "opencv2/contrib/contrib.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <fstream>
#include <iostream>
#include <dirent.h>
#include <regex>
#include "helper.h"

using namespace cv;
using namespace std;

class FisherMatcher {
private:
    int _num_components;
    double _threshold;
    Mat _eigenvectors;
    Mat _eigenvalues;
    Mat _mean;
    vector<Mat> _projections;
    Mat _labels;

public:
    FisherMatcher(int num_components = 0, double threshold = DBL_MAX) : _num_components(num_components),
                                                                         _threshold(threshold) {
    }

    Mat getMean() {
        return _mean;
    }

    Mat getEigenVectors() {
        return _eigenvectors;
    }

    void train(InputArrayOfArrays src, InputArray _lbls) {
        if (src.total() == 0) {
            string error_message = format("Empty training data was given. You'll need more than one sample to learn a model.");
            CV_Error(CV_StsBadArg, error_message);
        } else if (_lbls.getMat().type() != CV_32SC1) {
            string error_message = format("Labels must be given as integer (CV_32SC1). Expected %d, but was %d.", CV_32SC1, _lbls.type());
            CV_Error(CV_StsBadArg, error_message);
        }
        // make sure data has correct size
        if (src.total() > 1) {
            for (int i = 1; i < static_cast<int>(src.total()); i++) {
                if (src.getMat(i - 1).total() != src.getMat(i).total()) {
                    string error_message = format("In the Fisherfaces method all input samples (training images) must be of equal size! Expected %d pixels, but was %d pixels.", src.getMat(i - 1).total(), src.getMat(i).total());
                    CV_Error(CV_StsUnsupportedFormat, error_message);
                }
            }
        }
        // get data
        Mat labels = _lbls.getMat();
        Mat data = Helper::asRowMatrix(src, CV_64FC1);
        // number of samples
        int N = data.rows;
        // make sure labels are passed in correct shape
        if (labels.total() != (size_t) N) {
            string error_message = format("The number of samples (src) must equal the number of labels (labels)! len(src)=%d, len(labels)=%d.", N, labels.total());
            CV_Error(CV_StsBadArg, error_message);
        } else if (labels.rows != 1 && labels.cols != 1) {
            string error_message = format("Expected the labels in a matrix with one row or column! Given dimensions are rows=%s, cols=%d.", labels.rows, labels.cols);
            CV_Error(CV_StsBadArg, error_message);
        }
        // clear existing model data
        _labels.release();
        _projections.clear();
        // safely copy from cv::Mat to std::vector
        vector<int> ll;
        for (unsigned int i = 0; i < labels.total(); i++) {
            ll.push_back(labels.at<int>(i));
        }
        // get the number of unique classes
        int C = (int) remove_dups(ll).size();
        // clip number of components to be a valid number
        if ((_num_components <= 0) || (_num_components > (C - 1)))
            _num_components = (C - 1);
        // perform a PCA and keep (N-C) components
        PCA pca(data, Mat(), CV_PCA_DATA_AS_ROW, (N - C));
        // project the data and perform a LDA on it
        LDA lda(pca.project(data), labels, _num_components);
        // store the total mean vector
        _mean = pca.mean.reshape(1, 1);
        // store labels
        _labels = labels.clone();
        // store the eigenvalues of the discriminants
        lda.eigenvalues().convertTo(_eigenvalues, CV_64FC1);
        // Now calculate the projection matrix as pca.eigenvectors * lda.eigenvectors.
        // Note: OpenCV stores the eigenvectors by row, so we need to transpose it!
        gemm(pca.eigenvectors, lda.eigenvectors(), 1.0, Mat(), 0.0, _eigenvectors, GEMM_1_T);
        // store the projections of the original data
        for (int sampleIdx = 0; sampleIdx < data.rows; sampleIdx++) {
            Mat p = subspaceProject(_eigenvectors, _mean, data.row(sampleIdx));
            _projections.push_back(p);
        }
    }

    void predict(InputArray _src, int &minClass, double &minDist) const {
        Mat src = _src.getMat();
        // check data alignment just for clearer exception messages
        if (_projections.empty()) {
            // throw error if no data (or simply return -1?)
            string error_message = "This Fisherfaces model is not computed yet. Did you call Fisherfaces::train?";
            CV_Error(CV_StsBadArg, error_message);
        } else if (src.total() != (size_t) _eigenvectors.rows) {
            string error_message = format("Wrong input image size. Reason: Training and Test images must be of equal size! Expected an image with %d elements, but got %d.", _eigenvectors.rows, src.total());
            CV_Error(CV_StsBadArg, error_message);
        }
        // project into LDA subspace
        Mat q = subspaceProject(_eigenvectors, _mean, src.reshape(1, 1));
        // find 1-nearest neighbor
        minDist = DBL_MAX;
        minClass = -1;
        for (size_t sampleIdx = 0; sampleIdx < _projections.size(); sampleIdx++) {
            double dist = norm(_projections[sampleIdx], q, NORM_L2);
            if ((dist < minDist) && (dist < _threshold)) {
                minDist = dist;
                minClass = _labels.at<int>((int) sampleIdx);
            }
        }
    }

    int predict(InputArray _src) const {
        int label;
        double dummy;
        predict(_src, label, dummy);
        return label;
    }

    // See FaceRecognizer::load.
    void load(string filename) {
        FileStorage fs(filename, FileStorage::READ);
        //read matrices
        fs["num_components"] >> _num_components;
        fs["mean"] >> _mean;
        fs["eigenvalues"] >> _eigenvalues;
        fs["eigenvectors"] >> _eigenvectors;
        // read sequences
        readFileNodeList(fs["projections"], _projections);
        fs["labels"] >> _labels;
    }

// See FaceRecognizer::save.
    void save(string filename) const {
        FileStorage fs(filename, FileStorage::WRITE);
        // write matrices
        fs << "num_components" << _num_components;
        fs << "mean" << _mean;
        fs << "eigenvalues" << _eigenvalues;
        fs << "eigenvectors" << _eigenvectors;
        // write sequences
        writeFileNodeList(fs, "projections", _projections);
        fs << "labels" << _labels;
    }

};
