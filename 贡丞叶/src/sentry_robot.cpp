#include "sentry_robot.h"
#include <cmath>

int SentryRobot::total_targets_detected_ = 0;
int SentryRobot::instance_count_ = 0;
std::mutex SentryRobot::static_mutex_;

SentryRobot::SentryRobot(int id, double x, double y, double direction, bool movable,
                         ObjectColor color, double length, double width, double gimbal_direction)
    : Robot(id, x, y, direction, movable, color, length, width, gimbal_direction),
      current_damage_boost_(1.0), damage_boost_end_time_(0.0) {
    
    std::lock_guard<std::mutex> lock(static_mutex_);
    instance_count_++;
    
    ammo_inventory_[AmmoType::SMALL] = 1000;
    ammo_inventory_[AmmoType::LARGE] = 500;
}

SentryRobot::~SentryRobot() {
    std::lock_guard<std::mutex> lock(static_mutex_);
    instance_count_--;
}

void SentryRobot::fire() {
    fire(AmmoType::LARGE);
}

void SentryRobot::fire(AmmoType ammo_type) {
    std::lock_guard<std::mutex> lock(armors_mutex_);
    
    auto it = ammo_inventory_.find(ammo_type);
    if (it != ammo_inventory_.end() && it->second > 0) {
        it->second--;
    }
}

void SentryRobot::move(double length, double direction) {
    double new_x = getX() + length * std::cos(direction);
    double new_y = getY() + length * std::sin(direction);
    setPosition(new_x, new_y);
}

void SentryRobot::changeDirection(double changed_rad) {
    setDirection(getDirection() + changed_rad);
}

void SentryRobot::updateArmors() {
    Robot::updateArmors();
}

std::vector<std::shared_ptr<ArmorPlate>> SentryRobot::getArmors() const {
    std::lock_guard<std::mutex> lock(armors_mutex_);
    return armors_;
}

void SentryRobot::activateDamageBoost(double multiplier, double duration) {
    current_damage_boost_ = multiplier;
    damage_boost_end_time_ = duration;
}

std::ostream& operator<<(std::ostream& os, const SentryRobot& sentry) {
    os << "SentryRobot ID: " << sentry.getId()
       << " Position: (" << sentry.getX() << ", " << sentry.getY() << ")"
       << " Direction: " << sentry.getDirection()
       << " Damage Boost: " << sentry.getDamageBoost()
       << " Targets Detected: " << SentryRobot::getTotalTargetsDetected();
    return os;
}
