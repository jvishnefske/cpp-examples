//
// Created by j on 10/23/21.
//

#ifndef MIDDLEWARE_STUPID_FFT_H
#define MIDDLEWARE_STUPID_FFT_H

#include "fftw3.h"

// a c++ raii class to take the complex fft from a container of doubles
class FFT {
public:
    FFT(const std::vector<double>& data) {
        fftw_complex* in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * data.size());
        fftw_complex* out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * data.size());
        fftw_plan p = fftw_plan_dft_1d(data.size(), in, out, FFTW_FORWARD, FFTW_ESTIMATE);
        for (int i = 0; i < data.size(); i++) {
            in[i][0] = data[i];
            in[i][1] = 0;
        }
        fftw_execute(p);
        fftw_destroy_plan(p);
        fftw_free(in);
        fftw_free(out);
    }
    ~FFT() {
    }
    std::vector<double> get_real() {
        std::vector<double> ret(size());
        for (int i = 0; i < size(); i++) {
            ret[i] = std::real(data[i]);
        }
        return ret;
    }
    std::vector<double> get_imag() {
        std::vector<double> ret(size());
        for (int i = 0; i < size(); i++) {
            ret[i] = std::imag(data[i]);
        }
        return ret;
    }
    std::vector<double> get_abs() {
        std::vector<double> ret(size());
        for (int i = 0; i < size(); i++) {
            ret[i] = std::abs(data[i]);
        }
        return ret;
    }
    std::vector<double> get_phase() {
        std::vector<double> ret(size());
        for (int i = 0; i < size(); i++) {
            ret[i] = std::arg(data[i]);
        }
        return ret;
    }
    std::vector<double> get_real_phase() {
        std::vector<double> ret(size());
        for (int i = 0; i < size(); i++) {
            ret[i] = std::arg(data[i]) * 180 / M_PI;
        }
        return ret;
    }
    std::vector<double> get_imag_phase() {
        std::vector<double> ret(size());
        for (int i = 0; i < size(); i++) {
            ret[i] = std::arg(data[i]) * 180 / M_PI;
        }
        return ret;
    }
    std::vector<double> get_real_phase_unwrapped() {
        std::vector<double> ret(size());
        for (int i = 0; i < size(); i++) {
            ret[i] = std::arg(data[i]) * 180 / M_PI;
            if (ret[i] < 0) {
                ret[i] += 360;
            }
        }
        return ret;
    }
    std::vector<double> get_imag_phase_unwrapped() {
        std::vector<double> ret(size());
        for (int i = 0; i < size(); i++) {
            ret[i] = std::arg(data[i]) * 180 / M_PI;
            if (ret[i] < 0) {
                ret[i] += 360;
            }
        }
        return ret;
    }
    std::vector<double> get_real_phase_unwrapped_deg() {
        std::vector<double> ret(size());
        for (int i = 0; i < size(); i++) {
            ret[i] = std::arg(data[i]) * 180 / M_PI;
        }
        return ret;
    }
    std::vector<double> get_imag_phase_unwrapped_deg() {
        std::vector<double> ret(size());
        for (int i = 0; i < size(); i++) {
            ret[i] = std::arg(data[i]) * 180 / M_PI;
        }
        return ret;
    }
    std::vector<double> get_real_phase_unwrapped_deg_shifted() {
        std::vector<double> ret(size());
        for (int i = 0; i < size(); i++) {
            ret[i] = std::arg(data[i]) * 180 / M_PI;
            if (ret[i] < 0) {
                ret[i] += 360;
            }
            ret[i] -= 180;
        }
        return ret;
    }

#endif //MIDDLEWARE_STUPID_FFT_H
