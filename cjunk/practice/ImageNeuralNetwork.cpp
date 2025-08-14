// OpenCV-based image neural network demo
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <algorithm>
#include <numeric>
#include <sstream>

#include <opencv2/opencv.hpp>
#include <opencv2/ml.hpp>

// Simple image loader and preprocessor
class ImageProcessor {
public:
    static cv::Mat loadAndPreprocess(const std::string& filename, int target_size = 28) {
        cv::Mat image = cv::imread(filename, cv::IMREAD_GRAYSCALE);
        if (image.empty()) {
            throw std::runtime_error("Cannot load image: " + filename);
        }
        
        // Resize to target size
        cv::Mat resized;
        cv::resize(image, resized, cv::Size(target_size, target_size));
        
        // Normalize to [0, 1]
        cv::Mat normalized;
        resized.convertTo(normalized, CV_32F, 1.0/255.0);
        
        return normalized;
    }
    
    static cv::Mat generateSyntheticImage(int digit, int size = 28) {
        cv::Mat image = cv::Mat::zeros(size, size, CV_32F);
        
        // Generate simple synthetic patterns for digits 0-9
        switch (digit % 10) {
            case 0: // Circle
                cv::circle(image, cv::Point(size/2, size/2), size/3, cv::Scalar(1.0), -1);
                cv::circle(image, cv::Point(size/2, size/2), size/4, cv::Scalar(0.0), -1);
                break;
            case 1: // Vertical line
                cv::line(image, cv::Point(size/2, size/4), cv::Point(size/2, 3*size/4), cv::Scalar(1.0), 3);
                break;
            case 2: // Horizontal lines
                cv::line(image, cv::Point(size/4, size/3), cv::Point(3*size/4, size/3), cv::Scalar(1.0), 2);
                cv::line(image, cv::Point(size/4, 2*size/3), cv::Point(3*size/4, 2*size/3), cv::Scalar(1.0), 2);
                break;
            case 3: // Three horizontal lines
                cv::line(image, cv::Point(size/4, size/4), cv::Point(3*size/4, size/4), cv::Scalar(1.0), 2);
                cv::line(image, cv::Point(size/4, size/2), cv::Point(3*size/4, size/2), cv::Scalar(1.0), 2);
                cv::line(image, cv::Point(size/4, 3*size/4), cv::Point(3*size/4, 3*size/4), cv::Scalar(1.0), 2);
                break;
            default: // Cross pattern
                cv::line(image, cv::Point(size/4, size/4), cv::Point(3*size/4, 3*size/4), cv::Scalar(1.0), 2);
                cv::line(image, cv::Point(3*size/4, size/4), cv::Point(size/4, 3*size/4), cv::Scalar(1.0), 2);
                break;
        }
        
        return image;
    }
};

// Dataset manager for synthetic or real data
class DataSet {
public:
    void generateSyntheticData(int samples_per_class = 50, int num_classes = 5) {
        images.clear();
        labels.clear();
        
        for (int digit = 0; digit < num_classes; ++digit) {
            for (int sample = 0; sample < samples_per_class; ++sample) {
                cv::Mat image = ImageProcessor::generateSyntheticImage(digit);
                
                // Add some noise for variety
                cv::Mat noise(image.size(), CV_32F);
                cv::randu(noise, -0.1, 0.1);
                image += noise;
                
                images.push_back(image);
                labels.push_back(digit);
            }
        }
        
        std::cout << "Generated " << images.size() << " synthetic images for " 
                  << num_classes << " classes" << std::endl;
    }
    
    void loadFromFiles(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            // If file doesn't exist, generate synthetic data instead
            std::cout << "File not found: " << filename << ", generating synthetic data" << std::endl;
            generateSyntheticData();
            return;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string label_str, image_filename;
            ss >> label_str >> image_filename;
            
            try {
                int label = std::stoi(label_str);
                cv::Mat image = ImageProcessor::loadAndPreprocess(image_filename);
                
                labels.push_back(label);
                images.push_back(image);
            } catch (const std::exception& e) {
                std::cerr << "Error processing line: " << line << " - " << e.what() << std::endl;
            }
        }
        
        if (images.empty()) {
            std::cout << "No valid images loaded, generating synthetic data" << std::endl;
            generateSyntheticData();
        }
    }
    
    std::vector<int> labels;
    std::vector<cv::Mat> images;
};

