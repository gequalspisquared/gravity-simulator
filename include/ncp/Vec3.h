#ifndef VEC3_H
#define VEC3_H

namespace ncp
{
    class Vec3
    {
    public:
        double x = 0;
        double y = 0;
        double z = 0;

        Vec3();
        Vec3(double x0, double y0, double z0);

        // methods
        double mag2() const;
        double mag() const;
        double dist2(const Vec3 &rhs) const;
        double dist(const Vec3 &rhs) const;

        void normalize();

        // operator overloads
        bool operator==(const Vec3 &rhs) const;
        bool operator!=(const Vec3 &rhs) const;

        double operator*(const Vec3 &rhs) const; // dot product

        void operator+=(const Vec3 &rhs);
        void operator-=(const Vec3 &rhs);
        void operator*=(const double scale);
        void operator/=(const double scale);

        Vec3 operator+(const Vec3 &rhs) const;
        Vec3 operator-(const Vec3 &rhs) const;
        Vec3 operator*(const double scale) const;
        Vec3 operator/(const double scale) const;
    };
}

#endif