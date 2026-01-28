#pragma once

#include<string>
#include<memory>
#include<optional>
#include<Target.hpp>
#include<basic_information.hpp>

namespace RobotSimulation{

//具有颜色的对象
class ColoredObject
{
protected:
    ObjectColor color;

public:
    ColoredObject(ObjectColor color);
    virtual ~ColoredObject()=default;
    ObjectColor getColour() const {return color;}
    void setColour(ObjectColor newColour) {color=newColour;}

};

//机器人基类
class Robot:public Object,public ColoredObject
{
protected:
    double length;
    double width;
    double gimbalAngle;
    std::vector<std::shared_ptr<ArmorPlate>> armorPlates;
    std::string rotationFunction;
    Vector2D position;

public:
    Robot(int id,const Vector2D& pos,double dir,
        bool movable,ObjectColor color,double length,double width);
    virtual ~Robot() override;

    //装甲板管理
    void addArmorPlate(std::shared_ptr<ArmorPlate> armor);
    void removeArmorPlate(int plateNumber);
    void updateAllArmorPlates();
    std::vector<std::shared_ptr<ArmorPlate>> getArmorPlates() const;

    //云台控制
    double getGimbalAngle() const{return gimbalAngle;}
    void setGimbalAngle(double angle);
    double getGimbalWorldDirection() const;

    //旋转控制
    void setRotationFunction(const std::string& func);
    void applyRotation(double time);
    Vector2D getPosition() const {return position;}

    //虚函数控制
    std::vector<Vector2D> getBounds() const override;
    std::string describe() const override;

    ///几何信息
    double getLength() const{return length;}
    double getWidth() const{return width;}

};

//装甲板
class ArmorPlate:public Target{
protected:
    ObjectColor color;
    int ArmorNumber;
    ArmorSide side;
    bool isHittable;
    Vector2D ArmorPosition;
    Vector2D outwardsNormal;
    std::weak_ptr<Robot> hostRobot;

public:
    ArmorPlate(int id,const Vector2D& armorpos,ObjectColor color,
        int number,ArmorSide side,std::shared_ptr<Robot> host);
    ~ArmorPlate() override;

    //实现虚函数
    std::vector<Vector2D> getBounds() const override;
    std::string describe() const override;
    bool isHittable(const Vector2D& hitDirection) const override;
    void onHit(double damage) override;

    //更新位置
    void updateWorldPosition();

    //信息
    ObjectColor getColour() const{return color;}
    int getNumber() const{return ArmorNumber;}
    ArmorSide getside() const{return side;}
    Vector2D getOutwardNormal() const{return outwardsNormal;}

    //获取宿主
    std::shared_ptr<Robot> getHost() const;
};

//能量机关
class EnergyMechanism:public Target{
private:
    bool isActivated;
    double activationTime;
    double boostMultiplier;
    double boostDuration;

public:
    EnergyMechanism(int id,const Vector2D& pos,double dir,
        double width,double height);
    ~EnergyMechanism() override;

    //实现虚函数
    std::vector<Vector2D> getBounds() const override;
    std::string describe() const override;
    bool isHittable(const Vector2D& hitDirection) const override;
    void onHit(double damage) override;

    //伤害加成
    void activate(double multiplier,double duration);
    bool isActive(double currentTime) const;
    double getCurrentMultiplier(double surrentTime) const;
};

//碰撞检测
class Ray{
public:
    Vector2D origin;
    Vector2D direction;

    Ray(const Vector2D& origin,const Vector2D& direction);

    //线段相交
    std::optional<Vector2D> intersectSegment(
        const Vector2D& p1,const Vector2D& p2
    ) const;

};

//障碍物
class Obstacle:public Target{
private:
    std::string material;
    bool destructible;
    double durability;
    double maxDurability;

public:
    Obstacle(int id,const Vector2D& pos,double dir,
        double width,double height,
        const std::string& material="concrete",bool destructible=false,double durability=100.0);
    ~Obstacle() override;

    //实现虚函数
    std::vector<Vector2D> getBounds() const override;
    std::string describe() const override;
    bool isHittable(const Vector2D& hitDirection) const override;
    void onHit(double damage) override;

    //遮挡测试
    bool blockLineOfSight(const Vector2D& from,const Vector2D& to) const;
};

}