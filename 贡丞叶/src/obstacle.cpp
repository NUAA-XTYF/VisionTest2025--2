#include "obstacle.h"

Obstacle::Obstacle(int id, double x, double y, double direction, double width,
                   const std::string& material, bool destructible)
    : Target(id, x, y, direction, width), 
      material_(material), destructible_(destructible), health_(100.0) {}

std::vector<Point> Obstacle::getBounds() const {
    std::vector<Point> bounds(4);
    double half_width = width_ / 2.0;
    
    bounds[0] = {getX() - half_width, getY() - half_width};
    bounds[1] = {getX() + half_width, getY() - half_width};
    bounds[2] = {getX() + half_width, getY() + half_width};
    bounds[3] = {getX() - half_width, getY() + half_width};
    
    return bounds;
}

std::string Obstacle::describe() const {
    return "Obstacle (" + material_ + ") at (" + 
           std::to_string(getX()) + ", " + std::to_string(getY()) + 
           "), destructible: " + std::to_string(destructible_);
}

bool Obstacle::takeDamage(double damage) {
    if (!destructible_) return false;
    
    health_ -= damage;
    if (health_ <= 0) {
        health_ = 0;
        return true; // 被摧毁
    }
    return false;
}
