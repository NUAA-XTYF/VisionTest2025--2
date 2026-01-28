#pragma once

#include<ostream>
#include<Robot.hpp>

namespace RobotSimulation{

//我方机器人（哨兵）
class SentryRobot:public Robot{
private:
    //静态成员
    static int totalTargetDetected;
    static int instanceCount;

    //弹药库存
    std::map<AmmoType,int> ammunition;

    //伤害加成
    double damageMultiplier;
    double boostEndTime;

    //射击统计
    int shotsFired;
    int hitsLanded;
    int totalDamageDealt;

public:
    //析沟函数与构造函数
    SentryRobot(int id,const Vector2D& pos,double dir,bool movable,ObjectColor color,double length,double width);
    ~SentryRobot() override;

    //静态函数
    static int getTotalTargetsDetected() {return totalTargetDetected;}
    static int getInstanceCount(){return instanceCount;}
    static void resetStatistics();

    //弹药管理
    void addAmmo(AmmoType type,int count);
    bool hasAmmo(AmmoType type) const;
    int getAmmoCount(AmmoType type) const;

    //移动
    void move(double distance);
    void move(double distance,double direction);
    void changeDirection(double deltaAngle);

    //开火
    void fire();
    void fire(AmmoType ammoType);
    void fireAt(std::shared_ptr<Target> target);
    void fireAt(std::shared_ptr<Target> target,AmmoType ammoType);

    //伤害加成
    void activateDamageBoost(double multiplier,double duration);
    double getCurrentDamageBoost(double currentTime) const;
    bool hasActiveBoost(double currentTime) const;

    //目标检测
    void detectTargets(const std::vector<std::shared_ptr<Target>>& allTargets);

    //统计信息
    int getShotsFired() const{return shotsFired;}
    int getHitsLanded() const{return hitsLanded;}
    int getTotalDamage() const{return totalDamageDealt;}
    double getAccurary() const{
        return shotsFired>0 ? static_cast<double>(hitsLanded)/shotsFired : 0.0;
    }

    //友元函数输出
    friend std::ostream& operator<<(std::ostream& os,const SentryRobot& sentry);
};

//友元函数输出运算符声明
std::ostream& operator<<(std::ostream& os,const SentryRobot& sentry);

}