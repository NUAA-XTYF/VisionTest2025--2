C++代码报告：巡天御风视觉组第二次考核项目
一、项目概述
1.1 项目背景

本项目为"巡天御风"视觉组第二次考核作业，旨在通过C++实现一个机甲大师哨兵模拟系统。系统能够模拟哨兵机器人自动巡逻、识别目标（装甲板、能量机关）并发射弹丸的完整流程。
1.2 核心目标

    学习并应用C++进阶知识，包括面向对象编程、现代C++特性

    实现完整的机器人控制、伤害判定系统

    掌握多线程协同和现代C++特性的实际应用

    通过YAML配置驱动系统运行

1.3 技术栈

    C++17标准

    面向对象编程

    多线程同步

    YAML配置文件解析

    表达式解析（exprtk）

    智能指针管理

二、系统架构设计
2.1 整体架构
text

┌─────────────────┐
│    GameSimulation    │
├─────────────────┤
│    + initialize()   │
│    + run()          │
│    + update()       │
└─────────┬─────────┘
          │
┌─────────▼─────────┐
│   对象管理体系     │
├─────────────────┤
│  - 哨兵机器人     │
│  - 敌方机器人     │
│  - 能量机关       │
│  - 障碍物         │
└─────────┬─────────┘
          │
┌─────────▼─────────┐
│   YAML配置系统    │
└─────────────────┘

2.2 核心类继承关系
text

Object (基类)
├── Target (抽象目标类)
│   ├── ArmorPlate (装甲板)
│   ├── EnergyMechanism (能量机关)
│   └── Obstacle (障碍物)
│
├── Robot (机器人基类)
│   ├── EnemyRobot (敌方机器人)
│   └── SentryRobot (哨兵机器人)
│
└── ColoredObject (颜色对象混入类)

三、关键类详细设计
3.1 Object类（基类）

功能：所有游戏对象的基类，管理对象的通用属性。
class Object {
    int id_;                    // 唯一标识符
    double x_, y_;              // 位置坐标
    double direction_;          // 朝向（弧度）
    bool movable_;              // 是否可移动
    // exprtk表达式解析相关成员
};

3.2 Target类（抽象目标类）

功能：所有可攻击目标的抽象基类。
cpp

class Target : public Object {
    virtual std::vector<Point> getBounds() const = 0;
    virtual std::string describe() const = 0;
    double width_;  // 目标宽度
};
3.3 ArmorPlate类（装甲板）

功能：敌方机器人的装甲板部件。
cpp

class ArmorPlate : public Target, public ColoredObject {
    int number_;                // 装甲板编号
    ArmorSide side_;           // 所属侧面
    std::weak_ptr<Robot> host_; // 宿主机器人（弱引用）
    
    bool isHitFromOutside(double hit_direction) const;
};
关键算法：单面击打判定
cpp

bool ArmorPlate::isHitFromOutside(double hit_direction) const {
    double armor_normal = getDirection() + M_PI/2;
    double direction_diff = hit_direction - armor_normal;
    // 角度归一化到[-π, π]
    return std::cos(direction_diff) < 0;
}
3.4 Robot类（机器人基类）

功能：所有机器人的基类，支持旋转和装甲板管理。
cpp

class Robot : public Object, public ColoredObject, 
              public std::enable_shared_from_this<Robot> {
    double length_, width_;      // 机器人尺寸
    double gimbal_direction_;    // 云台朝向
    std::vector<std::shared_ptr<ArmorPlate>> armors_;
    
    void updateArmors();  // 更新装甲板位置
};

关键特性：

    继承enable_shared_from_this支持安全地获取shared_ptr

    支持底盘旋转函数实现小陀螺效果

3.5 SentryRobot类（哨兵机器人）

功能：我方哨兵机器人，具有攻击能力。
cpp

class SentryRobot : public Robot {
    // 静态成员
    static int total_targets_detected_;
    static int instance_count_;
    static std::mutex static_mutex_;
    
    // 函数重载
    void fire();
    void fire(AmmoType ammo_type);
    
    // 伤害加成系统
    double current_damage_boost_;
    double damage_boost_end_time_;
};

设计模式：单例模式（Config类）、观察者模式（weak_ptr观察宿主）
3.6 DamageCalculator类（伤害计算器）

功能：计算不同类型弹丸的伤害。
cpp

