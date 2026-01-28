#include "energy_mechanism.h"
#include <cmath>

EnergyMechanism::EnergyMechanism(int id, double x, double y, double direction, double width,
                                 double damage_boost, double boost_duration)
    : Target(id, x, y, direction, width), 
      active_(false), damage_boost_(damage_boost), 
      boost_duration_(boost_duration) {}

void EnergyMechanism::activate() {
    active_ = true;
    activation_time_ = std::chrono::steady_clock::now();
}

double EnergyMechanism::getRemainingTime() const {
    if (!active_) return 0.0;
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration<double>(now - activation_time_).count();
    return std::max(0.0, boost_duration_ - elapsed);
}

std::vector<Point> EnergyMechanism::getBounds() const {
    std::vector<Point> bounds(4);
    double half_width = width_ / 2.0;
    
    bounds[0] = {getX() - half_width, getY() - half_width};
    bounds[1] = {getX() + half_width, getY() - half_width};
    bounds[2] = {getX() + half_width, getY() + half_width};
    bounds[3] = {getX() - half_width, getY() + half_width};
    
    return bounds;
}

std::string EnergyMechanism::describe() const {
    return "EnergyMechanism at (" + std::to_string(getX()) + ", " + 
           std::to_string(getY()) + "), active: " + 
           std::to_string(active_) + ", boost: " + 
           std::to_string(damage_boost_);
}

bool EnergyMechanism::isHitFromOutside(double hit_direction) const {
    return true; // 能量机关双面可击
}
