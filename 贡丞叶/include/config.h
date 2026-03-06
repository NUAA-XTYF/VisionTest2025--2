#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>
#include <memory>
#include <optional>

struct GlobalConfig {
    double map_width = 28.0;
    double map_height = 15.0;
    double base_damage = 100.0;
    double damage_distance_factor = 0.001;
    double armor_plate_width = 0.14;
    double robot_length = 0.6;
    double robot_width = 0.45;
    double damage_boost = 2.0;
    double boost_duration = 5.0;
};

struct ArmorConfig {
    std::string side;
    std::string color;
    int number = 0;
};

struct RobotConfig {
    double x = 0.0;
    double y = 0.0;
    double direction = 0.0;
    std::string color;
    bool movable = false;
    double health = 600.0;
    double max_health = 600.0;
    std::vector<ArmorConfig> armors;
    std::optional<std::string> movement_x;
    std::optional<std::string> movement_y;
    std::optional<std::string> movement_dir;
    std::optional<std::string> chassis_rotation;
};

struct SentryConfig {
    double x = 0.0;
    double y = 0.0;
    double direction = 0.0;
    std::string color;
    bool movable = false;
    std::optional<double> gimbal_direction;
};

struct EnergyConfig {
    double x = 0.0;
    double y = 0.0;
    double direction = 0.0;
    double width = 1.0;
    std::optional<double> damage_boost;
    std::optional<double> boost_duration;
};

struct ObstacleConfig {
    double x = 0.0;
    double y = 0.0;
    double direction = 0.0;
    double width = 0.5;
    std::string material = "concrete";
    bool destructible = false;
};

class Config {
public:
    static Config& getInstance() {
        static Config instance;
        return instance;
    }
    
    bool loadFromFile(const std::string& filename);
    void loadDefault();
    
    const GlobalConfig& getGlobal() const { return global_; }
    const SentryConfig& getSentry() const { return sentry_; }
    const std::vector<RobotConfig>& getEnemies() const { return enemies_; }
    const std::vector<EnergyConfig>& getEnergies() const { return energies_; }
    const std::vector<ObstacleConfig>& getObstacles() const { return obstacles_; }
    
private:
    Config() = default;
    ~Config() = default;
    
    GlobalConfig global_;
    SentryConfig sentry_;
    std::vector<RobotConfig> enemies_;
    std::vector<EnergyConfig> energies_;
    std::vector<ObstacleConfig> obstacles_;
};

#endif
