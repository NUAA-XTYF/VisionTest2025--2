#include<Target.hpp>
#include<basic_information.hpp>
#include<Object.hpp>

using namespace RobotSimulation;

Target::Target(int id,const Vector2D& pos,double dir,
    double width,double height)
    :Object(id,pos,dir,false),width(width),height(height){}

std::vector<Vector2D> Target::getBounds() const {
    throw std::runtime_error("Pure virtual function called: Target::getBounds()");
    return std::vector<Vector2D>();
}

// 纯虚函数，在派生类中实现
std::string Target::describe() const {
    throw std::runtime_error("Pure virtual function called: Target::describe()");
    return "";
}
bool Target::isHittable(const Vector2D& hitDirection) const {
    throw std::runtime_error("Pure virtual function called: Target::isHittable()");
    return false;
}
void Target::onHit(double damage) {
    throw std::runtime_error("Pure virtual function called: Target::onHit()");
}
