#ifndef ROBOT_H
#define ROBOT_H

#include "object.h"
#include "colored_object.h"
#include "armor_plate.h"
#include <vector>
#include <memory>
#include <functional>

class Robot : public Object, public ColoredObject, public std::enable_shared_from_this<Robot> {
public:
    Robot(int id, double x, double y, double direction, bool movable,
          ObjectColor color, double length, double width, double gimbal_direction = 0.0);
    virtual ~Robot() = default;
    
    double getLength() const { return length_; }
    double getWidth() const { return width_; }
    double getGimbalDirection() const { return gimbal_direction_; }
    void setGimbalDirection(double direction) { gimbal_direction_ = direction; }
    
    virtual void update(double time) override;
    virtual void updateArmors();
    virtual std::vector<std::shared_ptr<ArmorPlate>> getArmors() const { return armors_; }
    
    void setChassisRotationFunction(const std::string& expr);
    
protected:
    double length_;
    double width_;
    double gimbal_direction_;
    std::vector<std::shared_ptr<ArmorPlate>> armors_;
    
    typedef exprtk::symbol_table<double> symbol_table_t;
    typedef exprtk::expression<double> expression_t;
    typedef exprtk::parser<double> parser_t;
    
    std::shared_ptr<symbol_table_t> rotation_symbol_table_;
    std::shared_ptr<expression_t> rotation_expression_;
    parser_t rotation_parser_;
    double rotation_t_;
};

#endif
