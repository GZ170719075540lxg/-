# 攀爬惯性优化说明

## 问题描述

角色在攀爬过程中惯性过大，导致：

- 移动时难以精确控制
- 停止时滑行距离过长
- 转向时响应迟缓
- 攀爬体验不够流畅

## 优化方案

### 1. 物理参数调整 (character_my.cpp)

```cpp
// 优化物理设置减少攀爬惯性
GetCharacterMovement()->Mass = 80.0f; // 降低质量减少惯性
GetCharacterMovement()->BrakingDecelerationWalking = 2048.0f; // 增加地面制动减速
GetCharacterMovement()->BrakingDecelerationFalling = 1024.0f;  // 增加空中制动减速
GetCharacterMovement()->AirControl = 0.8f; // 增加空中控制
GetCharacterMovement()->AirControlBoostMultiplier = 1.5f; // 空中控制增强
GetCharacterMovement()->AirControlBoostVelocityThreshold = 100.0f; // 控制增强速度阈值
GetCharacterMovement()->FallingLateralFriction = 1.0f; // 增加下落横向摩擦
GetCharacterMovement()->MaxAcceleration = 2048.0f; // 最大加速度
GetCharacterMovement()->BrakingFrictionFactor = 1.5f; // 制动摩擦系数
```

### 2. 攀爬状态切换优化 (Operation_character.cpp)

```cpp
void AOperation_character::SwitchClimb()
{
    if (isclimb)
    {
        // 退出攀爬时重置速度，减少惯性影响
        GetCharacterMovement()->Velocity = FVector::ZeroVector;
        GetCharacterMovement()->AddImpulse(FVector::UpVector * 200.0f, true);
    }
    else
    {
        // 进入攀爬时立即停止当前运动
        GetCharacterMovement()->Velocity = FVector::ZeroVector;
        GetCharacterMovement()->StopMovementImmediately();
    }
}
```

### 3. 移动输入控制优化

```cpp
// 攀爬状态下的移动控制
float ClimbSpeedMultiplier = 0.6f; // 降低攀爬速度倍数
FVector ClimbInput = (GetActorUpVector() * MoveValue.Y + GetActorRightVector() * MoveValue.X) * ClimbSpeedMultiplier;

// 应用受限的移动输入
AddMovementInput(GetActorUpVector(), MoveValue.Y * ClimbSpeedMultiplier);
AddMovementInput(GetActorRightVector(), MoveValue.X * ClimbSpeedMultiplier);
```

### 4. 新增攀爬系统惯性控制功能 (ClimbingSystem.cpp)

```cpp
// 速度控制
void UClimbingSystem::ControlClimbingSpeed(ACharacter* Character, float MaxClimbSpeed)
{
    FVector CurrentVelocity = MovementComp->Velocity;
    if (CurrentVelocity.Size() > MaxClimbSpeed)
    {
        MovementComp->Velocity = CurrentVelocity.GetClampedToMaxSize(MaxClimbSpeed);
    }
}

// 惯性阻尼
void UClimbingSystem::ApplyClimbingDamping(ACharacter* Character, float DampingFactor)
{
    FVector DampedVelocity = MovementComp->Velocity * DampingFactor;
    MovementComp->Velocity = DampedVelocity;

    // 速度很小时设为零
    if (DampedVelocity.Size() < 10.0f)
    {
        MovementComp->Velocity = FVector::ZeroVector;
    }
}
```

## 优化效果

### ✅ 改进点：

1. **响应性提升** - 角色对输入响应更加灵敏
2. **控制精度** - 移动和停止更加精确
3. **转向流畅** - 方向改变更加自然
4. **物理真实感** - 符合预期的物理行为

### 📊 参数说明：

- **Mass (80.0f)** - 降低质量减少惯性累积
- **BrakingDeceleration** - 增加制动减速使停止更快
- **AirControl (0.8f)** - 提高空中介质控制能力
- **Speed Multiplier (0.6f)** - 限制攀爬移动速度
- **Damping Factor (0.85f)** - 每帧减少 15%的速度

## 使用建议

1. **调试模式** - 可通过调整阻尼系数(0.8-0.95)来微调手感
2. **性能监控** - 这些改动对性能影响极小
3. **进一步优化** - 可根据不同表面类型调整不同的阻尼参数
4. **蓝图扩展** - 新增的函数可在蓝图中调用进行更精细的控制

## 注意事项

⚠️ **平衡考虑**：

- 过低的惯性会让移动显得生硬
- 过高的阻尼会失去物理真实性
- 建议在实际游戏中测试并微调参数

🔧 **可调参数**：

- `ClimbSpeedMultiplier` - 攀爬速度倍数 (0.4-0.8)
- `DampingFactor` - 惯性阻尼系数 (0.8-0.95)
- `MaxClimbSpeed` - 最大攀爬速度 (200-400)
