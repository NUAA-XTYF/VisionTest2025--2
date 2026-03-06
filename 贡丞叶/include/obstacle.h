#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "target.h"
#include <string>

class Obstacle : public Target {
public:
    Obstacle(int id, double x, double y, double direction, double width,
             const std::string& material = "concrete", bool destructible = false);
    virtual ~Obstacle() = default;
    
    std::string getMaterial() const { return material_; }
    bool isDestructible() const { return destructible_; }
    void setHealth(double health) { health_ = health; }
    double getHealth() const { return health_; }
    bool takeDamage(double damage);
    
    virtual std::vector<Point> getBounds() const override;
    virtual std::string describe() const override;
    
private:
    std::string material_;
    bool destructible_;
    double health_;
};

#endif
