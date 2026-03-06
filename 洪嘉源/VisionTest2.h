// VisionTest2.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。

#pragma once

#include <iostream>
#include <string>
#include <map>
#include <algorithm>
#include <cmath>
#include <vector>
#include <memory>
#include <sstream>
#include <array>
#include <fstream>
#include <thread>
#include <chrono>

#include <yaml-cpp/yaml.h>
#include "./package/exprtk/exprtk.hpp"

// 公共枚举与工具声明
enum class ObjectColor {
    RED,
    BLUE,
    UNKNOWN
};

enum class ArmorSide {
    FRONT,
    BACK,
    LEFT,
    RIGHT,
    TOP,
    BOTTOM
};

enum class AmmoType {
    LARGE,
    SMALL,
    UNKNOWN
};

namespace EnumUtils {
    std::string objectColorToString(ObjectColor color);
    ObjectColor stringToObjectColor(const std::string& str);

    std::string armorSideToString(ArmorSide side);
    ArmorSide stringToArmorSide(const std::string& s);

    std::string ammoTypeToString(AmmoType type);
    AmmoType stringToAmmoType(const std::string& str);
}

// 简单的几何工具与类型
struct Vec2 {
    double x;
    double y;
    Vec2() : x(0), y(0) {}
    Vec2(double _x, double _y) : x(_x), y(_y) {}
};

// 几何函数声明（实现移到 .cpp）
Vec2 rotateLocalToWorld(const Vec2& local, double hostDir);
double normalizeAngle(double ang);
double distance(const Vec2& a, const Vec2& b);
double angleTo(const Vec2& from, const Vec2& to);
Vec2 rotatePoint(const Vec2& p, double ang);
double cross2d(const Vec2& a, const Vec2& b);

// 射线与线段相交检测（射线 origin + t*dir, t>=0; 线段 a->b, u in [0,1]）
bool rayIntersectsSegment(const Vec2& origin, const Vec2& dir, const Vec2& a, const Vec2& b, double* outT = nullptr, double* outU = nullptr);

// 环形缓冲区模板
template<typename T, size_t N>
class RingBuffer {
    static_assert(N > 0, "RingBuffer size must be > 0");
    std::array<T, N> buf{};
    size_t head = 0;
    size_t tail = 0;
    size_t count = 0;
public:
    RingBuffer() = default;
    void push(const T& v) {
        buf[tail] = v;
        tail = (tail + 1) % N;
        if (count == N) {
            // overwrite oldest
            head = (head + 1) % N;
        } else {
            ++count;
        }
    }
    bool pop(T& out) {
        if (count == 0) return false;
        out = buf[head];
        head = (head + 1) % N;
        --count;
        return true;
    }
    bool full() const { return count == N; }
    bool empty() const { return count == 0; }
    size_t size() const { return count; }
    const T& operator[](size_t idx) const {
        if (idx >= count) throw std::out_of_range("RingBuffer index");
        size_t real = (head + idx) % N;
        return buf[real];
    }
};

// calculateDamage overloads - 声明
double calculateDamage(AmmoType type, const Vec2& a, const Vec2& b);
double calculateDamage(AmmoType type, double x1, double y1, double x2, double y2);
double calculateDamage(AmmoType type, double distance);

// 抽象目标基类
class Target; // forward

// Object 类（可移动的基础对象）
class Object {
protected:
    static int nextId;
    int id;
    double posX;
    double posY;
    double direction;
    bool movable;

    std::string moveExprX;
    std::string moveExprY;
    std::string moveExprDir;
    exprtk::symbol_table<double> symbleTable;
    exprtk::expression<double> exprX;
    exprtk::expression<double> exprY;
    exprtk::expression<double> exprDir;
    exprtk::parser<double> parser;

    double exprTime;
    double exprSpeed;
    double exprAngle;

    void setupExprTk();
    void compileExpressions();

public:
    Object(double x = 0, double y = 0, double dir = 0, bool mov = true);
    virtual ~Object();

    void setMovementExpressions(const std::string& exprX, const std::string& exprY, const std::string& exprDir);

    double getX() const { return posX; }
    double getY() const { return posY; }
    double getDirection() const { return direction; }
    int getId() const { return id; }

    void setPosition(double x, double y) { posX = x; posY = y; }
    void setDirection(double d) { direction = d; }
};

// ColoredObject
class ColoredObject : public Object {
protected:
    ObjectColor color;
public:
    ColoredObject(ObjectColor col = ObjectColor::UNKNOWN, double x = 0, double y = 0, double dir = 0, bool mov = true);
    virtual ~ColoredObject();
    ObjectColor getColor() const { return color; }
};

