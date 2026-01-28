#ifndef ENEMY_ROBOT_H
#define ENEMY_ROBOT_H

#include "robot.h"
#include <memory>
#include <vector>

struct ArmorConfig;

class EnemyRobot : public Robot {
public:
    EnemyRobot(int id, double x, double y, double direction, bool movable,
               ObjectColor color, double length, double width, double gimbal_direction,
               double health, double max_health);
    virtual ~EnemyRobot() = default;
    
    double getHealth() const { return health_; }
    double getMaxHealth() const { return max_health_; }
    bool isAlive() const { return health_ > 0; }
    
    void takeDamage(double damage);
    
    void createArmorsFromConfig(const std::vector<ArmorConfig>& configs);
    virtual void updateArmors() override;
    virtual std::vector<std::shared_ptr<ArmorPlate>> getArmors() const override { return armors_; }
    
private:
    double health_;
    double max_health_;
    std::vector<std::shared_ptr<ArmorPlate>> armors_;
};

#endif
