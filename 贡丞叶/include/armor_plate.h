#ifndef ARMOR_PLATE_H
#define ARMOR_PLATE_H

#include "target.h"
#include "colored_object.h"
#include <memory>

enum class ArmorSide {
    FRONT,
    BACK,
    LEFT,
    RIGHT
};

class Robot;

class ArmorPlate : public Target, public ColoredObject {
public:
    ArmorPlate(int id, double x, double y, double direction, double width,
               ObjectColor color, int number, ArmorSide side, 
               std::weak_ptr<Robot> host);
    virtual ~ArmorPlate() = default;
    
    int getNumber() const { return number_; }
    ArmorSide getSide() const { return side_; }
    
    virtual std::vector<Point> getBounds() const override;
    virtual std::string describe() const override;
    
    bool isHitFromOutside(double hit_direction) const;
    
    std::weak_ptr<Robot> getHost() const { return host_; }
    
private:
    int number_;
    ArmorSide side_;
    std::weak_ptr<Robot> host_;
};

#endif
