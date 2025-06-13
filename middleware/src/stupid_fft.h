//
// Created by j on 10/23/21.
//

#ifndef MIDDLEWARE_STUPID_FFT_H
#define MIDDLEWARE_STUPID_FFT_H

#include "fftw3.h"
#include <vector>
#include <cmath>
#include <complex> // Required for std::complex, std::real, std::imag, std::abs, std::arg

// a c++ raii class to take the complex fft from a container of doubles
class FFT {
public:
    FFT(const std::vector<double>& data) {
        fftw_complex* in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * data.size());
        fftw_complex* out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * data.size());
        fftw_plan p = fftw_plan_dft_1d(data.size(), in, out, FFTW_FORWARD, FFTW_ESTIMATE);
        for (size_t i = 0; i < data.size(); ++i) {
            in[i][0] = data[i];
            in[i][1] = 0;
        }
        fftw_execute(p);
        fftw_destroy_plan(p);
        fftw_free(in);

        // Store the results in a std::vector<std::complex<double>>
        fft_size_ = data.size();
        fft_result_.resize(fft_size_);
        for (size_t i = 0; i < fft_size_; ++i) {
            fft_result_[i] = std::complex<double>(out[i][0], out[i][1]);
        }
        fftw_free(out); // Free the temporary fftw_complex array
    }
    ~FFT() {
        // fft_result_ (std::vector) handles its own memory, no explicit free needed here.
    }

    size_t size() const {
        return fft_size_;
    }

    std::vector<double> get_real() {
        std::vector<double> ret(fft_size_);
        for (size_t i = 0; i < fft_size_; ++i) {
            ret[i] = std::real(fft_result_[i]);
        }
        return ret;
    }
    std::vector<double> get_imag() {
        std::vector<double> ret(fft_size_);
        for (size_t i = 0; i < fft_size_; ++i) {
            ret[i] = std::imag(fft_result_[i]);
        }
        return ret;
    }
    std::vector<double> get_abs() {
        std::vector<double> ret(fft_size_);
        for (size_t i = 0; i < fft_size_; ++i) {
            ret[i] = std::abs(fft_result_[i]);
        }
        return ret;
    }
    std::vector<double> get_phase() {
        std::vector<double> ret(fft_size_);
        for (size_t i = 0; i < fft_size_; ++i) {
            ret[i] = std::arg(fft_result_[i]);
        }
        return ret;
    }
    std::vector<double> get_real_phase() {
        std::vector<double> ret(fft_size_);
        for (size_t i = 0; i < fft_size_; ++i) {
            ret[i] = std::arg(fft_result_[i]) * 180 / M_PI;
        }
        return ret;
    }
    std::vector<double> get_imag_phase() {
        std::vector<double> ret(fft_size_);
        for (size_t i = 0; i < fft_size_; ++i) {
            ret[i] = std::arg(fft_result_[i]) * 180 / M_PI;
        }
        return ret;
    }
    std::vector<double> get_real_phase_unwrapped() {
        std::vector<double> ret(fft_size_);
        for (size_t i = 0; i < fft_size_; ++i) {
            ret[i] = std::arg(fft_result_[i]) * 180 / M_PI;
            if (ret[i] < 0) {
                ret[i] += 360;
            }
        }
        return ret;
    }
    std::vector<double> get_imag_phase_unwrapped() {
        std::vector<double> ret(fft_size_);
        for (size_t i = 0; i < fft_size_; ++i) {
            ret[i] = std::arg(fft_result_[i]) * 180 / M_PI;
            if (ret[i] < 0) {
                ret[i] += 360;
            }
        }
        return ret;
    }
    std::vector<double> get_real_phase_unwrapped_deg() {
        std::vector<double> ret(fft_size_);
        for (size_t i = 0; i < fft_size_; ++i) {
            ret[i] = std::arg(fft_result_[i]) * 180 / M_PI;
        }
        return ret;
    }
    std::vector<double> get_imag_phase_unwrapped_deg() {
        std::vector<double> ret(fft_size_);
        for (size_t i = 0; i < fft_size_; ++i) {
            ret[i] = std::arg(fft_result_[i]) * 180 / M_PI;
        }
        return ret;
    }
    std::vector<double> get_real_phase_unwrapped_deg_shifted() {
        std::vector<double> ret(fft_size_);
        for (size_t i = 0; i < fft_size_; ++i) {
            ret[i] = std::arg(fft_result_[i]) * 180 / M_PI;
            if (ret[i] < 0) {
                ret[i] += 360;
            }
            ret[i] -= 180;
        }
        return ret;
    }

private:
    std::vector<std::complex<double>> fft_result_;
    size_t fft_size_;
}; // Closing brace and semicolon for the class definition

#endif //MIDDLEWARE_STUPID_FFT_H
