#include <algorithm>
#include <numeric>
#include <vector>
#include <random>
#include <cmath>
#include <chrono>
namespace ML{
    using namespace std;
class Perceptron{
    std::vector<double> weights;
    public:
    double bias;
    Perceptron(int inputs, double bias=1.0);
    double run(std::vector<double> x);
    void set_weights(std::vector<double>);
    double sigmoid(double x);
    };
    Perceptron::Perceptron(int inputs, double bias){
        this->bias = bias;
        weights.resize(inputs);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(-1,1);
        std::generate(weights.begin(), weights.end(), 
            [](){return 2.0*(double)(rand())/ RAND_MAX - 1.0;});
        for(int i=0; i<inputs; i++){
            weights[i] = dis(gen);
        }
    }
    double Perceptron::run(std::vector<double> x){
        double sum = 0;
        for(int i=0; i<x.size(); i++){
            sum += x[i]*weights[i];
        }
        return sigmoid(sum+bias);
    }
    void Perceptron::set_weights(std::vector<double> w){
        weights = w;
    }
    double Perceptron::sigmoid(double x){
        return 1/(1+exp(-x));
    }


class Layer{
    std::vector<Perceptron> perceptrons;
    public:
    Layer(int inputs, int outputs);
    std::vector<double> run(std::vector<double> x);
    void set_weights(vector<vector<double>>);
    };
    Layer::Layer(int inputs, int outputs){
        for(int i=0; i<outputs; i++){
            perceptrons.push_back(Perceptron(inputs));
        }
    }
    // no idea if this return type makes sence or what math is done here.
    std::vector<double> Layer::run(std::vector<double> x){
        std::vector<double> output;
        for(int i=0; i<perceptrons.size(); i++){
            output.push_back(perceptrons[i].run(x));
        }
        return output;
    }
    void Layer::set_weights(std::vector<std::vector<double>> w){
        for(int i=0; i<perceptrons.size(); i++){
            perceptrons[i].set_weights(w[i]);
        }
    }


class Network{
    std::vector<Layer> layers;
    public:
    Network(int inputs, int outputs, int hidden);
    std::vector<double> run(std::vector<double> x);
    void set_weights(std::vector<std::vector<std::vector<double>>>);
    };
    Network::Network(int inputs, int outputs, int hidden){
        for(int i=0; i<hidden; i++){
            layers.push_back(Layer(inputs, inputs));
        }
        layers.push_back(Layer(inputs, outputs));
    }
    std::vector<double> Network::run(vector<double> x){
        vector<double> output;
        for(int i=0; i<layers.size(); i++){
            output = layers[i].run(output);
        }
        return output;
    }
    void Network::set_weights(vector<vector<vector<double>>> w){
        for(int i=0; i<layers.size(); i++){
            layers[i].set_weights(w[i]);
        }
    }
}

