#ifndef TARGET_H
#define TARGET_H

#include "object.h"
#include <memory>
#include <vector>

struct Point {
    double x, y;
    Point(double x = 0, double y = 0) : x(x), y(y) {}
};

class Target : public Object {
public:
    Target(int id, double x, double y, double direction, double width);
    virtual ~Target() = 0;
    
    double getWidth() const { return width_; }
    void setWidth(double width) { width_ = width; }
    
    virtual std::vector<Point> getBounds() const = 0;
    virtual std::string describe() const = 0;
    
protected:
    double width_;
};
#endif // TARGET_H