// Fill out your copyright notice in the Description page of Project Settings.

#include "RoboticArm.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ARoboticArm::ARoboticArm()
{
    // 设置此actor每帧调用
    PrimaryActorTick.bCanEverTick = true;

    // 创建根组件
    RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    SetRootComponent(RootComp);

    // 创建机械臂各部件组件
    BaseComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base"));
    BaseComp->SetupAttachment(RootComp);

    Joint1Comp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Joint1"));
    Joint1Comp->SetupAttachment(RootComp);

    Joint2Comp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Joint2"));
    Joint2Comp->SetupAttachment(Joint1Comp);

    Joint3Comp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Joint3"));
    Joint3Comp->SetupAttachment(Joint2Comp);

    Joint4Comp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Joint4"));
    Joint4Comp->SetupAttachment(Joint3Comp);

    Joint5Comp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Joint5"));
    Joint5Comp->SetupAttachment(Joint4Comp);

    Joint6Comp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Joint6"));
    Joint6Comp->SetupAttachment(Joint5Comp);
    // 机械臂对象
}
void ARoboticArm::RotateJoint1(bool bClockwise)
{
    // 空判断
    if (!Joint1Comp)
        return;
    // 当前角度
    FRotator CurrentRotation = Joint1Comp->GetRelativeRotation();
    if (bClockwise)
    {
        // 设置角度
        UE_LOG(LogTemp, Warning, TEXT("Rotating Joint1 Clockwise"));
        CurrentRotation.Yaw += 5;
        if (CurrentRotation.Yaw > 90)
        {
            CurrentRotation.Yaw = 90;
        }
        Joint1Comp->SetRelativeRotation(CurrentRotation);
    }
    else
    {
        CurrentRotation.Yaw -= 5;
        if (CurrentRotation.Yaw < -90)
        {
            CurrentRotation.Yaw = -90;
        }
        Joint1Comp->SetRelativeRotation(CurrentRotation);
    }
}
void ARoboticArm::RotateJoint2(bool bClockwise)
{
    // 使用四元数进行旋转控制
    FQuat CurrentQuat = Joint2Comp->GetRelativeRotation().Quaternion();

    // 定义绕Y轴的旋转四元数(关节2是俯仰运动)
    float RotationAngle = bClockwise ? 5.0f : -5.0f;
    FQuat DeltaRotation = FQuat(FVector(0, 1, 0), FMath::DegreesToRadians(RotationAngle));

    // 应用相对旋转
    FQuat NewQuat = CurrentQuat * DeltaRotation;

    // 转换回欧拉角进行限制检查
    FRotator NewRotation = NewQuat.Rotator();

    UE_LOG(LogTemp, Warning, TEXT("Rotating Joint2 - Current Pitch: %f, Target Pitch: %f"),
           Joint2Comp->GetRelativeRotation().Pitch, NewRotation.Pitch);

    // 角度限制检查
    if (NewRotation.Pitch >= 0 && NewRotation.Pitch <= 180)
    {
        // 在有效范围内，应用旋转
        Joint2Comp->SetRelativeRotation(NewRotation);
        UE_LOG(LogTemp, Warning, TEXT("Joint2 rotation applied: %f"), NewRotation.Pitch);
    }
    else
    {
        // 超出限制范围，保持当前位置
        UE_LOG(LogTemp, Warning, TEXT("Joint2 rotation limit reached, staying at: %f"),
               Joint2Comp->GetRelativeRotation().Pitch);
    }
}
void ARoboticArm::RotateJoint3(bool bClockwise)
{
    // 简化直接测试版本 - 通过实验确定正确行为
    FRotator CurrentRotation = Joint3Comp->GetRelativeRotation();
    float CurrentPitch = CurrentRotation.Pitch;

    UE_LOG(LogTemp, Warning, TEXT("=== Joint3 TEST ==="));
    UE_LOG(LogTemp, Warning, TEXT("Current Pitch: %f"), CurrentPitch);
    UE_LOG(LogTemp, Warning, TEXT("Input bClockwise: %s"), bClockwise ? TEXT("TRUE") : TEXT("FALSE"));

    // 直接尝试运动，观察实际效果
    float TestDelta = bClockwise ? 5.0f : -5.0f;
    float TargetPitch = CurrentPitch + TestDelta;

    UE_LOG(LogTemp, Warning, TEXT("Attempting delta: %f, Target: %f"), TestDelta, TargetPitch);

    // 应用硬限制
    float FinalPitch = FMath::Clamp(TargetPitch, -90.0f, 90.0f);

    // 检查是否真的要移动
    if (FMath::Abs(FinalPitch - CurrentPitch) < 0.1f)
    {
        UE_LOG(LogTemp, Warning, TEXT("NO MOVEMENT - Limit reached or no change"));
        return;
    }

    // 执行移动
    CurrentRotation.Pitch = FinalPitch;
    Joint3Comp->SetRelativeRotation(CurrentRotation);

    UE_LOG(LogTemp, Warning, TEXT("MOVED SUCCESSFULLY to: %f (delta applied: %f)"),
           FinalPitch, FinalPitch - CurrentPitch);
    UE_LOG(LogTemp, Warning, TEXT("=================="));
}

