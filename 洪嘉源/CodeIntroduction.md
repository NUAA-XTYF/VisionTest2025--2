# 文件构成

本项目主要依赖项为 yaml-cpp 和 exprtk，放置于package目录下。

- 源代码文件： VisionTest2.cpp 
- 头文件： VisionTest2.h 
- yaml 配置文件： config.yml ，通过命令行传递参数。
- 所生成的可执行文件： VisionTest2 ， 在目录 .\build 下。

# 源码组成

头文件中只包含类与函数的声明，源文件中包含具体实现。主要类与函数如下：

- EnumUtils: 枚举类型工具，提供枚举与字符串之间的转换功能。
- Object: 基类。
- ColoredObject: 继承自 Object，增加颜色属性。
- 简单的几何工具。
- RingBuffer: 环形缓冲区， 使用Template实现。
- calculateDamage: 简单伤害计算器，随距离衰减。
- Target: 继承自 ColoredObject。
- ArmorPlate: 继承自 Target，增加法向量获取函数与单面击打判定。
- EneergyMechanism: 继承自 Target，增加能量机制相关属性与函数。
- Obstacle: 继承自 Target。
- Robot: 继承自 ColoredObject 与 std::enable_shared_from_this，增加机器人相关属性与函数。
- EnemyRobot: 继承自 Robot，增加敌方机器人相关属性与函数。
- SentryRobot: 继承自 Robot，增加哨兵机器人相关属性与函数，实现友元输出。
- isArmorVisible: 判断装甲板是否可见的函数。
- main 函数：程序入口，包含配置文件读取与主要逻辑实现。

# 
