#include<string>
#include<Object.hpp>
#include<basic_information.hpp>
#include<exprtk.hpp>

using namespace RobotSimulation;

Object::Object(int id,const Vector2D& pos,double dir,bool isMovable=false)
    :id(id),position(pos),direction(dir),isMovable(isMovable){}

void Object::setMovementController(std::unique_ptr<movement>controller){
    movementController=std::move(controller);
}

void Object::update(double deltaTime){
    if(movementController&&isMovable){
        movementController->update(*this,deltaTime);
    }
}
