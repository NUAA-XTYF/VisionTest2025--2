#include<geometry.hpp>
#include<basic_information.hpp>
#include<cmath>

using namespace RobotSimulation;

//加减乘除
Vector2D Vector2D::operator+(const Vector2D& other) const{
    return Vector2D(x + other.x,y + other.y);
}
Vector2D Vector2D::operator-(const Vector2D& other) const{
    return Vector2D(x - other.x,y - other.y);
}
Vector2D Vector2D::operator*(double scalar) const{
    return Vector2D(x * scalar,y * scalar);
}
Vector2D Vector2D::operator/(double scalar) const{
    if(std::abs(scalar)<1e-12){
        return Vector2D(0.0,0.0);
    }
    return Vector2D(x / scalar,y / scalar);
}

//点乘
double Vector2D::dot(const Vector2D& other) const{
    return x*other.x+y*other.y;
}

//差乘
double Vector2D::cross(const Vector2D& other) const{
    return x*other.x-y*other.y;
}

// 向量长度
double Vector2D::length() const {
    return std::sqrt(x * x + y * y);
}

// 向量长度的平方
double Vector2D::lengthSquared() const {
    return x * x + y * y;
}

// 单位化向量
Vector2D Vector2D::normalized() const {
    double len = length();
    if (len < 1e-12) {
        return Vector2D(0.0, 0.0);
    }
    return Vector2D(x / len, y / len);
}

// 旋转向量
Vector2D Vector2D::rotated(double angle) const {
    double cosA = std::cos(angle);
    double sinA = std::sin(angle);
    return Vector2D(x * cosA - y * sinA, x * sinA + y * cosA);
}

// 距离
double Vector2D::distanceTo(const Vector2D& other) const {
    double dx = x - other.x;
    double dy = y - other.y;
    return std::sqrt(dx * dx + dy * dy);
}

//归一化
double normalizedAngle(double angle){
    const double PI_2 =2*PI;
    angle=fmod(angle,PI_2);
    return angle;
}

// 计算两点间距离
double calculateDistance(const Vector2D& a, const Vector2D& b) {
    return a.distanceTo(b);
}
