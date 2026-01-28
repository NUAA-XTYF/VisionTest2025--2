#include "robot.h"
#include <cmath>

Robot::Robot(int id, double x, double y, double direction, bool movable,
             ObjectColor color, double length, double width, double gimbal_direction)
    : Object(id, x, y, direction, movable), ColoredObject(color),
      length_(length), width_(width), gimbal_direction_(gimbal_direction),
      rotation_t_(0.0) {
    
    rotation_symbol_table_ = std::make_shared<symbol_table_t>();
    rotation_symbol_table_->add_variable("t", rotation_t_);
    
    // 创建 pi 变量
    static double pi_value = 3.14159265358979323846; // 使用字面值
    rotation_symbol_table_->add_variable("pi", pi_value);
    
    rotation_expression_ = std::make_shared<expression_t>();
    rotation_expression_->register_symbol_table(*rotation_symbol_table_);
}

void Robot::update(double time) {
    Object::update(time);
    rotation_t_ = time;
    
    if (rotation_expression_ && rotation_expression_->value() != 0.0) {
        setDirection(getDirection() + rotation_expression_->value());
    }
}

void Robot::updateArmors() {
    double chassis_rotation = getDirection();
    double half_length = length_ / 2.0;
    double half_width = width_ / 2.0;
    
    for (auto& armor : armors_) {
        double local_x = 0.0, local_y = 0.0;
        
        switch (armor->getSide()) {
            case ArmorSide::FRONT:
                local_x = 0.0;
                local_y = half_length;
                break;
            case ArmorSide::BACK:
                local_x = 0.0;
                local_y = -half_length;
                break;
            case ArmorSide::LEFT:
                local_x = -half_width;
                local_y = 0.0;
                break;
            case ArmorSide::RIGHT:
                local_x = half_width;
                local_y = 0.0;
                break;
        }
        
        double cos_rot = std::cos(chassis_rotation);
        double sin_rot = std::sin(chassis_rotation);
        
        double world_x = getX() + local_x * cos_rot - local_y * sin_rot;
        double world_y = getY() + local_x * sin_rot + local_y * cos_rot;
        
        double armor_direction = chassis_rotation + 1.5707963267948966; // π/2
        if (armor->getSide() == ArmorSide::BACK) {
            armor_direction -= 3.14159265358979323846; // π
        }
        
        armor->setPosition(world_x, world_y);
        armor->setDirection(armor_direction);
    }
}

void Robot::setChassisRotationFunction(const std::string& expr) {
    if (!expr.empty()) {
        if (!rotation_parser_.compile(expr, *rotation_expression_)) {
            throw std::runtime_error("Failed to parse chassis rotation expression");
        }
    }
}