void ARoboticArm::RotateJoint4(bool bClockwise)
{
    // 当前角度
    FRotator CurrentRotation = Joint4Comp->GetRelativeRotation();
    if (bClockwise)
    {
        // 设置角度
        UE_LOG(LogTemp, Warning, TEXT("Rotating Joint4 Clockwise"));
        CurrentRotation.Pitch += 5;
        if (CurrentRotation.Pitch > 90)
        {
            CurrentRotation.Pitch = 90;
        }
        Joint4Comp->SetRelativeRotation(CurrentRotation);
    }
    else
    {
        CurrentRotation.Pitch -= 5;
        if (CurrentRotation.Pitch < -90)
        {
            CurrentRotation.Pitch = -90;
        }
        Joint4Comp->SetRelativeRotation(CurrentRotation);
    }
}

void ARoboticArm::RotateJoint5(bool bClockwise)
{
    // 当前角度
    FRotator CurrentRotation = Joint5Comp->GetRelativeRotation();
    if (bClockwise)
    {
        // 设置角度
        UE_LOG(LogTemp, Warning, TEXT("Rotating Joint5 Clockwise"));
        CurrentRotation.Yaw += 5;
        if (CurrentRotation.Yaw > 90)
        {
            CurrentRotation.Yaw = 90;
        }
        Joint5Comp->SetRelativeRotation(CurrentRotation);
    }
    else
    {
        CurrentRotation.Yaw -= 5;
        if (CurrentRotation.Yaw < -90)
        {
            CurrentRotation.Yaw = -90;
        }
        Joint5Comp->SetRelativeRotation(CurrentRotation);
    }
}
void ARoboticArm::RotateJoint6(bool bClockwise)
{
    // 当前角度
    FRotator CurrentRotation = Joint6Comp->GetRelativeRotation();
    if (bClockwise)
    {
        // 设置角度
        UE_LOG(LogTemp, Warning, TEXT("Rotating Joint6 Clockwise"));
        CurrentRotation.Roll += 5;
        if (CurrentRotation.Roll > 50)
        {
            CurrentRotation.Roll = 50;
        }
        Joint6Comp->SetRelativeRotation(CurrentRotation);
    }
    else
    {
        CurrentRotation.Roll -= 5;
        if (CurrentRotation.Roll < 0)
        {
            CurrentRotation.Roll = 0;
        }
        Joint6Comp->SetRelativeRotation(CurrentRotation);
    }
}

