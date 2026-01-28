#include"target.hpp"
#include<cmath>
//---------------------------------------------------------------------
Target::Target(int id0,double x0,double y0,double dir0,bool movable0=false,double w):
	Object(id0,x0,y0,dir0,movable0=false),width(w){
	}

virtual Bounds Target::GetBounds(){
	double half_width=width/2;
	double dx=half_width*cos(direction);
	double dy=half_width*sin(direction);
	return Bounds(x+dx,y+dy,x-dx,y-dy);
}
std::string Target::describe(){
	return "Target{ "
	+"[id:"+std::to_string(id)+"] "
	+"[pos:"+std::to_string(x)+","+std::to_string(y)+"] "
	+"[direction:"+std::to_string(direction)+"] "
	+"[width:"+std::to_string(width)+"] "
	+"}";
}
//---------------------------------------------------------------------
ArmorPlate::ArmorPlate(int number0,ArmorSide side0,ObjectColor color0,double width0,double relX0,double relY0,double normalX0=0,double normalY0=-1):
	width(width0),number(number0),side(side0),color(color0),relX(relX0),relY(relY0),normalX(normalX0),normalY(normalY0){
	}

Bounds ArmorPlate::GetBounds()override{
	auto host=hostRobot.lock();
    double worldX=host->getX()+relX;
    double worldY=host->getY()+relY;
    return Bounds(worldX-width/2, worldY-width/2, worldX+width/2, worldY+width/2);
}
bool ArmorPlate::isHit(double bulletdirX,double bulletdirY){
	double dot=bulletdirX*normalX+bulletdirY*normalY;
	return dot<0;
}
//---------------------------------------------------------------------
EnergyMechanism::EnergyMechanism(int id0,double x0,double y0,double dir0,double width,bool activated=true,double damage_boost,damage_duration=-1):
	Target(id0,x0,y0,dir0,false,w),isActivatable(activated),damage_boost(damage_boost0),damage_duration(damage_duration0){}
//---------------------------------------------------------------------
Obstacle::Obstacle(int id0,double x0,double y0,double dir0,double width0,std::string material0,bool destructible0,bool cover_only0):
	Target(id0,x0,y0,dir0,false,width0),material(material0),destructible(destructible0),cover_only(cover_only0){}
//---------------------------------------------------------------------
Robot::Robot(int id0,double x0,double y0,double dir0,ObjectColor::color,double len,double wid):
	Object(id0,x0,y0,dir0,true),Color(color),lenth(len),width(wid){}
void Robot::addArmorPlate(std::shared_ptr<ArmorPlate> armor){
    if(!armor)return;
    armor->setHost(std::shared_from_this());
    armorPlates.push_back(armor);
}














