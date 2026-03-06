#include "enemy_robot.h"
#include "config.h"
#include "utils.h"
#include <memory>
#include <iostream>

EnemyRobot::EnemyRobot(int id, double x, double y, double direction, bool movable,
                       ObjectColor color, double length, double width, double gimbal_direction,
                       double health, double max_health)
    : Robot(id, x, y, direction, movable, color, length, width, gimbal_direction),
      health_(health), max_health_(max_health) {}

void EnemyRobot::takeDamage(double damage) {
    if (damage < 0) {
        std::cerr << "Warning: Negative damage value: " << damage << std::endl;
        return;
    }
    
    health_ -= damage;
    if (health_ < 0) health_ = 0;
}

void EnemyRobot::createArmorsFromConfig(const std::vector<ArmorConfig>& configs) {
    armors_.clear(); // 清空现有装甲板
    
    int armor_id = getId() * 10; // 使用机器人ID作为基础，避免ID冲突
    
    // 获取当前机器人的shared_ptr（通过enable_shared_from_this）
    auto self_ptr = shared_from_this();
    
    for (const auto& config : configs) {
        auto armor = std::make_shared<ArmorPlate>(
            armor_id++, 0, 0, 0, Config::getInstance().getGlobal().armor_plate_width,
            stringToColor(config.color), config.number,
            stringToSide(config.side), self_ptr
        );
        armors_.push_back(armor);
    }
    updateArmors(); // 更新装甲板位置
}

void EnemyRobot::updateArmors() {
    Robot::updateArmors();
}
