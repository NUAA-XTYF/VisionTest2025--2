#include <iostream>
#include <memory>
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>
#include <algorithm>
#include <limits>
#include <iomanip>
#include <sstream>
#include <functional>
#include "config.h"
#include "sentry_robot.h"
#include "enemy_robot.h"
#include "energy_mechanism.h"
#include "geometry.h"
#include "damage_calculator.h"
#include "utils.h"
#include "ammo_type.h"
#include "colored_object.h"

class GameSimulation {
public:
    GameSimulation() : running_(false), current_time_(0.0) {}
    
    bool initialize() {
        // 加载配置文件
        if (!Config::getInstance().loadFromFile("config/config.yaml")) {
            std::cout << "Using default configuration\n";
            Config::getInstance().loadDefault();
        }
        
        // 初始化哨兵机器人
        auto& sentry_cfg = Config::getInstance().getSentry();
        sentry_ = std::make_shared<SentryRobot>(
            0, sentry_cfg.x, sentry_cfg.y, sentry_cfg.direction,
            sentry_cfg.movable, stringToColor(sentry_cfg.color),
            Config::getInstance().getGlobal().robot_length,
            Config::getInstance().getGlobal().robot_width,
            sentry_cfg.gimbal_direction.value_or(0.0)
        );
        
        // 设置哨兵的运动函数（如果配置中有）
        if (sentry_cfg.movable) {
            // 这里可以根据配置添加哨兵的运动函数
        }
        
        // 初始化敌人机器人
        int enemy_id = 1;
        for (const auto& enemy_cfg : Config::getInstance().getEnemies()) {
            auto enemy = std::make_shared<EnemyRobot>(
                enemy_id++, enemy_cfg.x, enemy_cfg.y, enemy_cfg.direction,
                enemy_cfg.movable, stringToColor(enemy_cfg.color),
                Config::getInstance().getGlobal().robot_length,
                Config::getInstance().getGlobal().robot_width,
                0.0, enemy_cfg.health, enemy_cfg.max_health
            );
            
            // 设置敌人的运动函数
            if (enemy_cfg.movable) {
                if (enemy_cfg.movement_x && enemy_cfg.movement_y && enemy_cfg.movement_dir) {
                    enemy->setMovementFunctions(
                        enemy_cfg.movement_x.value(),
                        enemy_cfg.movement_y.value(),
                        enemy_cfg.movement_dir.value()
                    );
                }
                
                if (enemy_cfg.chassis_rotation) {
                    enemy->setChassisRotationFunction(enemy_cfg.chassis_rotation.value());
                }
            }
            
            // 创建装甲板并添加到敌人
            enemy->createArmorsFromConfig(enemy_cfg.armors);
            enemies_.push_back(enemy);
        }
        
        std::cout << "Initialized " << enemies_.size() << " enemies\n";
        return true;
    }
    
