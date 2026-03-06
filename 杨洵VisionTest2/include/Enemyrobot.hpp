#pragma once

#include<Robot.hpp>

namespace RobotSimulation{

class EnemyRobot : public Robot,public ArmorPlate{
private:
    double health;
    double maxHealth;

public:
    EnemyRobot(int id, double x, double y, double dir, ObjectColor color,
               double length, double width, double gimbalAngle,
               int chassisNum, int gyroNum, double MaxHealth);
    virtual ~EnemyRobot() = default;
    
    // 血量管理
    double getHealth() const;
    double getMaxHealth() const;
    double getHealthPercentage() const;
    bool isAlive() const;
    
    // 伤害处理
    void takeDamage(double damage);
    double calculateEffectiveDamage(double rawDamage) const;
    void heal(double amount);
    void resetHealth();
    
    // 装甲管理
    double getArmorMultiplier() const;
    void setArmorMultiplier(double multiplier);
    
    // 从YAML创建装甲板
    void createArmorPlatesFromYAML(const std::vector<std::vector<double>>& armorData);
    void createArmorPlatesFromYAML(const std::string& yamlFilePath);
    
    // 状态更新
    void updateArmorPositions(); // 根据机器人姿态更新装甲板位置
    void update(double deltaTime); // 更新机器人状态
    
    // 序列化/反序列化
    std::string toYAML() const;
    static std::shared_ptr<EnemyRobot> fromYAML(const std::string& yamlStr);
    
    // 状态查询
    bool isTargetable() const;
    std::vector<std::shared_ptr<ArmorPlate>> getVulnerableArmors() const;
    
    // 友元函数，用于输出
    friend std::ostream& operator<<(std::ostream& os, const EnemyRobot& enemy);
};

// 友元函数声明
std::ostream& operator<<(std::ostream& os, const EnemyRobot& enemy);

// 辅助类：YAML装甲板数据解析器
class ArmorPlateData {
public:
    int plateNumber;
    ArmorSide side;
    double relativeX;
    double relativeY;
    double relativeAngle;
    
    ArmorPlateData(int number, ArmorSide side, double relX, double relY, double relAngle);
    static ArmorPlateData fromYAMLNode(const std::vector<double>& data);
    std::vector<double> toYAMLNode() const;
};
}