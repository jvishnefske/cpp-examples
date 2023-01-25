// handwriting machine learning model training
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <numeric>
#include <functional>
#include <map>
#include <set>
// class to open image, and normalize it
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/ml/ml.hpp"
#include "opencv2/features2d/features2d.hpp"
class Image {
public:
    Image(const std::string& filename) {
        image = cv::imread(filename, 0);
        if (image.empty()) {
            std::cerr << "Error: cannot open file " << filename << std::endl;
            exit(1);
        }
        cv::resize(image, image, cv::Size(28, 28));
        cv::threshold(image, image, 0, 255, cv::THRESH_BINARY);
    }
    cv::Mat image;
};
// class to store the data
class DataSet {
public:
    DataSet(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: cannot open file " << filename << std::endl;
            exit(1);
        }
        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string label;
            ss >> label;
            labels.push_back(std::stoi(label));
            std::string filename;
            ss >> filename;
            images.push_back(Image(filename));
        }
    }
    std::vector<int> labels;
    std::vector<Image> images;
};
// neural network class
class NeuralNetwork {
public:
    NeuralNetwork(const std::vector<int>& layers) {
        // create the network
        network.setLayerSizes(layers);
        network.setActivationFunction(cv::ml::ANN_MLP::SIGMOID_SYM, 1, 1);
        network.setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER, 100, 0.01));
    }
    void train(const DataSet& data) {
        // prepare the data
        cv::Mat inputs(data.images.size(), data.images[0].image.total(), CV_32FC1);
        cv::Mat outputs(data.labels.size(), 1, CV_32FC1);
        for (int i = 0; i < data.images.size(); i++) {
            inputs.at<float>(i, 0) = data.images[i].image.at<uchar>(0, 0);
            outputs.at<float>(i, 0) = data.labels[i];
        }
        // train the network
        network.train(inputs, cv::ml::ROW_SAMPLE, outputs);
    }
    void predict(const Image& image, int& label) {
        // prepare the data
        cv::Mat inputs(1, image.image.total(), CV_32FC1);
        inputs.at<float>(0, 0) = image.image.at<uchar>(0, 0);
        // predict the label
        cv::Mat outputs = network.predict(inputs);
        label = outputs.at<float>(0, 0);
    }
private:
    cv::ml::ANN_MLP network;
};
// main function
int main(int argc, char** argv) {
    // check the number of arguments
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " train_file test_file" << std::endl;
        return 1;
    }
    // load the data
    DataSet training_data(argv[1]);
    DataSet test_data(argv[2]);
    // create the neural network
    NeuralNetwork network(std::vector<int>{28 * 28, 100, 10});
    // train the network
    network.train(training_data);
    // test the network
    int correct = 0;
    for (int i = 0; i < test_data.images.size(); i++) {
        int label;
        network.predict(test_data.images[i], label);
        if (label == test_data.labels[i]) {
            correct++;
        }
    }
    std::cout << "Accuracy: " << (float)correct / test_data.images.size() << std::endl;
    return 0;
}
