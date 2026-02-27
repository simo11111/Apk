#ifndef MATH_UTILS_H
#define MATH_UTILS_H
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define PI 3.14159265f

struct Vec3 {
    float x, y, z;
    Vec3() : x(0), y(0), z(0) {}
    Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
    
    // Ops
    Vec3 operator+(const Vec3& v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
    Vec3 operator-(const Vec3& v) const { return Vec3(x - v.x, y - v.y, z - v.z); }
    Vec3 operator*(float s) const { return Vec3(x * s, y * s, z * s); }
    
    // Assignment Ops
    Vec3& operator+=(const Vec3& v) { x += v.x; y += v.y; z += v.z; return *this; }
    Vec3& operator-=(const Vec3& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
    Vec3& operator*=(float s) { x *= s; y *= s; z *= s; return *this; }

    float length() const { return sqrt(x*x + y*y + z*z); }
    
    void normalize() {
        float len = length();
        if (len > 0.0001f) {
            float invLen = 1.0f / len;
            x *= invLen; y *= invLen; z *= invLen;
        } else {
            x = 0; y = 0; z = 0;
        }
    }
};

inline bool checkCircleCollision(Vec3 p1, float r1, Vec3 p2, float r2) {
    float dx = p1.x - p2.x;
    float dz = p1.z - p2.z;
    float distSq = dx*dx + dz*dz;
    float radSum = r1 + r2;
    return distSq < (radSum * radSum);
}

// Column-Major Order 4x4 Matrix
struct Mat4 {
    float m[16];

    Mat4() { 
        identity();
    }

    void identity() {
        memset(m, 0, 16 * sizeof(float));
        m[0] = 1.0f; m[5] = 1.0f; m[10] = 1.0f; m[15] = 1.0f;
    }

    static void multiply(Mat4& out, const Mat4& a, const Mat4& b) {
        float temp[16];
        for (int c = 0; c < 4; c++) {
            for (int r = 0; r < 4; r++) {
                temp[c * 4 + r] = 
                    a.m[0 * 4 + r] * b.m[c * 4 + 0] +
                    a.m[1 * 4 + r] * b.m[c * 4 + 1] +
                    a.m[2 * 4 + r] * b.m[c * 4 + 2] +
                    a.m[3 * 4 + r] * b.m[c * 4 + 3];
            }
        }
        memcpy(out.m, temp, 16 * sizeof(float));
    }

    static void perspective(Mat4& out, float fov, float aspect, float n, float f) {
        float t = tan(fov * 0.5f);
        float a = 1.0f / t;
        memset(out.m, 0, 16 * sizeof(float));
        out.m[0] = a / aspect;
        out.m[5] = a;
        out.m[10] = (n + f) / (n - f);
        out.m[11] = -1.0f;
        out.m[14] = (2.0f * n * f) / (n - f);
        out.m[15] = 0.0f;
    }

    static void lookAt(Mat4& out, Vec3 eye, Vec3 center, Vec3 up) {
        Vec3 f = center - eye; f.normalize();
        Vec3 s; 
        s.x = f.y*up.z - f.z*up.y; 
        s.y = f.z*up.x - f.x*up.z; 
        s.z = f.x*up.y - f.y*up.x; 
        s.normalize();
        Vec3 u; 
        u.x = s.y*f.z - s.z*f.y; 
        u.y = s.z*f.x - s.x*f.z; 
        u.z = s.x*f.y - s.y*f.x;

        out.m[0]=s.x; out.m[4]=s.y; out.m[8]=s.z; out.m[12]=-(s.x*eye.x+s.y*eye.y+s.z*eye.z);
        out.m[1]=u.x; out.m[5]=u.y; out.m[9]=u.z; out.m[13]=-(u.x*eye.x+u.y*eye.y+u.z*eye.z);
        out.m[2]=-f.x; out.m[6]=-f.y; out.m[10]=-f.z; out.m[14]=(f.x*eye.x+f.y*eye.y+f.z*eye.z);
        out.m[3]=0.0f; out.m[7]=0.0f; out.m[11]=0.0f; out.m[15]=1.0f;
    }

    // Correct Translation Logic
    static void translate(Mat4& inOut, float x, float y, float z) {
        Mat4 t; // Identity
        t.m[12] = x;
        t.m[13] = y;
        t.m[14] = z;
        Mat4 result;
        multiply(result, inOut, t); // Apply translation
        memcpy(inOut.m, result.m, 16 * sizeof(float));
    }

    // Correct Non-Uniform Scaling Logic
    static void scale(Mat4& inOut, float x, float y, float z) {
        Mat4 s; // Identity
        s.m[0] = x;
        s.m[5] = y;
        s.m[10] = z;
        Mat4 result;
        multiply(result, inOut, s); // Apply scaling
        memcpy(inOut.m, result.m, 16 * sizeof(float));
    }
};
#endif

