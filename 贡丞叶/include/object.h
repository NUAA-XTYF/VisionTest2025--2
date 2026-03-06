#ifndef OBJECT_H
#define OBJECT_H

#include <memory>
#include <string>
#include <functional>
#include "exprtk.hpp"

class Object {
public:
    Object(int id, double x = 0.0, double y = 0.0, double direction = 0.0, bool movable = false);
    virtual ~Object() = default;
    
    int getId() const { return id_; }
    double getX() const { return x_; }
    double getY() const { return y_; }
    double getDirection() const { return direction_; }
    bool isMovable() const { return movable_; }
    
    void setPosition(double x, double y);
    void setDirection(double direction);
    void setMovable(bool movable);
    
    virtual void update(double time);
    
    void setMovementFunctions(const std::string& x_expr, const std::string& y_expr, 
                              const std::string& dir_expr);
    
protected:
    int id_;
    double x_;
    double y_;
    double direction_; // 弧度，0指向+Y，逆时针为正
    bool movable_;
    
    // exprtk解析器
    typedef exprtk::symbol_table<double> symbol_table_t;
    typedef exprtk::expression<double> expression_t;
    typedef exprtk::parser<double> parser_t;
    
    std::shared_ptr<symbol_table_t> symbol_table_;
    std::shared_ptr<expression_t> x_expression_;
    std::shared_ptr<expression_t> y_expression_;
    std::shared_ptr<expression_t> dir_expression_;
    parser_t parser_;
    double t_; // 时间变量
};
#endif // OBJECT_H