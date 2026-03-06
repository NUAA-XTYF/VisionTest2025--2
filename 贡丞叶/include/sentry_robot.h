#ifndef SENTRY_ROBOT_H
#define SENTRY_ROBOT_H

#include "robot.h"
#include "ammo_type.h"
#include <map>
#include <memory>
#include <mutex>
#include <iostream>

class SentryRobot : public Robot {
public:
    SentryRobot(int id, double x, double y, double direction, bool movable,
                ObjectColor color, double length, double width, double gimbal_direction = 0.0);
    ~SentryRobot();
    
    void fire();
    void fire(AmmoType ammo_type);
    
    void move(double length, double direction);
    void changeDirection(double changed_rad);
    
    virtual void updateArmors() override;
    virtual std::vector<std::shared_ptr<ArmorPlate>> getArmors() const override;
    
    void activateDamageBoost(double multiplier, double duration);
    double getDamageBoost() const { return current_damage_boost_; }
    
    static int getTotalTargetsDetected() { 
        std::lock_guard<std::mutex> lock(static_mutex_);
        return total_targets_detected_; 
    }
    static int getInstanceCount() { 
        std::lock_guard<std::mutex> lock(static_mutex_);
        return instance_count_; 
    }
    static void addTargetDetected(int count = 1) { 
        std::lock_guard<std::mutex> lock(static_mutex_);
        total_targets_detected_ += count; 
    }
    
    friend std::ostream& operator<<(std::ostream& os, const SentryRobot& sentry);
    
private:
    static int total_targets_detected_;
    static int instance_count_;
    static std::mutex static_mutex_;
    
    double current_damage_boost_;
    double damage_boost_end_time_;
    
    std::map<AmmoType, int> ammo_inventory_;
    std::vector<std::shared_ptr<ArmorPlate>> armors_;
    mutable std::mutex armors_mutex_;
};

#endif
