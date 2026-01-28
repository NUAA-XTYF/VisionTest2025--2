#ifndef ENERGY_MECHANISM_H
#define ENERGY_MECHANISM_H

#include "target.h"
#include <memory>
#include <chrono>

class EnergyMechanism : public Target {
public:
    EnergyMechanism(int id, double x, double y, double direction, double width,
                    double damage_boost = 2.0, double boost_duration = 5.0);
    virtual ~EnergyMechanism() = default;
    
    bool isActive() const { return active_; }
    void activate();
    double getDamageBoost() const { return damage_boost_; }
    double getRemainingTime() const;
    
    virtual std::vector<Point> getBounds() const override;
    virtual std::string describe() const override;
    
    bool isHitFromOutside(double hit_direction) const;
    
private:
    bool active_;
    double damage_boost_;
    double boost_duration_;
    std::chrono::steady_clock::time_point activation_time_;
};

#endif
