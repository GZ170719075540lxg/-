# 机械臂 Actor 使用指南

## 概述

这是一个完整的机械臂 Actor 类，可以在 Unreal Engine 中创建和控制机械臂。该机械臂具有 5 个自由度，支持正向运动学和简化逆运动学。

## 文件结构

```
Characters/
└── RoboticArm/
    ├── RoboticArm.h      # 头文件 - 声明类和结构体
    └── RoboticArm.cpp    # 实现文件 - 具体功能实现
```

## 主要特性

### 1. 机械臂结构

- **BaseComp**: 底座组件
- **Joint1Comp**: 第一关节
- **Joint2Comp**: 第二关节
- **Joint3Comp**: 第三关节
- **GripperComp**: 抓手组件

### 2. 关节系统

每个关节包含以下属性：

- 关节名称
- 当前角度
- 角度限制范围
- 旋转轴选择(X/Y/Z)
- 启用状态

### 3. 控制方式

#### 蓝图可调用函数：

- `ExecuteCommand()` - 执行字符串命令
- `MoveToEndEffectorPosition()` - 移动到指定位置
- `RotateJoint()` - 旋转指定关节
- `ResetToInitialPosition()` - 重置到初始位置
- `HomePosition()` - 回到原点
- `GrabObject()` - 抓取物体
- `ReleaseObject()` - 释放物体

#### 状态查询函数：

- `GetEndEffectorPosition()` - 获取末端执行器位置
- `IsIdle()` - 检查是否空闲

## 使用方法

### 1. 在关卡中放置机械臂

1. 在内容浏览器中右键创建 Actor
2. 选择 ARoboticArm 类
3. 拖拽到场景中

### 2. 配置机械臂外观

```cpp
// 在构造函数或BeginPlay中设置网格
UStaticMesh* BaseMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/Meshes/BaseMesh.BaseMesh"));
if (BaseMesh)
{
    BaseComp->SetStaticMesh(BaseMesh);
}
```

### 3. 蓝图控制示例

#### 字符串命令控制：

```
// 移动到坐标(100, 50, 80)
ExecuteCommand("MOVE_TO 100,50,80")

// 旋转底座到45度
ExecuteCommand("ROTATE_JOINT Base 45")

// 重置机械臂
ExecuteCommand("RESET")
```

#### 直接函数调用：

```
// 移动到指定位置
MoveToEndEffectorPosition(Vector(100, 50, 80))

// 旋转第一个关节到30度
RotateJoint(0, 30.0)

// 回到原点
HomePosition()
```

## 参数配置

### 可编辑参数：

- **MoveSpeed**: 移动速度 (单位/秒)
- **RotationSpeed**: 旋转速度 (度/秒)
- **bEnablePhysics**: 是否启用物理模拟
- **Joints**: 关节数组配置

### 关节配置示例：

```cpp
// 底座关节配置
Joints[0].JointName = "Base"
Joints[0].MinAngle = -180.0
Joints[0].MaxAngle = 180.0
Joints[0].RotationAxis = 2  // Z轴旋转
```

## 命令语法

### 支持的命令格式：

1. **移动命令**

   ```
   MOVE_TO x,y,z
   示例: MOVE_TO 100,50,80
   ```

2. **关节旋转命令**

   ```
   ROTATE_JOINT 关节名 角度
   示例: ROTATE_JOINT Base 45
   ```

3. **预设位置命令**
   ```
   RESET     // 重置到初始位置
   HOME      // 回到原点
   GRAB      // 抓取物体
   RELEASE   // 释放物体
   ```

## 扩展建议

### 1. 添加更多关节

在 InitializeJoints()函数中添加新的关节配置

### 2. 改进逆运动学

实现更精确的逆运动学算法来计算关节角度

### 3. 添加路径规划

实现平滑的轨迹规划功能

### 4. 物理交互

添加真实的物理抓取和碰撞检测

### 5. 动画支持

集成动画蓝图实现更自然的运动

## 注意事项

1. **性能考虑**：每帧都会计算关节位置，大量机械臂可能影响性能
2. **精度限制**：当前逆运动学实现较为简化
3. **碰撞检测**：需要手动添加适当的碰撞体积
4. **资源管理**：记得为各个组件分配合适的静态网格资源

## 调试功能

启用详细日志输出：

```cpp
// 在项目设置中启用LogTemp类别
// 或在控制台输入: log LogTemp Verbose
```

可视化调试：

```cpp
// 代码中包含了DrawDebugHelpers调用
// 可以在视口中看到调试线条和点
```

这个机械臂 Actor 提供了完整的基础框架，可以根据具体需求进行定制和扩展。
