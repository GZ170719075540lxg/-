// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/StaticMesh.h"
#include "RoboticArm.generated.h"

/**
 * 机械臂关节数结构体
 */
USTRUCT(BlueprintType)
struct FRoboticArmJoint
{
    GENERATED_BODY()

public:
    // 关节名称
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Joint")
    FString JointName;

    // 关节组件引用
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Joint")
    TObjectPtr<USceneComponent> JointComponent;

    // 当前角度
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Joint")
    float CurrentAngle;

    // 目标角度
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Joint")
    float TargetAngle;

    // 旋转速度
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Joint")
    float RotationSpeed;

    // 最小角度限制
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Joint")
    float MinAngle;

    // 最大角度限制
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Joint")
    float MaxAngle;

    // 旋转轴 (0=X, 1=Y, 2=Z)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Joint")
    int32 RotationAxis;

    // 是否启用
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Joint")
    bool bEnabled;

    FRoboticArmJoint()
        : JointName(""), JointComponent(nullptr), CurrentAngle(0.0f), TargetAngle(0.0f), RotationSpeed(90.0f), MinAngle(-180.0f), MaxAngle(180.0f), RotationAxis(2) // 默认绕Z轴旋转
          ,
          bEnabled(true)
    {
    }
};

/**
 * 机械臂控制命令枚举
 */
UENUM(BlueprintType)
enum class ERoboticArmCommand : uint8
{
    RAC_MoveToPosition UMETA(DisplayName = "移动到位置"),
    RAC_RotateJoint UMETA(DisplayName = "旋转关节"),
    RAC_Reset UMETA(DisplayName = "重置位置"),
    RAC_Home UMETA(DisplayName = "回到原点"),
    RAC_Grab UMETA(DisplayName = "抓取"),
    RAC_Release UMETA(DisplayName = "释放")
};

UCLASS()
class RPG_TU2_API ARoboticArm : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ARoboticArm();

    // protected:
    //     // Called when the game starts or when spawned
    //     virtual void BeginPlay() override;

    // public:
    //     // Called every frame
    //     virtual void Tick(float DeltaTime) override;

    /*
     * 机械臂基础组件
     */
public:
    // 根组件
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<USceneComponent> RootComp;

    // 底座组件
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> BaseComp;

    // 第一关节组件
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> Joint1Comp;

    // 第二关节组件
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> Joint2Comp;

    // 第三关节组件
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> Joint3Comp;
    // 第四关节组件
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> Joint4Comp;
    // 第五关节组件
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> Joint5Comp;
    // 第六关节组件
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> Joint6Comp;

    /*
     * 机械臂配置参数
     */
public:
    // 机械臂关节数组
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    TArray<FRoboticArmJoint> Joints;

    // 移动速度
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration",
              meta = (DisplayName = "移动速度"))
    float MoveSpeed;

    // 旋转速度
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration",
              meta = (DisplayName = "旋转速度"))
    float RotationSpeed;

    // 是否启用物理模拟
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration",
              meta = (DisplayName = "启用物理"))
    bool bEnablePhysics;

    /*
     * 当前状态
     */
public:
    // 是否正在移动
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
    bool bIsMoving;

    // 是否正在旋转
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
    bool bIsRotating;

    /*
     * 控制函数
     */
public:
    // 关节1运动控制
    UFUNCTION(BlueprintCallable, Category = "RoboticArm|JointControl")
    void RotateJoint1(bool bClockwise);

    // 关节2运动控制
    UFUNCTION(BlueprintCallable, Category = "RoboticArm|JointControl")
    void RotateJoint2(bool bClockwise);

    // 关节3运动控制
    UFUNCTION(BlueprintCallable, Category = "RoboticArm|JointControl")
    void RotateJoint3(bool bClockwise);

    // 关节4运动控制
    UFUNCTION(BlueprintCallable, Category = "RoboticArm|JointControl")
    void RotateJoint4(bool bClockwise);

    // 关节5运动控制
    UFUNCTION(BlueprintCallable, Category = "RoboticArm|JointControl")
    void RotateJoint5(bool bClockwise);
    // 关节6运动控制
    UFUNCTION(BlueprintCallable, Category = "RoboticArm|JointControl")
    void RotateJoint6(bool bClockwise);
    /*
     * 根据字符串执行命令
     */
    UFUNCTION(BlueprintCallable, Category = "RoboticArm|Control")
    void ExecuteCommand(FString Command);
};