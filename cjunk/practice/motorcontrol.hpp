#include <array>
#include <algorithm>


template <int m, int m>
class Matrix{
    public:
        Matrix(int n, int n) : m_n(n), m_m(m), m_data(n, std::array<int, m>()) {}
        Matrix(int n, int m, int init) : m_n(n), m_m(m), m_data(n, std::array<int, m>()) {
            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < m; ++j) {
                    m_data[i][j] = init;
                }
            }
        }
        Matrix(int n, int m, const std::array<int, m>& data) : m_n(n), m_m(m), m_data(n, std::array<int, m>()) {
            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < m; ++j) {
                    m_data[i][j] = data[i];
                }
            }
        }
        Matrix(const Matrix& other) : m_n(other.m_n), m_m(other.m_m), m_data(other.m_data) {}
        // addition operator
        Matrix& operator+=(const Matrix& other) {
            for (int i = 0; i < m_n; ++i) {
                for (int j = 0; j < m_m; ++j) {
                    m_data[i][j] += other.m_data[i][j];
                }
            }
            return *this;
        }
        // subtraction operator
        Matrix& operator-=(const Matrix& other) {
            for (int i = 0; i < m_n; ++i) {
                for (int j = 0; j < m_m; ++j) {
                    m_data[i][j] -= other.m_data[i][j];
                }
            }
            return *this;
        }
        // multiplication operator
        Matrix& operator*=(const Matrix& other) {
            for (int i = 0; i < m_n; ++i) {
                for (int j = 0; j < m_m; ++j) {
                    m_data[i][j] *= other.m_data[i][j];
                }
            }
            return *this;
        }
};
template <int n>
class KalmanFilter{
    public:
        KalmanFilter(int n, int n) : m_n(n), m_m(n), m_data(n, std::array<int, n>()) {}
        KalmanFilter(int n, int m, int init) : m_n(n), m_m(m), m_data(n, std::array<int, n>()) {
            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < m; ++j) {
                    m_data[i][j] = init;
                }
            }
        }
        void operator()(const Matrix<n, n>& x, const Matrix<n, n>& P, const Matrix<n, n>& F, const Matrix<n, n>& Q, const Matrix<n, n>& H, const Matrix<n, n>& R, const Matrix<n, n>& u, Matrix<n, n>& x_, Matrix<n, n>& P_) {
            x_ = F * x + u;
            P_ = F * P * F.transpose() + Q;
            Matrix<n, n> y = H * x_;
            Matrix<n, n> S = H * P_ * H.transpose() + R;
            Matrix<n, n> K = P_ * H.transpose() * S.inverse();
            x_ = x_ + K * (y - H * x_);
            P_ = (Matrix<n, n>::Identity() - K * H) * P_;
        }
    private:
        int m_n, m_m;
        std::array<std::array<int, m_n>, m_n> m_data;
};
// field oriented control
class ClarkeTransform{
    public:
        ClarkeTransform(int n, int n) : m_n(n), m_m(n), m_data(n, std::array<int, n>()) {}
        ClarkeTransform(int n, int m, int init) : m_n(n), m_m(m), m_data(n, std::array<int, n>()) {
            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < m; ++j) {
                    m_data[i][j] = init;
                }
            }
        }
        void operator()(const Matrix<n, n>& x, const Matrix<n, n>& P, const Matrix<n, n>& F, const Matrix<n, n>& Q, const Matrix<n, n>& H, const Matrix<n, n>& R, const Matrix<n, n>& u, Matrix<n, n>& x_, Matrix<n, n>& P_) {
            x_ = F * x + u;
            P_ = F * P * F.transpose() + Q;
            Matrix<n, n> y = H * x_;
            Matrix<n, n> S = H * P_ * H.transpose() + R;
            Matrix<n, n> K = P_ * H.transpose() * S.inverse();
            x_ = x_ + K * (y - H * x_);
            P_ = (Matrix<n, n>::Identity() - K * H) * P_;
        }
    private:
        int m_n, m_m;
        std::array<std::array<int, m_n>, m_n> m_data;
};

void test_filter() {
    KalmanFilter<3> kalman_filter(3, 3, 0);
    Matrix<3, 3> x(3, 3, 0);
    Matrix<3, 3> P(3, 3, 0);
    Matrix<3, 3> F(3, 3, 0);
    Matrix<3, 3> Q(3, 3, 0);
    Matrix<3, 3> H(3, 3, 0);
    Matrix<3, 3> R(3, 3, 0);
    Matrix<3, 3> u(3, 3, 0);
    Matrix<3, 3> x_(3, 3, 0);
    Matrix<3, 3> P_(3, 3, 0);
    kalman_filter(x, P, F, Q, H, R, u, x_, P_);
}
class ParkTransform{
    public:
        ParkTransform(int n, int n) : m_n(n), m_m(n), m_data(n, std::array<int, n>()) {}
        ParkTransform(int n, int m, int init) : m_n(n), m_m(m), m_data(n, std::array<int, n>()) {
            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < m; ++j) {
                    m_data[i][j] = init;
                }
            }
        }
        void operator()(const Matrix<n, n>& x, const Matrix<n, n>& P, const Matrix<n, n>& F, const Matrix<n, n>& Q, const Matrix<n, n>& H, const Matrix<n, n>& R, const Matrix<n, n>& u, Matrix<n, n>& x_, Matrix<n, n>& P_) {
            x_ = F * x + u;
            P_ = F * P * F.transpose() + Q;
            Matrix<n, n> y = H * x_;
            Matrix<n, n> S = H * P_ * H.transpose() + R;
            Matrix<n, n> K = P_ * H.transpose() * S.inverse();
            x_ = x_ + K * (y - H * x_);
            P_ = (Matrix<n, n>::Identity() - K * H) * P_;
        }
    private:
        int m_n, m_m;
        std::array<std::array<int, m_n>, m_n> m_data;
};

void test_park() {
    ParkTransform<3> park_transform(3, 3, 0);
    Matrix<3, 3> x(3, 3, 0);
    Matrix<3, 3> P(3, 3, 0);
    Matrix<3, 3> F(3, 3, 0);
    Matrix<3, 3> Q(3, 3, 0);
    Matrix<3, 3> H(3, 3, 0);
    Matrix<3, 3> R(3, 3, 0);
    Matrix<3, 3> u(3, 3, 0);
    Matrix<3, 3> x_(3, 3, 0);
    Matrix<3, 3> P_(3, 3, 0);
    park_transform(x, P, F, Q, H, R, u, x_, P_);
}
