#pragma once
#include"object.hpp"
//---------------------------------------------------------------------
struct Bounds{
    double x1,y1,x2,y2;
    Bounds(double x1,double y1,double x2,double y2):x1(x1),y1(y1),x2(x2),y2(y2){
	}
};
//---------------------------------------------------------------------
class Target:public Object{
protected:
    double width;
public:
    Target(int id0,double x0,double y0,double dir0,bool movable0=false,double w);
    virtual ~Target()=default;
    
    virtual Bounds GetBounds();
    std::string describe();
    
    double getwidth(){return width;}
};
//---------------------------------------------------------------------
class ArmorPlate:public Target{
protected:
	int number;
	ObjectColor color;
	ArmorSide side;
	std::weak_ptr<class Robot> hostRobot;
	double relX, relY;
    double normalX, normalY;
public:
	ArmorPlate(int number0,ArmorSide side0,ObjectColor color0,double width0,double relX0,double relY0,double normalX0=0,double normalY0=-1);
    Bounds GetBounds()override;
    bool isHit(double bulletdirX,double bulletdirY);
    void setHost(std::shared_ptr<class Robot> host){hostRobot=host;}
    ArmorSide getSide(){return side;}
};
//---------------------------------------------------------------------
class EnergyMechanism:public Target{
protected:
	bool isActivatable;
	double damage_boost;
	double damage_duration;
public:
	EnergyMechanism(int id0,double x0,double y0,double dir0,double width,bool activated=true,double damage_boost,damage_duration=-1);
};
//---------------------------------------------------------------------
class Obstacle:public Target{
protected:
	std::string material;
	bool destructible;
	bool cover_only;
public:
	Obstacle(int id0,double x0,double y0,double dir0,double width0,std::string material0,bool destructible0,bool cover_only0);
};
//---------------------------------------------------------------------
class Robot:public Object,public ColoredObject{
protected:
	double lenth;
	double width;
	double gimbalAngle;
	std::vector<std::shared_ptr<ArmorPlate>> armorPlates;
public:
	Robot(int id0,double x0,double y0,double dir0,ObjectColor::color,double len,double wid);
	virtual ~Robot()=default;
	void addArmorPlate(std::shared_ptr<ArmorPlate> armor){
        if(!armor)return;
        armor->setHost(std::shared_from_this());
        armorPlates.push_back(armor);
    }
    void spinning();//空 
    void updateArmor();//空，需要小陀螺函数 
    void setGimbalAngle(double angle){gimbalAngle=angle;}
};
class enermy_Robot:public Robot{
protected:
	double HitPoint;
	double maxHitPoint;
	bool isHitable;
};
class SentryRobot:public Robot{
	
};































