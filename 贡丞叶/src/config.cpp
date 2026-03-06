#include "config.h"
#include <fstream>
#include <yaml-cpp/yaml.h>
#include <iostream>

bool Config::loadFromFile(const std::string& filename) {
    try {
        YAML::Node config = YAML::LoadFile(filename);
        
        // 全局配置
        if (config["global"]) {
            auto global = config["global"];
            if (global["map_width"]) global_.map_width = global["map_width"].as<double>();
            if (global["map_height"]) global_.map_height = global["map_height"].as<double>();
            if (global["base_damage"]) global_.base_damage = global["base_damage"].as<double>();
            if (global["damage_distance_factor"]) global_.damage_distance_factor = global["damage_distance_factor"].as<double>();
            if (global["armor_plate_width"]) global_.armor_plate_width = global["armor_plate_width"].as<double>();
            if (global["robot_length"]) global_.robot_length = global["robot_length"].as<double>();
            if (global["robot_width"]) global_.robot_width = global["robot_width"].as<double>();
            if (global["damage_boost"]) global_.damage_boost = global["damage_boost"].as<double>();
            if (global["boost_duration"]) global_.boost_duration = global["boost_duration"].as<double>();
        }
        
        // 哨兵配置
        if (config["ally_sentry"]) {
            auto sentry = config["ally_sentry"];
            if (sentry["position"]) {
                sentry_.x = sentry["position"]["x"].as<double>();
                sentry_.y = sentry["position"]["y"].as<double>();
            }
            if (sentry["direction"]) sentry_.direction = sentry["direction"].as<double>();
            if (sentry["color"]) sentry_.color = sentry["color"].as<std::string>();
            if (sentry["movable"]) sentry_.movable = sentry["movable"].as<bool>();
            if (sentry["gimbal_direction"]) sentry_.gimbal_direction = sentry["gimbal_direction"].as<double>();
        }
        
        // 敌人配置
        if (config["enemy_robots"]) {
            for (const auto& enemy_node : config["enemy_robots"]) {
                RobotConfig enemy;
                
                if (enemy_node["position"]) {
                    enemy.x = enemy_node["position"]["x"].as<double>();
                    enemy.y = enemy_node["position"]["y"].as<double>();
                }
                if (enemy_node["direction"]) enemy.direction = enemy_node["direction"].as<double>();
                if (enemy_node["color"]) enemy.color = enemy_node["color"].as<std::string>();
                if (enemy_node["movable"]) enemy.movable = enemy_node["movable"].as<bool>();
                if (enemy_node["health"]) enemy.health = enemy_node["health"].as<double>();
                if (enemy_node["max_health"]) enemy.max_health = enemy_node["max_health"].as<double>();
                
                if (enemy_node["movement"]) {
                    auto movement = enemy_node["movement"];
                    if (movement["x"]) enemy.movement_x = movement["x"].as<std::string>();
                    if (movement["y"]) enemy.movement_y = movement["y"].as<std::string>();
                    if (movement["dir"]) enemy.movement_dir = movement["dir"].as<std::string>();
                }
                
                if (enemy_node["chassis_rotation_function"]) {
                    enemy.chassis_rotation = enemy_node["chassis_rotation_function"].as<std::string>();
                }
                
                if (enemy_node["armors"]) {
                    for (const auto& armor_node : enemy_node["armors"]) {
                        ArmorConfig armor;
                        if (armor_node["side"]) armor.side = armor_node["side"].as<std::string>();
                        if (armor_node["color"]) armor.color = armor_node["color"].as<std::string>();
                        if (armor_node["number"]) armor.number = armor_node["number"].as<int>();
                        enemy.armors.push_back(armor);
                    }
                }
                
                enemies_.push_back(enemy);
            }
        }
        
        return true;
    } catch (const YAML::Exception& e) {
        std::cerr << "Failed to load config file: " << e.what() << std::endl;
        return false;
    }
}

void Config::loadDefault() {
    global_ = GlobalConfig();
    
    sentry_.x = 2.0;
    sentry_.y = 2.0;
    sentry_.direction = 0.0;
    sentry_.color = "blue";
    sentry_.movable = true;
    sentry_.gimbal_direction = 0.0;
    
    RobotConfig enemy;
    enemy.x = 10.0;
    enemy.y = 8.0;
    enemy.direction = 0.0;
    enemy.color = "red";
    enemy.movable = true;
    enemy.health = 600.0;
    enemy.max_health = 600.0;
    
    ArmorConfig armor1{"front", "red", 1};
    ArmorConfig armor2{"left", "red", 2};
    ArmorConfig armor3{"back", "red", 3};
    ArmorConfig armor4{"right", "red", 4};
    
    enemy.armors = {armor1, armor2, armor3, armor4};
    enemies_ = {enemy};
}
