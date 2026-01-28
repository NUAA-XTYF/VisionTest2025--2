#include"object.hpp"
#include<cmath>
//---------------------------------------------------------------------
Object::Object(int id0,double x0,double y0,double dir0,bool movable0=false)
	:id(id0),x(x0),y(y0),direction(dir0),movable(movable0){
		st.add_constants();
		st.add_variable("t",t);
		st.add_variable("x",x);
		st.add_variable("y",y);
		st.add_variable("dir",direction);
		xexpr.register_symbol_table(st);
		yexpr.register_symbol_table(st);
		direxpr.register_symbol_table(st);
	}

void Object::movement(double deltatime,const std::string xS,const std::string yS,const std::string dirS){
	if(!movable)return;
	bool xmovable=parser.compile(xS,xexpr);
	bool ymovable=parser.compile(yS,yexpr);
	bool dirmovable=parser.compile(dirS,direxpr);
	if(!(xmovable && ymovable && dirmovable))return;
	t+=deltatime;
	x=xexpr.value();
	y=yexpr.value();
	direction=direxpr.value();
	
	direction=fmod(direction,2*M_PI);
	if(direction<-M_PI){
		direction+=2*M_PI;
    }
    if(direction>=M_PI){
    	direction-=2*M_PI;
	}
}
//---------------------------------------------------------------------
ColoredObject::ColoredObject(int id0,double x0,double y0,double dir0,bool movable0=false,ObjectColor Color0):
	Object(id0,x0,y0,dir0,movable0),Color(Color0){
	}















