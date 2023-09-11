#ifndef VECN_H
#define VECN_H

struct VecN {
    int N = 0;
    float* data = new float[N];
    
    VecN();
    VecN(int N);
    VecN(const VecN& v);
    ~VecN();

    void Zero();
    float Dot(const VecN& v) const;
    
    // Override operators
    float operator [] (const int i) const;
    float& operator [] (const int i);

    VecN& operator = (const VecN& v);
    bool operator == (const VecN& v) const;
    bool operator != (const VecN& v) const;
    
    VecN operator + (const VecN& v) const;
    VecN operator - (const VecN& v) const;
    VecN operator - () const;
    VecN operator * (const float n) const;
    VecN operator / (const float n) const;

    VecN& operator += (const VecN& v);
    VecN& operator -= (const VecN& v);
    VecN operator *= (const float n) const;
    VecN operator /= (const float n) const;
};

#endif
