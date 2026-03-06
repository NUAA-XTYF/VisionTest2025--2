#pragma once

#include<string>
#include<memory>
#include<geometry.hpp>

namespace RobotSimulation{

//运动
class movement{
public:
    virtual ~movement()= default;
    virtual void update(Object& obj,double deltatime)=0;
    virtual void Expression(const std::string& expr)=0;
};

class Object{
protected:
    int id;
    Vector2D position;
    double direction;
    bool isMovable;
    std::unique_ptr<movement> movementController;

public:
    Object(int id,const Vector2D& pos,double dir,bool isMovable=false);
    virtual ~Object()=default;

    //基础信息
    int RobotID() const{return id;}
    Vector2D RobotPosition() const{return position;}
    double RobotDirection() const{return direction;}
    bool isMoving() const{return isMovable;}

    //获取信息
    void Position(const Vector2D& pos){position=pos;}
    void Direction(const double dir){direction=normalizedAngle(dir);}

    //运动控制
    void setMovementController(std::unique_ptr<movement> controller);
    void update(double deltaTime);

    //虚函数
    virtual std::vector<Vector2D> getBounds() const=0;
    virtual std::string describe() const=0;
};
}