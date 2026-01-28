#pragma once

#include<string>

namespace RobotSimulation{

//颜色
enum class ObjectColor{
    Red,
    Blue
};

//装甲板方位
enum class ArmorSide{
    Front,
    Back,
    Left,
    Right,
    Top,
    Bottom
};

//弹药类型
enum class AmmoType{
    Normal_17,
    Normal_42,
    HighLight_17,
    HighLight_42
};

//字符串转换
std::string ColourToString(ObjectColor colour);
ObjectColor StringToColour(const std::string& str);
std::string TypeToString(AmmoType ammo);
AmmoType StringToAmmo(const std::string& str);
std::string SideToString(ArmorSide side);
ArmorSide StringToSide(const std::string& str);

//设置常值
constexpr double PI=3.1415926;
constexpr double DEG_TO_RAD=PI/180.0;
constexpr double RAD_TO_DEG=180.0/PI;

}