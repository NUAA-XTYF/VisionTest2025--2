#pragma once
#include"exprtk.hpp"
#include<iostream>
#include<memory>
#include<vector>
//---------------------------------------------------------------------
class Object{
protected:
	int id;
	double t;
	double x;
	double y;
	double direction;
	bool movable;
	
	exprtk::symbol_table<double> st;
	
	exprtk::expression<double> xexpr;
	exprtk::expression<double> yexpr;
	exprtk::expression<double> direxpr;
	
	std::string xstr;
	std::string ystr;
	std::string dirstr;
	
public:
	Object(int id0,double x0,double y0,double dir0,bool movable0=false);
	
	int get_id(){return id;}
	double get_x(){return x;}
	double get_y(){return y;}
	double get_dir(){return direction;}
	bool get_movable(){return movable;}
	
	void movement(double deltatime,const std::string xexpr,const std::string yexpr,const std::string direxpr);
	int getID(){return id;}
	double getT(){return t;}
	double getX(){return x;}
	double getY(){return y;}
	double getDIR(){return direction;}
};
//---------------------------------------------------------------------
enum ObjectColor{WHITE,BLACK};
enum ArmorSide{LEFT,RIGHT,FRONT};
enum AmmoType{A,B,C};
//---------------------------------------------------------------------
class ColoredObject:public Object{
protected:
	ObjectColor Color;
public:
	ColoredObject(int id0,double x0,double y0,double dir0,bool movable0=false,ObjectColor Color0);
	ObjectColor getColor(){return Color;}
};
//---------------------------------------------------------------------
namespace enumtool{
	inline std::string Colortostring(ObjectColor Cl){
		if(Cl==WHITE)return "WHITE";
		if(Cl==BLACK)return "BLACK";
		return "UNKNOWN";
	}
	inline std::string Armortostring(ArmorSide As){
		if(As==LEFT)return "LEFT";
		if(As==RIGHT)return "RIGHT";
		if(As==FRONT)return "FRONT";
		return "UNKNOWN";
	}
	inline std::string Ammotostring(AmmoType At){
		if(At==A)return "A";
		if(At==B)return "B";
		if(At==C)return "C";
		return "UNKNOWN";
	}
}
//---------------------------------------------------------------------
class Robot:public Object,public ColoredObject{};









