#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "ncp/Vec3.h"

class CTransform
{
public:
    ncp::Vec3 pos = {0.0, 0.0, 0.0};
    ncp::Vec3 rot = {0.0, 0.0, 0.0};
    ncp::Vec3 vel = {0.0, 0.0, 0.0};

    CTransform(const ncp::Vec3 &pos, const ncp::Vec3 &vel, const ncp::Vec3 &rot)
        : pos(pos)
        , vel(vel)
        , rot(rot)
    {
    }
};

class CGravity
{
public:
    double mass = 0.0;

    CGravity(const float mass)
        : mass(mass)
    {
    }
};

#endif