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

class EigenMatcher {
private:
    int _num_components;
    double _threshold;
    vector<Mat> _projections;
    Mat _labels;
    Mat _eigenvectors;
    Mat _eigenvalues;
    Mat _mean;

public:
    EigenMatcher(int num_components = 0, double threshold = DBL_MAX) : _num_components(num_components),
                                                                       _threshold(threshold) {
    }

    Mat getMean() {
        return _mean;
    }

    Mat getEigenVectors() {
        return _eigenvectors;
    }

    void train(InputArrayOfArrays _src, InputArray _local_labels) {
        if (_src.total() == 0) {
            string error_message = format("Empty training data was given. You'll need more than one sample to learn a model.");
            CV_Error(CV_StsBadArg, error_message);
        } else if (_local_labels.getMat().type() != CV_32SC1) {
            string error_message = format("Labels must be given as integer (CV_32SC1). Expected %d, but was %d.", CV_32SC1, _local_labels.type());
            CV_Error(CV_StsBadArg, error_message);
        }
        // make sure data has correct size
        if (_src.total() > 1) {
            for (int i = 1; i < static_cast<int>(_src.total()); i++) {
                if (_src.getMat(i - 1).total() != _src.getMat(i).total()) {
                    string error_message = format("In the Eigenfaces method all input samples (training images) must be of equal size! Expected %d pixels, but was %d pixels.", _src.getMat(i - 1).total(), _src.getMat(i).total());
                    CV_Error(CV_StsUnsupportedFormat, error_message);
                }
            }
        }
        // get labels
        Mat labels = _local_labels.getMat();
        // observations in row
        Mat data = Helper::asRowMatrix(_src, CV_64FC1);

        // number of samples
        int n = data.rows;
        // assert there are as much samples as labels
        if (static_cast<int>(labels.total()) != n) {
            string error_message = format("The number of samples (src) must equal the number of labels (labels)! len(src)=%d, len(labels)=%d.", n, labels.total());
            CV_Error(CV_StsBadArg, error_message);
        }
        // clear existing model data
        _labels.release();
        _projections.clear();
        // clip number of components to be valid
        if ((_num_components <= 0) || (_num_components > n))
            _num_components = n;

        // perform the PCA
        PCA pca(data, Mat(), CV_PCA_DATA_AS_ROW, _num_components);
        // copy the PCA results
        _mean = pca.mean.reshape(1, 1); // store the mean vector
        _eigenvalues = pca.eigenvalues.clone(); // eigenvalues by row
        transpose(pca.eigenvectors, _eigenvectors); // eigenvectors by column
        // store labels for prediction
        _labels = labels.clone();
        // save projections
        for (int sampleIdx = 0; sampleIdx < data.rows; sampleIdx++) {
            Mat p = subspaceProject(_eigenvectors, _mean, data.row(sampleIdx));
            _projections.push_back(p);
        }
    }

    void predict(InputArray _src, int &minClass, double &minDist) const {
        // get data
        Mat src = _src.getMat();
        // make sure the user is passing correct data
        if (_projections.empty()) {
            // throw error if no data (or simply return -1?)
            string error_message = "This Eigenfaces model is not computed yet. Did you call Eigenfaces::train?";
            CV_Error(CV_StsError, error_message);
        } else if (_eigenvectors.rows != static_cast<int>(src.total())) {
            // check data alignment just for clearer exception messages
            string error_message = format("Wrong input image size. Reason: Training and Test images must be of equal size! Expected an image with %d elements, but got %d.", _eigenvectors.rows, src.total());
            CV_Error(CV_StsBadArg, error_message);
        }
        // project into PCA subspace
        Mat q = subspaceProject(_eigenvectors, _mean, src.reshape(1, 1));
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
};