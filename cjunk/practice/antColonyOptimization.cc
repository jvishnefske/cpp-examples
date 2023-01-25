#include <cmath>
#include <future>
#include <vector>
#include <random>
#include <iostream>
#include <random>
#include <vector>
#include <chrono>
#include <thread>
#include <future>
#include <algorithm>
#include <iterator>
#include <numeric>
#include <functional>

template<typename Model>
class AntColonyOptimization {
public:
    AntColonyOptimization(Model& model, int ants, int iterations, double alpha, double beta, double rho, double q0)
        : model(model), ants(ants), iterations(iterations), alpha(alpha), beta(beta), rho(rho), q0(q0) {}
    void run() {
        std::vector<std::future<std::vector<double> > > futures;
        for (int i = 0; i < ants; ++i) {
            futures.push_back(std::async(std::launch::async, &AntColonyOptimization::ant, this));
        }
        for (auto& f : futures) {
            f.wait();
        }
    }
private:
    Model& model;
    int ants;
    int iterations;
    double alpha;
    double beta;
    double rho;
    double q0;
    std::vector<double> ant() {
        std::vector<double> path = model.get_path();
        std::vector<double> pheromone(path.size(), 1.0);
        double best_distance = std::numeric_limits<double>::max();
        for (int i = 0; i < iterations; ++i) {
            std::vector<double> next_path = model.get_path();
            double next_distance = model.get_distance(next_path);
            if (next_distance < best_distance) {
                path = next_path;
                best_distance = next_distance;
            }
            std::vector<double> delta_pheromone(path.size(), 0.0);
            for (int j = 0; j < path.size() - 1; ++j) {
                delta_pheromone[j] = 1.0 / model.get_distance(path);
            }
            for (int j = 0; j < path.size(); ++j) {
                pheromone[j] = (1.0 - rho) * pheromone[j] + delta_pheromone[j];
            }
            std::vector<double> probability(path.size(), 0.0);
            double sum = 0.0;
            for (int j = 0; j < path.size(); ++j) {
                probability[j] = std::pow(pheromone[j], alpha) * std::pow(model.get_visibility(j), beta);
                sum += probability[j];
            }
            for (int j = 0; j < path.size(); ++j) {
                probability[j] /= sum;
            }
            std::vector<double> cumulative_probability(path.size(), 0.0);
            cumulative_probability[0] = probability[0];
            for (int j = 1; j < path.size(); ++j) {
                cumulative_probability[j] = cumulative_probability[j - 1] + probability[j];
            }
            std::uniform_real_distribution<double> distribution(0.0, 1.0);
            double r = distribution(model.get_random_engine());
            int next = 0;
            for (int j = 0; j < path.size(); ++j) {
                if (r < cumulative_probability[j]) {
                    next = j;
                    break;
                }
            }
            if (next != path.size() - 1) {
                std::swap(path[next], path[next + 1]);
            }
            if (q0 > 0.0) {
                std::uniform_real_distribution<double> distribution(0.0, 1.0);
                double r = distribution(model.get_random_engine());
                if (r < q0) {
                    std::swap(path[0], path[1]);
                }
            }
        }
        return path;
    }
};

//The following code runs the ACO algorithm on the TSP problem.

void test_aco(int ants, int iterations, double alpha, double beta, double rho, double q0) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(2, 20);
    int cities = dis(gen);
    std::cout << "cities: " << cities << std::endl;
    std::uniform_real_distribution<double> dis2(-1.0, 1.0);
    std::vector<std::vector<double>> coordinates(cities);
    for (int i = 0; i < cities; ++i) {
        for (int j = 0; j < 2; ++j) {
            coordinates[i].push_back(dis2(gen));
        }
    }
    std::cout << "coordinates: " << std::endl;
    std::copy(coordinates.begin(), coordinates.end(), std::ostream_iterator<std::vector<double>>(std::cout, "\n"));
    std::cout << std::endl;
    Model model(coordinates);
    AntColonyOptimization aco(model, ants, iterations, alpha, beta, rho, q0);
    auto start = std::chrono::high_resolution_clock::now();
    aco.run();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "duration: " << duration << " microseconds" << std::endl;
    std::cout << "distance: " << model.get_distance(aco.get_path()) << std::endl;
    std::cout << "path: " << std::endl;
    std::copy(aco.get_path().begin(), aco.get_path().end(), std::ostream_iterator<int>(std::cout, " "));
    std::cout << std::endl;
}
int test_aco2(){
    test_aco(10, 100, 1.0, 10.0, 0.5, 0.5);
    test_aco(10, 100, 1.0, 10.0, 0.5, 0.0);
    test_aco(10, 100, 1.0, 10.0, 0.0, 0.5);
    test_aco(10, 100, 1.0, 10.0, 0.0, 0.0);
    test_aco(10, 100, 1.0, 10.0, 0.1, 0.1);
    test_aco(10, 100, 1.0, 10.0, 0.1, 0.0);
    test_aco(10, 100, 1.0, 10.0, 0.0, 0.1);
    test_aco(10, 100, 1.0, 10.0, 0.2, 0.2);
    test_aco(10, 100, 1.0, 10.0, 0.2, 0.0);
    test_aco(10, 100, 1.0, 10.0, 0.0, 0.2);
    test_aco(10, 100, 1.0, 10.0, 0.3, 0.3);
    test_aco(10, 100, 1.0, 10.0, 0.3, 0.0);
    test_aco(10, 100, 1.0, 10.0, 0.0, 0.3);
    // done
    return 0;
}

class Model {
public:
    Model(const std::vector<std::vector<double>>& coordinates)
        : coordinates(coordinates) {}
    std::vector<double> get_path() {
        std::vector<double> path(coordinates.size());
        std::iota(path.begin(), path.end(), 0);
        std::random_shuffle(path.begin(), path.end());
        return path;
    }
    double get_distance(const std::vector<double>& path) {
        double distance = 0.0;
        for (int i = 0; i < path.size() - 1; ++i) {
            distance += get_distance(path[i], path[i + 1]);
        }
        return distance;
    }
    double get_distance(int i, int j) {
        return std::sqrt(std::pow(coordinates[i][0] - coordinates[j][0], 2.0) + std::pow(coordinates[i][1] - coordinates[j][1], 2.0));
    }
    double get_visibility(int i) {
        double visibility = 0.0;
        for (int j = 0; j < coordinates.size(); ++j) {
            if (i != j) {
                visibility += 1.0 / get_distance(i, j);
            }
        }
        return visibility;
    }
private:
    const std::vector<std::vector<double>>& coordinates;
};

