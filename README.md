# VisionTest2025--2
[Readme.md](https://github.com/user-attachments/files/24354203/Readme.md)
# 哨兵机器人控制系统

## 背景
机甲大师哨兵需要自动巡逻、识别目标（装甲板、基地、能量机关）并发射弹丸。本项目要求以 YAML 配置驱动，完成机器人控制、伤害判定、多线程协同和现代 C++ 特性应用。

## 快速开始
- 构建：在 `build/` 目录运行 `cmake --build . --config Debug`（MinGW 需支持 `-Wa,-mbig-obj`）。
- 运行：`./Simulator.exe config.yaml`；若未提供外部配置，程序会加载内置示例 YAML 并可直接运行。
- 依赖：`yaml-cpp`、`exprtk`，C++20（若 `std::format` 不可用，请回退 `std::ostringstream`）。

## 当前实现要点
- 装甲板优先：每轮只在敌方装甲板上打击，使用 `isArmorHittable` 判断外侧可击性与朝向。
- 目标选择：按哨兵到装甲板的欧氏距离选最近可击目标，避免不可见或背面装甲。
- 连续循环：循环上限 2000 次；每轮 1ms 冷却，便于模拟移动目标。详见 [src/main.cpp](src/main.cpp#L1-L102)。
- 伤害结算：调用 `calculateDamage`（大弹丸）并叠加哨兵伤害加成后扣除敌人血量，打印命中日志。
- 配置容错：找不到外部 `config.yaml` 时加载内置示例配置，确保可运行。

## 任务分解（按搭建顺序）

### 1. 基础对象模型
- `Object`：公共基类，管理 ID、当前位置/朝向、可移动标记；若 `movable=true`，使用 exprtk 驱动 `movement` 的 x/y/dir 分量。
- `Target`：抽象目标，包含宽度、`GetBounds()`（返回端点）、虚析构和 `describe()`。
- 枚举与工具：`ColoredObject::ObjectColor`，`ArmorSide`、`AmmoType` 及其字符串转换。

### 2. 目标派生类
- `ArmorPlate`：颜色、编号、所属边；单面击打（外法向量点积 < 0）；持有宿主弱引用；位置由宿主姿态计算，不从 YAML 取绝对坐标。
- `EnergyMechanism`：可激活状态；命中提供伤害加成（倍数、持续时间可配置，缺省取全局）。
- `Obstacle`：遮挡体，含材质/可破坏性属性，仅用于视线遮挡。

### 3. 机器人层级与姿态
- `Robot`：继承 `Object` 与 `ColoredObject`，包含长宽、云台相对角、底盘小陀螺函数，持有装甲板并可 `UpdateArmors()`（底盘旋转 -> 世界旋转）。
- `EnemyRobot`：血量/最大血量，可被伤害；装甲板自 YAML 列表创建并随姿态更新。
- `SentryRobot`：我方哨兵，负责移动、开火、伤害加成；支持云台调整。

### 4. 静态成员与友元
- `SentryRobot` 维护 `totalTargetsDetected`、`instanceCount`，提供查询方法。
- 友元 `operator<<` 输出哨兵摘要。

### 5. 函数重载与接口
- `SentryRobot::fire()` 与 `fire(AmmoType)`；`move(length, direction)`；`changeDirection(changedRad)`。

### 6. 模板与通用工具
- `RingBuffer<T, N>`：环形缓冲区，含 push/pop/full/empty/size 与值访问。
- `calculateDamage`：按弹丸类型与距离计算伤害，支持坐标重载，包含距离衰减。
- 几何工具：角度归一化、距离、旋转、射线-线段相交、朝向换算。

### 7. 观测与判定模型
- 坐标系：世界系 $(x, y)$，$\theta=0$ 指向 +Y，逆时针为正；相对角用 $\operatorname{atan2}(dx, dy)$ 归一化到 $[-\pi, \pi)$。
- 装甲板绑定：局部四边中心，经底盘旋转和世界旋转得到世界坐标；外法向量用于单面击打判定（点积 < 0 表示外侧命中）。
- 遮挡与检测：障碍物可提供线段集合供射线相交；检测接口可返回目标可见角域、距离（可扩展实现）。

### 8. 伤害与血量流程
- 敌方血量初始化自 YAML；`TakeDamage` 归零则死亡。
- 伤害 = `calculateDamage` × 当前伤害加成；能量机关命中触发 `ActivateDamageBoost(multiplier, duration)`。
- 单面击打使用外法向量点积判定；能量机关双面可击。

### 9. STL 与智能指针
- 使用 `std::vector` 管理目标，`std::map` 管理弹药库存。
- `std::unique_ptr`/`std::shared_ptr`/`std::weak_ptr` 负责检测模块、共享数据和状态观察；演示移动语义可转移控制权。

### 10. 可扩展建议：多线程与同步
- 当前实现为单线程主循环；如需扩展检测/控制/通信线程，建议用 `std::mutex` + `std::lock_guard` 保护共享容器。
- 需加锁的共享数据：目标列表、观测结果、状态共享数据。

### 11. 现代 C++ 特性
- C++17：`std::optional`、结构化绑定、`if constexpr`、内联变量。
- C++20（可选）：概念、ranges、三路比较、指定初始化器、协程、`std::format`（若编译器不支持请回退 `std::ostringstream`）。

### 12. YAML 配置与全局参数
- 使用 yaml-cpp 读取场景：哨兵、敌人、能量机关、障碍物、全局参数。
- exprtk 解析时间函数，支持变量 `t`、常量 `pi`，用于轨迹与小陀螺。
- 必填字段：
	- `ally_sentry.position{ x, y }`，`direction{ x, y }`，`color`，`movable`；可选 `gimbal_direction`。
	- `enemy_robots[]`：`position`、`direction`、`color`、`movable`、`health`、`max_health`、`armors[]`（side/color/number）；可选 `movement`（x/y/dir 分量表达式）、`chassis_rotation_function`。
	- `energy_mechanisms[]`：`position`、`direction`、`width`；可选 `damage_boost`、`boost_duration`。
	- `obstacles[]`：`position`、`direction`、`width`；可选 `material`、`destructible`。
	- `global`：`map_width`、`map_height`、`base_damage`、`damage_distance_factor`、`armor_plate_width`、`robot_length`、`robot_width`、`damage_boost`、`boost_duration`。

### 13. 运行流程与日志
- 启动加载 `config.yaml`；缺失时使用内置示例 YAML，确保可运行。
- 初始化哨兵与敌人，调用 `UpdateArmors()` 同步装甲板到当前姿态。
- 每轮循环（最多 2000 次）：筛选所有存活敌人的可击装甲板，按距离选最近；若无可击装甲板则等待 1ms 后重试。
- 命中时调用 `fire()`，以大弹丸计算伤害并乘以哨兵伤害加成，再对装甲所属敌人扣血并打印命中日志。
- 轮末固定 1ms 冷却，便于移动目标获得新的姿态；所有敌人死亡或达上限即终止，汇总射击次数。

## 验收检查清单
- 可编译并运行，外部缺省时自动加载内置示例 YAML。
- 装甲板随姿态更新并进行单面击打判定，可击性由 `isArmorHittable` 判定朝向。
- 循环 1ms 冷却，按最近可击装甲板射击；所有敌人被击毁或迭代达上限即结束。
- 伤害包含距离衰减与哨兵伤害加成；命中日志输出装甲面、距离、伤害与血量。
- 弹药库存、静态计数器、友元输出按要求工作。
- 若扩展多线程，关键共享数据需加锁；现代 C++17/20 特性使用合理，`std::format` 兼容性已考虑。

## 评分要点
- 继承体系与功能完整度（Object → Target/Robot → 派生类）。
- 静态成员、函数重载、友元输出的正确实现。
- YAML 配置加载与 exprtk 驱动运动的有效性。
- 伤害与血量系统（单面/双面击打、加成）的准确性。
- STL、智能指针、多线程同步的合理使用与并发安全。
- 现代 C++17/20 特性的应用质量与编译兼容性（`std::format` 不可用时提供回退）。
- 日志输出和整体运行连贯性。