class DamageCalculator {
public:
    static double calculateDamage(AmmoType ammo_type, double distance,
                                 double base_damage = 100.0,
                                 double distance_factor = 0.001) {
        double damage_multiplier = getMultiplier(ammo_type);
        double distance_penalty = std::exp(-distance_factor * distance);
        return base_damage * damage_multiplier * distance_penalty;
    }
};

伤害公式：
text

伤害 = 基础伤害 × 弹丸系数 × e^(-距离衰减因子 × 距离)

四、系统核心功能实现
4.1 YAML配置系统

配置结构：
yaml

global:
  base_damage: 100.0
  damage_distance_factor: 0.001
  
ally_sentry:
  position: {x: 2.0, y: 2.0}
  color: "blue"
  movable: true
  
enemy_robots:
  - position: {x: 10.0, y: 8.0}
    health: 600.0
    armors:
      - side: "front"
        number: 1

实现特点：

    使用yaml-cpp库解析配置文件

    支持缺省配置和容错处理

    支持数学表达式运动轨迹

4.2 运动系统

表达式解析：
cpp

void Object::setMovementFunctions(const std::string& x_expr, 
                                  const std::string& y_expr,
                                  const std::string& dir_expr) {
    if (!parser_.compile(x_expr, *x_expression_)) {
        throw std::runtime_error("Failed to parse x expression");
    }
    // ... 其他表达式
}

void Object::update(double time) {
    t_ = time;
    if (x_expression_) x_ = x_expression_->value();
    if (y_expression_) y_ = y_expression_->value();
    if (dir_expression_) direction_ = dir_expression_->value();
}

4.3 伤害判定系统

攻击流程：

    寻找最近的可见目标

    检查是否可从外部击中（单面击打判定）

    计算伤害（考虑距离衰减）

    应用伤害加成

    扣除目标血量

4.4 多线程同步

线程安全设计：
cpp

class SentryRobot : public Robot {
    static std::mutex static_mutex_;
    
    static void addTargetDetected(int count = 1) {
        std::lock_guard<std::mutex> lock(static_mutex_);
        total_targets_detected_ += count;
    }
};

global:
  base_damage: 100.0
  damage_distance_factor: 0.001
  
ally_sentry:
  position: {x: 2.0, y: 2.0}
  color: "blue"
  movable: true
  
enemy_robots:
  - position: {x: 10.0, y: 8.0}
    health: 600.0
    armors:
      - side: "front"
        number: 1

实现特点：

    使用yaml-cpp库解析配置文件

    支持缺省配置和容错处理

    支持数学表达式运动轨迹

4.2 运动系统

表达式解析：
cpp

void Object::setMovementFunctions(const std::string& x_expr, 
                                  const std::string& y_expr,
                                  const std::string& dir_expr) {
    if (!parser_.compile(x_expr, *x_expression_)) {
        throw std::runtime_error("Failed to parse x expression");
    }
    // ... 其他表达式
}

void Object::update(double time) {
    t_ = time;
    if (x_expression_) x_ = x_expression_->value();
    if (y_expression_) y_ = y_expression_->value();
    if (dir_expression_) direction_ = dir_expression_->value();
}

4.3 伤害判定系统

攻击流程：

    寻找最近的可见目标

    检查是否可从外部击中（单面击打判定）

    计算伤害（考虑距离衰减）

    应用伤害加成

    扣除目标血量

4.4 多线程同步

线程安全设计：
cpp

class SentryRobot : public Robot {
    static std::mutex static_mutex_;
    
    static void addTargetDetected(int count = 1) {
        std::lock_guard<std::mutex> lock(static_mutex_);
        total_targets_detected_ += count;
    }
};
编译和运行指南
bash

# 安装依赖
sudo apt-get install libyaml-cpp-dev

# 下载exprtk.hpp到third_party目录

# 编译项目
mkdir build && cd build
cmake .. -DCMAKE_CXX_STANDARD=17
make

# 运行
./visi_sim

9.2 配置文件示例
yaml

# config/config.yaml
global:
  map_width: 28.0
  map_height: 15.0
  base_damage: 100.0
  
ally_sentry:
  position: {x: 2.0, y: 2.0}
  direction: 0.0
  color: "blue"
  
enemy_robots:
  - position: {x: 10.0, y: 8.0}
    health: 600.0
    armors:
      - side: "front"
        color: "red"
        number: 1
