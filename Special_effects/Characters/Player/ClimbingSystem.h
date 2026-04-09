// ClimbingSystem.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "ClimbingSystem.generated.h"

// 前向声明
class AOperation_character;

// 枚举类记录当前攀爬状态
UENUM(BlueprintType)
enum class EClimbingState : uint8
{
    Idle,    // 空闲状态
    Climbing // 攀爬状态
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class RPG_TU2_API UClimbingSystem : public UActorComponent
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly, Category = "Climbing")
    EClimbingState ClimbingState = EClimbingState::Idle;

    // 攀爬速度（用于动画）
    UPROPERTY(BlueprintReadOnly, Category = "Climbing")
    float CurrentClimbSpeed = 0.0f;

    // 垂直攀爬速度（用于动画）
    UPROPERTY(BlueprintReadOnly, Category = "Climbing")
    float CurrentClimbVSpeed = 0.0f;

    UClimbingSystem();
    /// @brief 是否进入攀爬状态
    /// @param TargetActor 目标角色
    /// @param MoveValue 鼠标移动值
    /// @return 是否进入攀爬状态
    UFUNCTION(BlueprintCallable, Category = "Climbing")
    bool ComeinClimb(AOperation_character *TargetActor, FVector MoveValue);

    /// @brief 获取当前攀爬状态
    /// @return 当前攀爬状态
    UFUNCTION(BlueprintCallable, Category = "Climbing")
    EClimbingState GetCurrentState() const { return ClimbingState; }

    /// @brief 开始攀爬
    /// @param TargetActor 目标角色
    /// @param MoveValue 移动方向
    UFUNCTION(BlueprintCallable, Category = "Climbing")
    void StartClimb(AOperation_character *TargetActor, FVector MoveValue);

    /// @brief 停止攀爬
    /// @param TargetActor 目标角色
    UFUNCTION(BlueprintCallable, Category = "Climbing")
    void StopClimb(AOperation_character *TargetActor);

    /// @brief 攀爬状态下的移动逻辑
    /// @param TargetActor 目标角色
    /// @param MoveValue 移动输入值（X为左右，Y为上下）
    UFUNCTION(BlueprintCallable, Category = "Climbing")
    void ClimbingMove(AOperation_character *TargetActor, FVector2D MoveValue);

    /// @brief 规范角色位置，使角色贴合物体表面
    /// @param TargetActor 目标角色
    /// @return 是否成功贴合表面
    UFUNCTION(BlueprintCallable, Category = "Climbing")
    bool SnapToSurface(AOperation_character *TargetActor);

    /// @brief 平滑旋转角色朝向
    /// @param TargetActor 目标角色
    /// @param FromDirection 起始方向向量
    /// @param ToDirection 目标方向向量
    /// @param RotationSpeed 旋转速度（0-1之间，值越大旋转越快）
    /// @return 是否完成旋转
    UFUNCTION(BlueprintCallable, Category = "Climbing")
    bool SmoothRotateToDirection(AOperation_character *TargetActor, const FVector &FromDirection, const FVector &ToDirection, float RotationSpeed);

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
};
