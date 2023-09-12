#include "VecN.h"

#include <cmath>

VecN::VecN() : N(0), data(nullptr) {}

VecN::VecN(int N) : N(N) {
    data = new float[N];
}

VecN::VecN(const VecN &v) : N(v.N) {
    data = new float[N];
    for (int i = 0; i < N; i++) {
        data[i] = v.data[i];
    }
}

VecN::~VecN() {
    delete[] data;
}

void VecN::Zero() {
    for (int i = 0; i < N; i++)
        data[i] = 0.0f;
}

float VecN::Dot(const VecN &v) const {
    float result = 0.0f;
    if (N != v.N) return result;
    for (int i = 0; i < N; i++) {
        result += data[i] * v.data[i];
    }
    return result;
}

float VecN::operator[](const int i) const {
    return data[i];
}

float &VecN::operator[](const int i) {
    return data[i];
}

VecN &VecN::operator=(const VecN &v) {
    if (this == &v) return *this;

    delete[] data;
    data = new float[v.N];
    this->N = v.N;
    for (int i = 0; i < N; i++) {
        data[i] = v.data[i];
    }
    return *this;
}

// use limits/epsilon to compare floating point numbers
bool VecN::operator==(const VecN &v) const {
    if (N != v.N) return false;
    for (int i = 0; i < N; i++) {
        if (std::abs(data[i] - v.data[i]) > std::numeric_limits<float>::epsilon()) {
            return false;
        }
    }
    return true;
}

bool VecN::operator!=(const VecN &v) const {
    return !(*this == v);
}

VecN &VecN::operator+=(const VecN &v) {
    if (N != v.N) return *this;
    for (int i = 0; i < N; i++) {
        data[i] += v.data[i];
    }
    return *this;
}

VecN &VecN::operator-=(const VecN &v) {
    if (N != v.N) return *this;
    for (int i = 0; i < N; i++) {
        data[i] -= v.data[i];
    }
    return *this;
}

VecN VecN::operator*=(const float n) const {
    VecN result = *this;
    for (int i = 0; i < N; i++) {
        result.data[i] *= n;
    }
    return result;
}

VecN VecN::operator/=(const float n) const {
    VecN result;
    for (int i = 0; i < N; i++) {
        result.data[i] = data[i] / n;
    }
    return result;
}

VecN VecN::operator+(const VecN &v) const {
    VecN result;
    if (N != v.N) return result;
    for (int i = 0; i < N; i++) {
        result.data[i] = data[i] + v.data[i];
    }
    return result;
}

VecN VecN::operator-(const VecN &v) const {
    VecN result;
    if (N != v.N) return result;
    for (int i = 0; i < N; i++) {
        result.data[i] = data[i] - v.data[i];
    }
    return result;
}

VecN VecN::operator-() const {
    VecN result;
    for (int i = 0; i < N; i++) {
        result.data[i] = -data[i];
    }
    return result;
}

VecN VecN::operator*(const float n) const {
    VecN result = *this;
    return result *= n;
}

VecN VecN::operator/(const float n) const {
    VecN result = *this;
    return result /= n;
}