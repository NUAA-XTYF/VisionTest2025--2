#include<Robot.hpp>
#include<Object.hpp>
#include<geometry.hpp>

using namespace RobotSimulation;

//颜色
ColoredObject::ColoredObject(ObjectColor color): color(color){}

//装甲板

//能量机关

//障碍物

//机器人
Robot::Robot(int id, const Vector2D& pos, double dir, bool movable, 
             ObjectColor color, double length, double width)
    : Object(id, pos, dir, movable), ColoredObject(color),
      length(length), width(width), position(pos), 
      gimbalAngle(0.0), rotationFunction("") {}
Robot::~Robot() {
    armorPlates.clear();
}

// 装甲板管理
void Robot::addArmorPlate(std::shared_ptr<ArmorPlate> armor) {
    armorPlates.push_back(armor);
}

void Robot::removeArmorPlate(int plateNumber) {
    armorPlates.erase(
        std::remove_if(armorPlates.begin(), armorPlates.end(),
            [plateNumber](const std::shared_ptr<ArmorPlate>& armor) {
                return armor->getNumber() == plateNumber;
            }),
        armorPlates.end()
    );
}

void Robot::updateAllArmorPlates() {
    for (auto& armor : armorPlates) {
        armor->updateWorldPosition();
    }
}

std::vector<std::shared_ptr<ArmorPlate>> Robot::getArmorPlates() const {
    return armorPlates;
}

// 云台控制
void Robot::setGimbalAngle(double angle) {
    gimbalAngle = angle;
    // 规范化角度到 [-π, π)
    while (gimbalAngle < -PI) gimbalAngle += 2 * M_PI;
    while (gimbalAngle >= PI) gimbalAngle -= 2 * M_PI;
}

double Robot::getGimbalWorldDirection() const {
    // 云台的世界朝向 = 底盘朝向 + 云台相对角度
    return getGimbalWorldDirection() + gimbalAngle;
}

// 旋转控制
void Robot::setRotationFunction(const std::string& func) {
    rotationFunction = func;
}

void Robot::applyRotation(double time) {
    if (!rotationFunction.empty()) {
        double rotationSpeed = 1.0; 
        double deltaAngle = rotationSpeed * time;
        
        // 更新朝向
        applyRotation(getGimbalWorldDirection() + deltaAngle);
        
        // 更新位置
        double moveSpeed = 0.5;
        Vector2D currentpos=getPosition();
        Vector2D newPosition = Vector2D(moveSpeed * cos(getGimbalWorldDirection()) * time+currentpos.x,moveSpeed * sin(getGimbalWorldDirection()) * time+currentpos.y);
        
        // 更新所有装甲板位置
        updateAllArmorPlates();
    }
}

Vector2D Robot::getPosition() const {
    return position;
}

// 虚函数实现
std::vector<Vector2D> Robot::getBounds() const {
    std::vector<Vector2D> bounds;
    
    // 计算机器人的四个角点（矩形）
    double halfLength = length / 2.0;
    double halfWidth = width / 2.0;
    double dir = getGimbalWorldDirection();
    
    // 四个角点相对于中心的位置
    Vector2D corners[4] = {
        Vector2D(-halfLength, -halfWidth), // 左下
        Vector2D(-halfLength, halfWidth),  // 右下
        Vector2D(halfLength, halfWidth),   // 右上
        Vector2D(halfLength, -halfWidth)   // 左上
    };
    
    // 旋转并平移每个角点
    double cosDir = cos(dir);
    double sinDir = sin(dir);
    
    for (int i = 0; i < 4; i++) {
        double x = corners[i].x * cosDir - corners[i].y * sinDir + position.x;
        double y = corners[i].x * sinDir + corners[i].y * cosDir + position.y;
        bounds.push_back(Vector2D(x, y));
    }
    
    return bounds;
}

std::string Robot::describe() const {
    std::string desc = "Robot [ID=" + std::to_string(Object::id) + 
                       ", Color=" + (ObjectColor() == ObjectColor::Red ? "RED" : "BLUE") +
                       ", Position=(" + std::to_string(position.x) + ", " + 
                       std::to_string(position.y) + ")" +
                       ", Direction=" + std::to_string(getGimbalWorldDirection()) +
                       ", Size=" + std::to_string(length) + "x" + 
                       std::to_string(width) + "]";
    return desc;
}