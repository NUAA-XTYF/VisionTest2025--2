#include "object.h"
#include <stdexcept>

Object::Object(int id, double x, double y, double direction, bool movable)
    : id_(id), x_(x), y_(y), direction_(direction), movable_(movable), t_(0.0) {
    symbol_table_ = std::make_shared<symbol_table_t>();
    symbol_table_->add_variable("t", t_);
    
    // 创建 pi 变量，因为 std::numbers::pi 是 const，不能直接绑定
    static double pi_value = 3.14159265358979323846; // 使用字面值代替 std::numbers::pi
    symbol_table_->add_variable("pi", pi_value);
    
    x_expression_ = std::make_shared<expression_t>();
    y_expression_ = std::make_shared<expression_t>();
    dir_expression_ = std::make_shared<expression_t>();
    
    x_expression_->register_symbol_table(*symbol_table_);
    y_expression_->register_symbol_table(*symbol_table_);
    dir_expression_->register_symbol_table(*symbol_table_);
}

void Object::setPosition(double x, double y) {
    x_ = x;
    y_ = y;
}

void Object::setDirection(double direction) {
    direction_ = direction;
}

void Object::setMovable(bool movable) {
    movable_ = movable;
}

void Object::update(double time) {
    if (!movable_) return;
    
    t_ = time;
    
    if (x_expression_ && x_expression_->value() != 0.0) {
        x_ = x_expression_->value();
    }
    
    if (y_expression_ && y_expression_->value() != 0.0) {
        y_ = y_expression_->value();
    }
    
    if (dir_expression_ && dir_expression_->value() != 0.0) {
        direction_ = dir_expression_->value();
    }
}

void Object::setMovementFunctions(const std::string& x_expr, const std::string& y_expr, 
                                  const std::string& dir_expr) {
    if (!x_expr.empty()) {
        if (!parser_.compile(x_expr, *x_expression_)) {
            throw std::runtime_error("Failed to parse x expression");
        }
    }
    
    if (!y_expr.empty()) {
        if (!parser_.compile(y_expr, *y_expression_)) {
            throw std::runtime_error("Failed to parse y expression");
        }
    }
    
    if (!dir_expr.empty()) {
        if (!parser_.compile(dir_expr, *dir_expression_)) {
            throw std::runtime_error("Failed to parse direction expression");
        }
    }
}