// Neural network wrapper using OpenCV ML
class NeuralNetwork {
public:
    NeuralNetwork(const std::vector<int>& layer_sizes) {
        network = cv::ml::ANN_MLP::create();
        
        cv::Mat layers = cv::Mat(1, static_cast<int>(layer_sizes.size()), CV_32S);
        for (size_t i = 0; i < layer_sizes.size(); ++i) {
            layers.at<int>(0, static_cast<int>(i)) = layer_sizes[i];
        }
        
        network->setLayerSizes(layers);
        network->setActivationFunction(cv::ml::ANN_MLP::SIGMOID_SYM, 1.0, 1.0);
        network->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER + cv::TermCriteria::EPS, 1000, 0.01));
        network->setTrainMethod(cv::ml::ANN_MLP::BACKPROP, 0.1, 0.1);
    }
    
    void train(const DataSet& data) {
        if (data.images.empty()) {
            throw std::runtime_error("No training data available");
        }
        
        // Prepare input matrix
        int input_size = data.images[0].total();
        cv::Mat inputs(static_cast<int>(data.images.size()), input_size, CV_32F);
        
        for (size_t i = 0; i < data.images.size(); ++i) {
            cv::Mat flattened = data.images[i].reshape(1, 1);
            flattened.copyTo(inputs.row(static_cast<int>(i)));
        }
        
        // Prepare output matrix (one-hot encoding)
        int num_classes = *std::max_element(data.labels.begin(), data.labels.end()) + 1;
        cv::Mat outputs = cv::Mat::zeros(static_cast<int>(data.labels.size()), num_classes, CV_32F);
        
        for (size_t i = 0; i < data.labels.size(); ++i) {
            outputs.at<float>(static_cast<int>(i), data.labels[i]) = 1.0f;
        }
        
        std::cout << "Training neural network..." << std::endl;
        std::cout << "Input shape: " << inputs.size() << std::endl;
        std::cout << "Output shape: " << outputs.size() << std::endl;
        
        bool success = network->train(inputs, cv::ml::ROW_SAMPLE, outputs);
        if (!success) {
            throw std::runtime_error("Neural network training failed");
        }
        
        std::cout << "Training completed successfully" << std::endl;
    }
    
    int predict(const cv::Mat& image) {
        cv::Mat input = image.reshape(1, 1);
        cv::Mat output;
        
        network->predict(input, output);
        
        // Find the class with highest probability
        cv::Point max_loc;
        cv::minMaxLoc(output, nullptr, nullptr, nullptr, &max_loc);
        
        return max_loc.x;
    }
    
    double evaluate(const DataSet& test_data) {
        if (test_data.images.empty()) {
            return 0.0;
        }
        
        int correct = 0;
        for (size_t i = 0; i < test_data.images.size(); ++i) {
            int predicted = predict(test_data.images[i]);
            if (predicted == test_data.labels[i]) {
                correct++;
            }
        }
        
        return static_cast<double>(correct) / test_data.images.size();
    }

private:
    cv::Ptr<cv::ml::ANN_MLP> network;
};

// Demo function
void demo_opencv_neural_network() {
    std::cout << "=== OpenCV Neural Network Demo ===" << std::endl;
    
    try {
        // Create training and test datasets
        DataSet training_data, test_data;
        
        // Generate synthetic data since we don't have real image files
        training_data.generateSyntheticData(100, 5);  // 100 samples per class, 5 classes
        test_data.generateSyntheticData(20, 5);       // 20 samples per class for testing
        
        // Create neural network (input_size, hidden_layer, output_classes)
        int input_size = training_data.images[0].total();
        NeuralNetwork network({input_size, 50, 5});
        
        // Train the network
        network.train(training_data);
        
        // Evaluate on test data
        double accuracy = network.evaluate(test_data);
        std::cout << "Test accuracy: " << (accuracy * 100.0) << "%" << std::endl;
        
        // Test individual predictions
        std::cout << "\nSample predictions:" << std::endl;
        for (int i = 0; i < std::min(5, static_cast<int>(test_data.images.size())); ++i) {
            int predicted = network.predict(test_data.images[i]);
            int actual = test_data.labels[i];
            
            std::cout << "Sample " << i << ": Predicted=" << predicted 
                      << ", Actual=" << actual 
                      << (predicted == actual ? " ✓" : " ✗") << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

int main(int argc, char** argv) {
    std::cout << "=== Image Neural Network with OpenCV ===" << std::endl;
    
    try {
        // Run the demo with synthetic data
        demo_opencv_neural_network();
        
        // If command line arguments are provided, try to load real data
        if (argc == 3) {
            std::cout << "\n=== Testing with provided data files ===" << std::endl;
            
            DataSet training_data, test_data;
            training_data.loadFromFiles(argv[1]);
            test_data.loadFromFiles(argv[2]);
            
            if (!training_data.images.empty() && !test_data.images.empty()) {
                int input_size = training_data.images[0].total();
                int num_classes = *std::max_element(training_data.labels.begin(), training_data.labels.end()) + 1;
                
                NeuralNetwork network({input_size, 100, num_classes});
                network.train(training_data);
                
                double accuracy = network.evaluate(test_data);
                std::cout << "Accuracy with provided data: " << (accuracy * 100.0) << "%" << std::endl;
            }
        }
        
        std::cout << "\n=== Demo completed successfully ===" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}
