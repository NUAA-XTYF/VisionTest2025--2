#pragma once

#include<vector>
#include<cmath>
#include<exprtk.hpp>

namespace RobotSimulation{

class Vector2D{
public:
    double x,y;
    Vector2D(double x,double y):x(x),y(y){}

    //加减乘除
    Vector2D operator+(const Vector2D& other) const;
    Vector2D operator-(const Vector2D& other) const;
    Vector2D operator*(double scalar) const;
    Vector2D operator/(double scalar) const;

    //点乘与差乘
    double dot(const Vector2D& other) const;
    double cross(const Vector2D& other) const;

    //长宽
    double length() const;
    double lengthSquared() const;
    Vector2D normalized() const;

    //旋转
    Vector2D rotated(double angle) const;

    //距离
    double distanceTo(const Vector2D& other) const;

};
    //角度，归一化
    double normalizedAngle(double angle);
    double calculateDistance(const Vector2D& a,const Vector2D& b);
}