    void run() {
        running_ = true;
        int loop_count = 0;
        const int max_loops = 2000;
        
        std::cout << "Starting simulation...\n";
        std::cout << "========================================\n";
        
        while (running_ && loop_count < max_loops) {
            // 更新时间
            update(loop_count * 0.001);
            
            // 寻找最近的攻击目标
            auto target = findNearestTarget();
            if (target) {
                attackTarget(target);
            } else {
                // 如果没有可攻击目标，等待一小段时间
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            
            // 冷却时间
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            ++loop_count;
            
            // 检查是否所有敌人都被消灭
            if (areAllEnemiesDead()) {
                std::cout << "\n========================================\n";
                std::cout << "All enemies destroyed!\n";
                break;
            }
            
            // 每100轮输出一次进度
            if (loop_count % 100 == 0) {
                int alive_count = std::count_if(enemies_.begin(), enemies_.end(),
                    [](const auto& enemy) { return enemy->isAlive(); });
                std::cout << "Progress: Loop " << loop_count 
                          << ", Enemies remaining: " << alive_count << "\n";
            }
        }
        
        // 如果达到最大循环次数
        if (loop_count >= max_loops) {
            std::cout << "\n========================================\n";
            std::cout << "Reached maximum loop count: " << max_loops << "\n";
        }
        
        printSummary(loop_count);
    }
    
private:
    void update(double dt) {
        current_time_ = dt;
        
        // 更新哨兵位置
        if (sentry_->isMovable()) {
            sentry_->update(dt);
        }
        
        // 更新所有敌人位置和装甲板
        for (auto& enemy : enemies_) {
            if (enemy->isMovable() && enemy->isAlive()) {
                enemy->update(dt);
                enemy->updateArmors();
            }
        }
    }
    
    std::shared_ptr<ArmorPlate> findNearestTarget() {
        std::shared_ptr<ArmorPlate> nearest = nullptr;
        double min_distance = std::numeric_limits<double>::max();
        
        for (const auto& enemy : enemies_) {
            if (!enemy->isAlive()) continue;
            
            for (const auto& armor : enemy->getArmors()) {
                // 计算距离
                double distance = Geometry::distance(
                    sentry_->getX(), sentry_->getY(),
                    armor->getX(), armor->getY()
                );
                
                // 检查是否可以从外部击中
                if (distance < min_distance && armor->isHitFromOutside(sentry_->getDirection())) {
                    min_distance = distance;
                    nearest = armor;
                }
            }
        }
        
        return nearest;
    }
    
    void attackTarget(std::shared_ptr<ArmorPlate> target) {
        if (!target) return;
        
        // 哨兵开火
        sentry_->fire(AmmoType::LARGE);
        
        // 计算基础伤害
        double damage = DamageCalculator::calculateDamage(
            AmmoType::LARGE,
            sentry_->getX(), sentry_->getY(),
            target->getX(), target->getY(),
            Config::getInstance().getGlobal().base_damage,
            Config::getInstance().getGlobal().damage_distance_factor
        );
        
        // 应用伤害加成
        damage *= sentry_->getDamageBoost();
        
        // 获取装甲板所属的敌人
        auto host = target->getHost().lock();
        if (host) {
            // 将Robot转换为EnemyRobot
            auto enemy = std::dynamic_pointer_cast<EnemyRobot>(host);
            if (enemy && enemy->isAlive()) {
                enemy->takeDamage(damage);
                
                // 输出命中信息
                std::ostringstream oss;
                oss << "[Hit] Enemy " << enemy->getId() 
                    << " Armor " << target->getNumber() << "-" 
                    << sideToString(target->getSide())
                    << ": damage=" << std::fixed << std::setprecision(2) << damage
                    << ", remaining health=" << std::fixed << std::setprecision(2) << enemy->getHealth();
                std::cout << oss.str() << std::endl;
                
                // 如果敌人死亡，输出信息
                if (!enemy->isAlive()) {
                    std::cout << "[Enemy Destroyed] Enemy " << enemy->getId() << " has been destroyed!\n";
                }
            }
        }
        
        // 增加检测计数
        SentryRobot::addTargetDetected();
    }
    
    bool areAllEnemiesDead() const {
        return std::all_of(enemies_.begin(), enemies_.end(),
            [](const auto& enemy) { return !enemy->isAlive(); });
    }
    
    void printSummary(int total_loops) {
        std::cout << "\n=== Simulation Summary ===\n";
        std::cout << "Total loops: " << total_loops << "\n";
        std::cout << "Total targets detected: " << SentryRobot::getTotalTargetsDetected() << "\n";
        std::cout << "Sentry instances: " << SentryRobot::getInstanceCount() << "\n";
        
        int alive_count = std::count_if(enemies_.begin(), enemies_.end(),
            [](const auto& enemy) { return enemy->isAlive(); });
        std::cout << "Enemies remaining: " << alive_count << "\n";
        
        // 输出每个敌人的状态
        std::cout << "\nEnemy status:\n";
        for (const auto& enemy : enemies_) {
            std::cout << "  Enemy " << enemy->getId() 
                      << ": health=" << std::fixed << std::setprecision(2) << enemy->getHealth()
                      << "/" << enemy->getMaxHealth()
                      << ", alive=" << (enemy->isAlive() ? "yes" : "no") << "\n";
        }
        
        // 输出哨兵状态
        std::cout << "\nSentry status:\n";
        std::cout << "  Position: (" << sentry_->getX() << ", " << sentry_->getY() << ")\n";
        std::cout << "  Direction: " << sentry_->getDirection() << " radians\n";
        std::cout << "  Damage boost: " << sentry_->getDamageBoost() << "x\n";
        
        // 计算总伤害
        double total_damage = 0;
        for (const auto& enemy : enemies_) {
            total_damage += enemy->getMaxHealth() - enemy->getHealth();
        }
        std::cout << "Total damage dealt: " << std::fixed << std::setprecision(2) << total_damage << "\n";
    }
    
    std::shared_ptr<SentryRobot> sentry_;
    std::vector<std::shared_ptr<EnemyRobot>> enemies_;
    bool running_;
    double current_time_;
};

int main() {
    try {
        std::cout << "========================================\n";
        std::cout << "Visi Simulation - NUAA XTYF\n";
        std::cout << "========================================\n\n";
        
        GameSimulation simulation;
        
        if (!simulation.initialize()) {
            std::cerr << "Failed to initialize simulation\n";
            return 1;
        }
        
        simulation.run();
        std::cout << "\nSimulation completed successfully.\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error during simulation: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error during simulation\n";
        return 1;
    }
    
    return 0;
}
