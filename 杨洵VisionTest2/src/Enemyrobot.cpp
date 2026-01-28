#include<Enemyrobot.hpp>
#include<Robot.hpp>
#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<sstream>

using namespace RobotSimulation;

// 构造函数
EnemyRobot::EnemyRobot(int id, double x, double y, double dir, ObjectColor color,
                       double length, double width, double gimbalAngle,
                       int chassisNum, int gyroNum, double maxHealth)
    : Robot(id, Vector2D(x, y), dir, true, color, length, width),
      health(maxHealth), maxHealth(maxHealth){}

// 血量管理
double EnemyRobot::getHealth() const {
    return health;
}

double EnemyRobot::getMaxHealth() const {
    return maxHealth;
}

double EnemyRobot::getHealthPercentage() const {
    return (maxHealth > 0) ? (health / maxHealth) * 100.0 : 0.0;
}

bool EnemyRobot::isAlive() const {
    return health > 0;
}

// 伤害处理
void EnemyRobot::takeDamage(double damage) {
    if (health <= 0) return; // 已经死亡
    
    // 计算实际伤害（考虑装甲减免）
    double effectiveDamage = calculateEffectiveDamage(damage);
    health -= effectiveDamage;
    
    if (health < 0) health = 0;
    
    std::cout << "EnemyRobot " <<Object::RobotID << " took " << effectiveDamage 
              << " damage. Health: " << health << "/" << maxHealth << std::endl;
}

double EnemyRobot::calculateEffectiveDamage(double rawDamage) const {
    // 这里可以添加更复杂的伤害计算逻辑
    // 例如：考虑装甲类型、角度、距离等
    return rawDamage;
}

void EnemyRobot::heal(double amount) {
    health += amount;
    if (health > maxHealth) {
        health = maxHealth;
    }
    
    std::cout << "EnemyRobot " << Object::RobotID << " healed " << amount 
              << ". Health: " << health << "/" << maxHealth << std::endl;
}

void EnemyRobot::resetHealth() {
    health = maxHealth;
    std::cout << "EnemyRobot " << Object::RobotID << " health reset to " << health << std::endl;
}

// 装甲管理
double EnemyRobot::getArmorMultiplier() const {
    // 这里可以根据装甲板的状态计算伤害减免
    // 返回小于1.0的值表示有伤害减免
    return 1.0; // 默认无减免
}

void EnemyRobot::setArmorMultiplier(double multiplier) {
    // 设置全局装甲伤害减免系数
    // 在实际实现中，可能会影响所有装甲板
    std::cout << "Armor multiplier set to " << multiplier << std::endl;
}

// 从YAML创建装甲板
void EnemyRobot::createArmorPlatesFromYAML(const std::vector<std::vector<double>>& armorData) {
    // 清除现有的装甲板
    armorPlates.clear();
    
    for (const auto& armorInfo : armorData) {
        if (armorInfo.size() >= 5) {
            int plateNumber = static_cast<int>(armorInfo[0]);
            ArmorSide side = static_cast<ArmorSide>(static_cast<int>(armorInfo[1]));
            double relativeX = armorInfo[2];
            double relativeY = armorInfo[3];
            double relativeAngle = armorInfo[4];
            
            // 创建ArmorPlateData对象
            ArmorPlateData data(plateNumber, side, relativeX, relativeY, relativeAngle);
            
            // 根据数据创建装甲板
            // 计算装甲板的世界位置
            Vector2D robotPos = getPosition();
            double robotDir = getGimbalWorldDirection();
            
            // 将相对坐标转换为世界坐标
            double worldX = robotPos.x + relativeX * cos(robotDir) - relativeY * sin(robotDir);
            double worldY = robotPos.y + relativeX * sin(robotDir) + relativeY * cos(robotDir);
            
            // 创建装甲板
            auto armor = std::make_shared<ArmorPlate>(
                plateNumber, 
                Vector2D(worldX, worldY), 
                ObjectColor(),
                plateNumber, 
                side
            );
            
            // 添加到装甲板列表
            addArmorPlate(armor);
            
            std::cout << "Created armor plate " << plateNumber 
                      << " at (" << worldX << ", " << worldY << ")" << std::endl;
        }
    }
    
    std::cout << "Created " << armorPlates.size() << " armor plates from vector data" << std::endl;
}

