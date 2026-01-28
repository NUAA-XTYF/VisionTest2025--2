#ifndef COLORED_OBJECT_H
#define COLORED_OBJECT_H

#include <string>
#include <map>

enum class ObjectColor {
    RED,
    BLUE,
    UNKNOWN
};

inline std::string colorToString(ObjectColor color) {
    static const std::map<ObjectColor, std::string> colorMap = {
        {ObjectColor::RED, "RED"},
        {ObjectColor::BLUE, "BLUE"},
        {ObjectColor::UNKNOWN, "UNKNOWN"}
    };
    
    auto it = colorMap.find(color);
    return it != colorMap.end() ? it->second : "UNKNOWN";
}

inline ObjectColor stringToColor(const std::string& str) {
    static const std::map<std::string, ObjectColor> strMap = {
        {"red", ObjectColor::RED},
        {"Red", ObjectColor::RED},
        {"RED", ObjectColor::RED},
        {"blue", ObjectColor::BLUE},
        {"Blue", ObjectColor::BLUE},
        {"BLUE", ObjectColor::BLUE}
    };
    
    auto it = strMap.find(str);
    return it != strMap.end() ? it->second : ObjectColor::UNKNOWN;
}

class ColoredObject {
public:
    ColoredObject(ObjectColor color) : color_(color) {}
    virtual ~ColoredObject() = default;
    
    ObjectColor getColor() const { return color_; }
    void setColor(ObjectColor color) { color_ = color; }
    
protected:
    ObjectColor color_;
};

#endif
