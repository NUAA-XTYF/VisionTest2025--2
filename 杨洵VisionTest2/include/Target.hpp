#pragma once

#include<string>
#include<vector>
#include<Object.hpp>

namespace RobotSimulation{

class Target:public Object{
protected:
    double width;
    double height;

public:
    Target(int id,const Vector2D& pos,double dir,
    double width,double height);
    virtual ~Target() override=default;

    //获取长宽
    double getWidth()const {return width;}
    double getHeight()const {return height;}

    //返回端点
    virtual std::vector<Vector2D> getBounds() const=0;

    //describe
    virtual std::string describe() const=0;

    virtual bool isHittable(const Vector2D& hitDirection) const=0;
    virtual void onHit(double damage)=0;
};
}