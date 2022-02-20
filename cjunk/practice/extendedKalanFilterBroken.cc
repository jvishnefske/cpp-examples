#include <cmath>
#include <future>
template<typename Model>
class ExtendedKalmanFilter {
public:
    ExtendedKalmanFilter(Model& model) : model_(model) {}
    void predict(double dt) {
        model_.predict(dt);
        model_.update_state();
    }
    void update(const Eigen::VectorXd& z) {
        model_.update(z);
    }
private:
    Model& model_;
};

class ExampleModel {
public:
    ExampleModel() : state_(0.0, 0.0, 0.0) {}
    void predict(double dt) {
        state_ = state_ + dt * state_;
    }
    void update(const Eigen::VectorXd& z) {
        state_ = z;
    }
    Eigen::VectorXd state() const {
        return state_;
    }
private:
    Eigen::VectorXd state_;
};

int main() {
    ExampleModel model;
    ExtendedKalmanFilter<ExampleModel> filter(model);
    std::vector<double> measurements = {1.0, 2.0, 3.0};
    for (auto& z : measurements) {
        filter.predict(1.0);
        filter.update(z);
        std::cout << model.state() << std::endl;
    }
}

