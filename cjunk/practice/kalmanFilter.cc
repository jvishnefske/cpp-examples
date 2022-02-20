#include <cmath>
#include <future>
template<typename Model>
class KalmanFilter {
public:
    KalmanFilter(Model& model) : model_(model) {}
    void init(const Eigen::VectorXd& x0, const Eigen::MatrixXd& P0) {
        x_ = x0;
        P_ = P0;
    }
    void predict() {
        x_ = model_.f(x_);
        P_ = model_.F(x_) * P_ * model_.F(x_).transpose() + model_.Q();
    }
    void update(const Eigen::VectorXd& z) {
        Eigen::VectorXd y = z - model_.h(x_);
        Eigen::MatrixXd H = model_.H(x_);
        Eigen::MatrixXd S = H * P_ * H.transpose() + model_.R();
        Eigen::MatrixXd K = P_ * H.transpose() * S.inverse();
        x_ = x_ + K * y;
        P_ = (Eigen::MatrixXd::Identity(P_.rows(), P_.cols()) - K * H) * P_;
    }
    Eigen::VectorXd x() const { return x_; }
    Eigen::MatrixXd P() const { return P_; }
private:
    Model& model_;
    Eigen::VectorXd x_;
    Eigen::MatrixXd P_;
};
