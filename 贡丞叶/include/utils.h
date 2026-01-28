#ifndef UTILS_H
#define UTILS_H

#include "ammo_type.h"
#include "colored_object.h"
#include "armor_plate.h"
#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

inline std::string sideToString(ArmorSide side) {
    switch (side) {
        case ArmorSide::FRONT: return "FRONT";
        case ArmorSide::BACK: return "BACK";
        case ArmorSide::LEFT: return "LEFT";
        case ArmorSide::RIGHT: return "RIGHT";
        default: return "UNKNOWN";
    }
}

inline ArmorSide stringToSide(const std::string& str) {
    if (str == "front" || str == "Front" || str == "FRONT") 
        return ArmorSide::FRONT;
    if (str == "back" || str == "Back" || str == "BACK") 
        return ArmorSide::BACK;
    if (str == "left" || str == "Left" || str == "LEFT") 
        return ArmorSide::LEFT;
    if (str == "right" || str == "Right" || str == "RIGHT") 
        return ArmorSide::RIGHT;
    return ArmorSide::FRONT;
}

inline std::string getCurrentTimeString() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&time);
    
    std::ostringstream oss;
    oss << std::put_time(&tm, "%H:%M:%S");
    
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    oss << "." << std::setfill('0') << std::setw(3) << ms.count();
    return oss.str();
}

inline std::string formatString(const std::string& format) {
    return format;
}

template<typename T, typename... Args>
std::string formatString(const std::string& format, T value, Args... args) {
    std::ostringstream oss;
    size_t pos = format.find("{}");
    if (pos != std::string::npos) {
        oss << format.substr(0, pos) << value;
        std::string remaining = format.substr(pos + 2);
        if constexpr (sizeof...(Args) > 0) {
            oss << formatString(remaining, args...);
        } else {
            oss << remaining;
        }
        return oss.str();
    }
    return format;
}

#endif
