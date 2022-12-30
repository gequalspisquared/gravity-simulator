#include <math.h>

#include "ncp/Vec3.h"

using namespace ncp;

Vec3::Vec3() {}

Vec3::Vec3(double x0, double y0, double z0)
    : x(x0)
    , y(y0)
    , z(z0)
{
}

double Vec3::mag2() const
{
    return x*x + y*y + z*z;
}

double Vec3::mag() const
{
    return sqrt(mag2());
}

double Vec3::dist2(const Vec3 &rhs) const
{
    double dx = x - rhs.x;
    double dy = y - rhs.y;
    double dz = z - rhs.z;
    return dx*dx + dy*dy + dz*dz;
}

double Vec3::dist(const Vec3 &rhs) const
{
    return sqrt(dist2(rhs));
}

void Vec3::normalize()
{
    const double length = mag();
    x /= length;
    y /= length;
    z /= length;
}

bool Vec3::operator==(const Vec3 &rhs) const
{
    return x == rhs.x && y == rhs.y && z == rhs.z;
}

bool Vec3::operator!=(const Vec3 &rhs) const
{
    return x != rhs.x || y != rhs.y || z != rhs.z;
}

double Vec3::operator*(const Vec3 &rhs) const
{
    return x*rhs.x + y*rhs.y + z*rhs.z;
}

void Vec3::operator+=(const Vec3 &rhs)
{
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
}

void Vec3::operator-=(const Vec3 &rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
}

void Vec3::operator*=(const double scale)
{
    x *= scale; 
    y *= scale; 
    z *= scale; 
}

void Vec3::operator/=(const double scale)
{
    x /= scale;
    y /= scale;
    z /= scale;
}

Vec3 Vec3::operator+(const Vec3 &rhs) const
{
    return Vec3(x + rhs.x, y + rhs.y, z + rhs.z);
}

Vec3 Vec3::operator-(const Vec3 &rhs) const
{
    return Vec3(x - rhs.x, y - rhs.y, z - rhs.z);
}

Vec3 Vec3::operator*(const double scale) const
{
    return Vec3(x * scale, y * scale, z * scale);
}

Vec3 Vec3::operator/(const double scale) const
{
    return Vec3(x / scale, y / scale, z / scale);
}