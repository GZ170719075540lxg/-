# 机械臂控制工具类使用指南

## 概述

本工具类提供了一套完整的机械臂控制功能，可以通过输入 Actor 对象和字符串命令来控制机械臂执行各种操作。

## 核心功能

### 1. 主要控制函数

#### ControlRoboticArm

通过字符串命令控制机械臂执行操作

```cpp
// C++ 使用示例
AActor* MyRoboticArm = /* 获取机械臂Actor */;
FString Command = "MoveToPosition 100 200 150";
bool Success = UMyBlueprintFunctionLibrary::ControlRoboticArm(MyRoboticArm, Command);
```

```blueprint
// Blueprint 使用示例
RoboticArmActor = GetYourRoboticArmActor()
CommandString = "MoveToPosition 100 200 150"
Success = ControlRoboticArm(RoboticArmActor, CommandString)
```

### 2. 支持的命令类型

#### 移动命令

- **MoveToPosition**: 移动到指定位置
  ```
  "MoveToPosition 100 200 150"  // 移动到坐标(100, 200, 150)
  "move position 100 200 150"   // 同样有效
  ```

#### 关节控制命令

- **RotateJoint**: 旋转指定关节
  ```
  "RotateJoint Shoulder 45"     // 旋转肩部关节到45度
  "rotate joint Elbow -30"      // 旋转肘部关节到-30度
  ```

#### 物体操作命令

- **GrabObject**: 抓取物体

  ```
  "GrabObject Cube"             // 抓取标签为Cube的物体
  "grab box"                    // 抓取标签为box的物体
  ```

- **ReleaseObject**: 释放物体
  ```
  "ReleaseObject"               // 释放当前抓取的所有物体
  "release"                     // 同样有效
  ```

#### 位置重置命令

- **HomePosition**: 回到初始位置
  ```
  "HomePosition"                // 移动到预设的初始位置
  "home"                        // 同样有效
  "reset"                       // 同样有效
  ```

#### 停止命令

- **Stop**: 停止所有运动
  ```
  "Stop"                        // 立即停止所有运动
  "stop"                        // 同样有效
  "halt"                        // 同样有效
  ```

### 3. 直接控制函数

#### 位置控制

```cpp
// 直接移动到指定位置
FVector TargetPos(100.0f, 200.0f, 150.0f);
bool Success = UMyBlueprintFunctionLibrary::MoveRoboticArmToPosition(MyRoboticArm, TargetPos);
```

#### 关节旋转

```cpp
// 旋转特定关节
bool Success = UMyBlueprintFunctionLibrary::RotateRoboticArmJoint(MyRoboticArm, "Shoulder", 45.0f);
```

#### 物体抓取/释放

```cpp
// 抓取物体
AActor* ObjectToGrab = /* 获取要抓取的物体 */;
bool GrabSuccess = UMyBlueprintFunctionLibrary::GrabObjectWithRoboticArm(MyRoboticArm, ObjectToGrab);

// 释放物体
bool ReleaseSuccess = UMyBlueprintFunctionLibrary::ReleaseObjectFromRoboticArm(MyRoboticArm);
```

#### 状态查询

```cpp
// 获取关节信息
TArray<FRoboticArmJoint> Joints = UMyBlueprintFunctionLibrary::GetRoboticArmJointInfo(MyRoboticArm);

// 设置多个关节角度
TMap<FString, float> JointAngles;
JointAngles.Add("Shoulder", 30.0f);
JointAngles.Add("Elbow", -45.0f);
JointAngles.Add("Wrist", 90.0f);
bool Success = UMyBlueprintFunctionLibrary::SetRoboticArmJointAngles(MyRoboticArm, JointAngles);
```

## 使用示例

### Blueprint 示例

```
1. 获取机械臂Actor引用
2. 调用ControlRoboticArm函数
3. 传入命令字符串
4. 处理返回的成功/失败状态
```

### C++示例

```cpp
// 示例：完整的机械臂控制流程
void ARobotController::ControlMechanicalArm()
{
    AActor* RoboticArm = GetRoboticArmActor();

    // 移动到工作位置
    UMyBlueprintFunctionLibrary::ControlRoboticArm(RoboticArm, "MoveToPosition 200 100 80");

    // 旋转关节准备抓取
    UMyBlueprintFunctionLibrary::ControlRoboticArm(RoboticArm, "RotateJoint Wrist 45");

    // 抓取物体
    UMyBlueprintFunctionLibrary::ControlRoboticArm(RoboticArm, "GrabObject TargetBox");

    // 移动到放置位置
    UMyBlueprintFunctionLibrary::ControlRoboticArm(RoboticArm, "MoveToPosition -100 150 120");

    // 释放物体
    UMyBlueprintFunctionLibrary::ControlRoboticArm(RoboticArm, "ReleaseObject");

    // 回到初始位置
    UMyBlueprintFunctionLibrary::ControlRoboticArm(RoboticArm, "HomePosition");
}
```

## 注意事项

1. **Actor 验证**: 所有函数都会自动验证输入的 Actor 是否有效
2. **参数格式**: 命令字符串中的参数需要用空格分隔
3. **坐标系**: 位置参数使用世界坐标系
4. **角度单位**: 关节角度使用度数制
5. **物体识别**: 抓取物体时通过 Tag 标签匹配物体

## 错误处理

函数会自动记录日志信息：

- 警告级别：参数错误、对象未找到等
- 信息级别：操作成功执行的详细信息

建议在生产环境中检查函数返回值并适当处理错误情况。