// 抽象目标基类（现在继承 ColoredObject，保留位置等）
class Target : public ColoredObject {
protected:
    double width;
public:
    Target(ObjectColor col = ObjectColor::UNKNOWN, double w = 1.0,
        double x = 0, double y = 0, double dir = 0, bool mov = true);
    virtual ~Target();

    double getWidth() const;

    virtual std::vector<std::pair<Vec2, Vec2>> GetBounds() const = 0;

    virtual std::string describe() const = 0;
};

// ArmorPlate
class ArmorPlate : public Target {
private:
    ArmorSide side;
    int number;
    std::weak_ptr<Object> host;
    Vec2 localCenter;
public:
    ArmorPlate(ArmorSide s, int num, double w, const Vec2& localCtr = Vec2(0,0), std::shared_ptr<Object> hostPtr = nullptr, ObjectColor col = ObjectColor::UNKNOWN);

    void setHost(std::shared_ptr<Object> hostPtr);
    void setLocalCenter(const Vec2& ctr);

    Vec2 worldCenter() const;
    Vec2 outerNormal() const;
    bool isHittableBy(const Vec2& attackerPos) const;

    std::vector<std::pair<Vec2, Vec2>> GetBounds() const override;
    std::string describe() const override;

    int getNumber() const { return number; }
    std::weak_ptr<Object> getHost() const { return host; }
};

// EnergyMechanism
class EnergyMechanism : public Target {
private:
    bool active;
    double boostMultiplier;
    double boostDuration;
public:
    EnergyMechanism(double w = 1.0, double multiplier = 1.5, double duration = 2.0, ObjectColor col = ObjectColor::UNKNOWN);
    void Activate();
    void Deactivate();
    bool isActive() const;
    double getMultiplier() const;
    double getDuration() const;

    std::vector<std::pair<Vec2, Vec2>> GetBounds() const override;
    std::string describe() const override;
};

// Obstacle
class Obstacle : public Target {
private:
    bool destructible;
    std::vector<std::pair<Vec2, Vec2>> segments;
public:
    Obstacle(double w = 1.0, bool destr = false, ObjectColor col = ObjectColor::UNKNOWN);
    void addSegment(const Vec2& a, const Vec2& b);
    const std::vector<std::pair<Vec2, Vec2>>& getSegments() const;

    std::vector<std::pair<Vec2, Vec2>> GetBounds() const override;
    std::string describe() const override;
};

// Robot 基类
class Robot : public ColoredObject, public std::enable_shared_from_this<Robot> {
protected:
    double chassisLength;
    double chassisWidth;
    double gimbalDirection;
    std::vector<std::shared_ptr<ArmorPlate>> armors;
public:
    Robot(ObjectColor col = ObjectColor::UNKNOWN, double len = 1.0, double wid = 1.0, double x = 0, double y = 0, double dir = 0);
    virtual ~Robot();

    void addArmor(const std::shared_ptr<ArmorPlate>& ap);
    void UpdateArmors();
    const std::vector<std::shared_ptr<ArmorPlate>>& getArmors() const;

    void setGimbalDirection(double g);
    double getGimbalDirection() const;
};

// EnemyRobot
class EnemyRobot : public Robot {
private:
    double health;
    double maxHealth;
public:
    EnemyRobot(double hp = 100.0, double maxHp = 100.0, ObjectColor col = ObjectColor::UNKNOWN, double len = 1.0, double wid = 1.0, double x = 0, double y = 0, double dir = 0);
    bool isAlive() const;
    void TakeDamage(double dmg);
    double getHealth() const;
};

// SentryRobot
class SentryRobot : public Robot {
private:
    double damageBonus;
public:
    static int totalTargetsDetected;
    static int instanceCount;

    SentryRobot(ObjectColor col = ObjectColor::UNKNOWN, double len = 1.0, double wid = 1.0, double x = 0, double y = 0, double dir = 0);
    ~SentryRobot();

    void setDamageBonus(double b);
    double getDamageBonus() const;

    void move(double length, double dirRad);
    void changeDirection(double changedRad);
    void fire(const std::shared_ptr<ArmorPlate>& target, AmmoType ammo = AmmoType::LARGE);

    friend std::ostream& operator<<(std::ostream& os, const SentryRobot& s);
};

// visibility check
bool isArmorVisible(const SentryRobot& sentry, const std::shared_ptr<ArmorPlate>& ap, const std::vector<std::shared_ptr<Obstacle>>& obstacles);


