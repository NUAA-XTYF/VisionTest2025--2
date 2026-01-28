#include "armor_plate.h"
#include "robot.h"
#include "utils.h"
#include <cmath>

ArmorPlate::ArmorPlate(int id, double x, double y, double direction, double width,
                       ObjectColor color, int number, ArmorSide side, 
                       std::weak_ptr<Robot> host)
    : Target(id, x, y, direction, width), ColoredObject(color),
      number_(number), side_(side), host_(host) {}

std::vector<Point> ArmorPlate::getBounds() const {
    std::vector<Point> bounds(4);
    double half_width = width_ / 2.0;
    
    bounds[0] = {getX() - half_width, getY() - half_width};
    bounds[1] = {getX() + half_width, getY() - half_width};
    bounds[2] = {getX() + half_width, getY() + half_width};
    bounds[3] = {getX() - half_width, getY() + half_width};
    
    return bounds;
}

std::string ArmorPlate::describe() const {
    return "ArmorPlate " + std::to_string(number_) + 
           " (" + sideToString(side_) + ") " + 
           colorToString(getColor());
}

bool ArmorPlate::isHitFromOutside(double hit_direction) const {
    double armor_normal = getDirection() + 1.5707963267948966; // π/2
    double direction_diff = hit_direction - armor_normal;
    
    while (direction_diff > 3.14159265358979323846) direction_diff -= 6.28318530717958647692; // 2π
    while (direction_diff < -3.14159265358979323846) direction_diff += 6.28318530717958647692; // 2π
    
    return std::cos(direction_diff) < 0;
}