void ARoboticArm::ExecuteCommand(FString Command)
{
    // 解析命令并执行相应的操作
    /*
    1:关机正转
    q：关节1反转
    2:关节2正转
    w:关节2反转
    3:关节3正转
    e:关节3反转
    4:关节4正转
    r:关节4反转
    5:关节5正转
    t:关节5反转
    6:关节6正转
    y:关节6反转
    */
    UE_LOG(LogTemp, Warning, TEXT("服务端控制机械臂%s"), *Command);

    // 准确提取指令数字
    FString ServerCommand;

    // 查找"指令是"后面的数字
    FString SearchText = TEXT("指令是");
    int32 Index = Command.Find(SearchText);

    if (Index != INDEX_NONE)
    {
        // 从"指令是"之后开始查找数字
        int32 StartPos = Index + SearchText.Len();

        // 提取后续字符直到遇到非数字字符
        for (int32 i = StartPos; i < Command.Len(); i++)
        {
            TCHAR Char = Command[i];
            if (FChar::IsDigit(Char))
            {
                ServerCommand.AppendChar(Char);
            }
            else if (ServerCommand.Len() > 0)
            {
                // 已经找到数字，遇到非数字字符就停止
                break;
            }
        }
    }

    // 如果上面的方法没找到，尝试备用方案
    if (ServerCommand.IsEmpty())
    {
        // 直接取最后一个字符（假设是数字）
        ServerCommand = Command.Right(1);
    }

    UE_LOG(LogTemp, Warning, TEXT("提取的指令: %s"), *ServerCommand);
    if (ServerCommand == "1")
    {
        // 关机正转
        UE_LOG(LogTemp, Warning, TEXT("Executing Command: 1"));
        RotateJoint1(true);
    }
    else if (ServerCommand == "q")
    {
        // 关节1反转
        UE_LOG(LogTemp, Warning, TEXT("Executing Command: q"));
        RotateJoint1(false);
    }
    else if (ServerCommand == "2")
    {
        // 关节2正转
        UE_LOG(LogTemp, Warning, TEXT("Executing Command: 2"));
        RotateJoint2(true);
    }
    else if (ServerCommand == "w")
    {
        // 关节2反转
        UE_LOG(LogTemp, Warning, TEXT("Executing Command: w"));
        RotateJoint2(false);
    }
    else if (ServerCommand == "3")
    {
        // 关节3正转
        UE_LOG(LogTemp, Warning, TEXT("Executing Command: 3"));
        RotateJoint3(true);
    }
    else if (ServerCommand == "e")
    {
        // 关节3反转
        UE_LOG(LogTemp, Warning, TEXT("Executing Command: e"));
        RotateJoint3(false);
    }
    else if (ServerCommand == "4")
    {
        // 关节4正转
        UE_LOG(LogTemp, Warning, TEXT("Executing Command: 4"));
        RotateJoint4(true);
    }
    else if (ServerCommand == "r")
    {
        // 关节4反转
        UE_LOG(LogTemp, Warning, TEXT("Executing Command: r"));
        RotateJoint4(false);
    }
    else if (ServerCommand == "5")
    {
        // 关节5正转
        UE_LOG(LogTemp, Warning, TEXT("Executing Command: 5"));
        RotateJoint5(true);
    }
    else if (ServerCommand == "t")
    {
        // 关节5反转
        UE_LOG(LogTemp, Warning, TEXT("Executing Command: t"));
        RotateJoint5(false);
    }
    else if (ServerCommand == "6")
    {
        // 关节6正转
        UE_LOG(LogTemp, Warning, TEXT("Executing Command: 6"));
        RotateJoint6(true);
    }
    else if (ServerCommand == "y")
    {
        // 关节6反转
        UE_LOG(LogTemp, Warning, TEXT("Executing Command: y"));
        RotateJoint6(false);
    }
    else
    {
        // 无效命令
        UE_LOG(LogTemp, Warning, TEXT("无效命令:%s"), *ServerCommand);
    }
}
