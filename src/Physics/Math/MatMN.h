#ifndef MATMN_H
#define MATMN_H

#include "VecN.h"

struct MatMN {
    int M = 0;
    int N = 0;
    VecN* rows{};
    
    MatMN();
    MatMN(int M, int N);
    MatMN(const MatMN& m);
    ~MatMN();

    void Zero();
    MatMN Transpose() const;
    
    // Override operators
    const MatMN& operator = (const MatMN& m);  // m1 = m2
    VecN operator * (const VecN& v) const;     // m1 * v
    MatMN operator * (const MatMN& m) const;   // m1 * m2
};


#endif
