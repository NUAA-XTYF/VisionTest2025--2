#ifndef AMMO_TYPE_H
#define AMMO_TYPE_H

#include <string>
#include <map>
#include <stdexcept>

enum class AmmoType {
    SMALL,   // 小弹丸
    LARGE,   // 大弹丸
    UNKNOWN  // 未知类型
};

// AmmoType 到字符串的转换
inline std::string ammoTypeToString(AmmoType type) {
    static const std::map<AmmoType, std::string> typeMap = {
        {AmmoType::SMALL, "SMALL"},
        {AmmoType::LARGE, "LARGE"},
        {AmmoType::UNKNOWN, "UNKNOWN"}
    };
    
    auto it = typeMap.find(type);
    return it != typeMap.end() ? it->second : "UNKNOWN";
}

// 字符串到 AmmoType 的转换
inline AmmoType stringToAmmoType(const std::string& str) {
    static const std::map<std::string, AmmoType> strMap = {
        {"small", AmmoType::SMALL},
        {"Small", AmmoType::SMALL},
        {"SMALL", AmmoType::SMALL},
        {"large", AmmoType::LARGE},
        {"Large", AmmoType::LARGE},
        {"LARGE", AmmoType::LARGE}
    };
    
    auto it = strMap.find(str);
    return it != strMap.end() ? it->second : AmmoType::UNKNOWN;
}

// AmmoType 输出运算符重载
inline std::ostream& operator<<(std::ostream& os, AmmoType type) {
    return os << ammoTypeToString(type);
}

#endif // AMMO_TYPE_H