void EnemyRobot::createArmorPlatesFromYAML(const std::string& yamlFileName) {
    std::ifstream file(yamlFileName);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open YAML file: " << yamlFileName << std::endl;
        return;
    }
    
    std::vector<std::vector<double>> armorData;
    std::string line;
    
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::vector<double> armorInfo;
        double value;
        while (iss>>value) {
            armorInfo.push_back(value);
        }
        
        if (armorInfo.size() >= 5) {
            armorData.push_back(armorInfo);
        }
    }
    
    file.close();
    
    // 使用向量数据创建装甲板
    createArmorPlatesFromYAML(armorData);
    
    std::cout << "Created armor plates from YAML file: " << yamlFileName << std::endl;
}

// 状态更新
void EnemyRobot::updateArmorPositions() {
    // 获取机器人的当前状态
    Vector2D robotPos = getPosition();
    double robotDir = getGimbalWorldDirection();
    
    // 更新所有装甲板的位置
    for (auto& armor : armorPlates) {
        armor->updateWorldPosition();
    }getGimbalWorldDirection();
}

void EnemyRobot::update(double deltaTime) {
    // 调用基类的更新
    update(deltaTime); 
    
    // 更新装甲板位置
    updateArmorPositions();
}

std::shared_ptr<EnemyRobot> EnemyRobot::fromYAML(const std::string& yamlStr) {
    std::cout << "Warning: EnemyRobot::fromYAML not fully implemented" << std::endl;
    return nullptr;
}

// 状态查询
bool EnemyRobot::isTargetable() const {
    return isAlive() && health > 0;
}

std::vector<std::shared_ptr<ArmorPlate>> EnemyRobot::getVulnerableArmors() const {
    std::vector<std::shared_ptr<ArmorPlate>> vulnerableArmors;
    
    for (const auto& armor : armorPlates) {
        if (armor->isHittable(Vector2D(0, 0))) { 
            vulnerableArmors.push_back(armor);
        }
    }
    
    return vulnerableArmors;
}

// ArmorPlateData 类的实现
ArmorPlateData::ArmorPlateData(int number, ArmorSide side, double relX, double relY, double relAngle)
    : plateNumber(number), side(side), relativeX(relX), relativeY(relY), relativeAngle(relAngle) {}

ArmorPlateData ArmorPlateData::fromYAMLNode(const std::vector<double>& data) {
    if (data.size() >= 5) {
        return ArmorPlateData(
            static_cast<int>(data[0]),
            static_cast<ArmorSide>(static_cast<int>(data[1])),
            data[2],
            data[3],
            data[4]
        );
    }
    
    // 返回默认值
    return ArmorPlateData(0, ArmorSide::Front, 0.0, 0.0, 0.0);
}

std::vector<double> ArmorPlateData::toYAMLNode() const {
    return {
        static_cast<double>(plateNumber),
        static_cast<double>(side),
        relativeX,
        relativeY,
        relativeAngle
    };
}

// 友元函数实现
std::ostream& operator<<(std::ostream& os, const EnemyRobot& enemy) {
    os << "EnemyRobot[ID=" << enemy.RobotID
       << ", Health=" << enemy.getHealth() << "/" << enemy.getMaxHealth()
       << ", Position=(" << enemy.getPosition().x << "," << enemy.getPosition().y << ")"
       << ", Alive=" << (enemy.isAlive() ? "true" : "false")
       << ", ArmorPlates=" << enemy.Robot::armorPlates.size() << "]";
    return os;
}