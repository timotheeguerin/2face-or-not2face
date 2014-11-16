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
    int component_number;
    double threshold;
    Mat eigenvectors;
    Mat eigenvalues;
    Mat mean;
    vector<Mat> projections;
    vector<int> persons;

public:
    FisherMatcher(int component_number = 0, double threshold = DBL_MAX) : component_number(component_number),
                                                                          threshold(threshold) {
    }

    Mat getMean() {
        return mean;
    }

    Mat getEigenVectors() {
        return eigenvectors;
    }

    void train(vector<Mat> images, vector<int> lbls) {
        if (images.size() <= 1) {
            cerr << "You need at least 2 images to train.";
            exit(-1);
        } else if (lbls.size() != images.size()) {
            cerr << "The number of labels needs to be the same as the number of images";
            exit(-1);
        }
        // make sure data has correct size
        for (int i = 1; i < images.size(); i++) {
            if (images[i - 1].total() != images[i].total()) {
                cerr << "All images must have the same number of pixel";
                exit(-1);
            }
        }
        this->persons = vector<int>(lbls);
        // Clear previous calculation
        this->projections.clear();

        // Convert the image in the right format for PCA
        Mat data = Helper::asRowMatrix(images, CV_64FC1);

        // Get the number of unique persons
        int unique_person_count = (int) remove_dups(vector<int>(lbls)).size();
        // Update the number of component if overflow
        if ((component_number <= 0) || (component_number > (unique_person_count - 1)))
            component_number = (unique_person_count - 1);
        // Perform the pca
        PCA pca(data, Mat(), CV_PCA_DATA_AS_ROW, (int) (images.size() - unique_person_count));
        // Perform lda on the projected data
        LDA lda(pca.project(data), lbls, component_number);
        // store the total mean vector
        mean = pca.mean.reshape(1, 1);

        // store the eigenvalues of the discriminants
        lda.eigenvalues().convertTo(eigenvalues, CV_64FC1);

        // Calculate the projection matrix
        gemm(pca.eigenvectors, lda.eigenvectors(), 1.0, Mat(), 0.0, eigenvectors, GEMM_1_T);

        // Save the projections
        for (int sampleIdx = 0; sampleIdx < data.rows; sampleIdx++) {
            Mat p = subspaceProject(eigenvectors, mean, data.row(sampleIdx));
            projections.push_back(p);
        }
    }

    void predict(InputArray _src, int &bestMatch, double &bestDistance) const {
        Mat src = _src.getMat();
        // check data alignment just for clearer exception messages
        if (projections.empty()) {
            cerr << "Error no data. Train the program first";
            exit(-1);
        } else if (src.total() != (size_t) eigenvectors.rows) {
            cerr << "Error the image size is not right";
            exit(-1);
        }
        // project into LDA subspace
        Mat q = subspaceProject(eigenvectors, mean, src.reshape(1, 1));

        // Find the best match
        bestDistance = threshold;
        bestMatch = -1;
        for (int i = 0; i != projections.size(); ++i) {
            double distance = norm(projections[i], q, NORM_L2);
            if ((distance < bestDistance)) {
                bestDistance = distance;
                bestMatch = persons[i];
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
        fs["num_components"] >> component_number;
        fs["mean"] >> mean;
        fs["eigenvalues"] >> eigenvalues;
        fs["eigenvectors"] >> eigenvectors;
        // read sequences
        readFileNodeList(fs["projections"], projections);
        fs["labels"] >> persons;
    }

// See FaceRecognizer::save.
    void save(string filename) const {
        FileStorage fs(filename, FileStorage::WRITE);
        // write matrices
        fs << "num_components" << component_number;
        fs << "mean" << mean;
        fs << "eigenvalues" << eigenvalues;
        fs << "eigenvectors" << eigenvectors;
        // write sequences
        writeFileNodeList(fs, "projections", projections);
        fs << "labels" << persons;
    }

};