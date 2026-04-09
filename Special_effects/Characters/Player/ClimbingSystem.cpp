// ClimbingSystem.cpp
#include "ClimbingSystem.h"
#include "Operation_character.h"
#include "GameFramework/CharacterMovementComponent.h"

UClimbingSystem::UClimbingSystem()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UClimbingSystem::BeginPlay()
{
    Super::BeginPlay();
}

void UClimbingSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // 在攀爬状态下，如果没有输入，则清除Velocity以防止滑行
    if (ClimbingState == EClimbingState::Climbing)
    {
        AOperation_character *Owner = Cast<AOperation_character>(GetOwner());
        if (Owner && Owner->GetCharacterMovement())
        {
            // 只在没有攀爬输入时清除速度
            if (FMath::IsNearlyZero(CurrentClimbSpeed) && FMath::IsNearlyZero(CurrentClimbVSpeed))
            {
                Owner->GetCharacterMovement()->Velocity = FVector::ZeroVector;
            }
        }
    }
}

bool UClimbingSystem::ComeinClimb(AOperation_character *TargetActor, FVector MoveValue)
{
    if (!TargetActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("[攀爬检测] 目标角色为空"));
        return false;
    }

    // 获取摄像机组件
    UCameraComponent *CameraComp = TargetActor->FindComponentByClass<UCameraComponent>();
    if (!CameraComp)
    {
        UE_LOG(LogTemp, Warning, TEXT("[攀爬检测] 未找到摄像机组件"));
        return false;
    }

    // 获取摄像机方向并将Z设置为0（只保留水平方向）
    FVector CameraForward = CameraComp->GetForwardVector();
    CameraForward.Z = 0.0f;
    CameraForward.Normalize();

    // 根据鼠标值计算新方向：X为正右，Y为正前
    FVector RightVector = CameraComp->GetRightVector();
    RightVector.Z = 0.0f;
    RightVector.Normalize();

    // 计算最终方向：Y(前) * MoveValue.Y + X(右) * MoveValue.X
    FVector NewDirection = (CameraForward * MoveValue.Y + RightVector * MoveValue.X).GetSafeNormal();

    if (NewDirection.IsNearlyZero())
    {
        UE_LOG(LogTemp, Warning, TEXT("[攀爬检测] 移动方向为零向量"));
        return false;
    }

    // 发射射线检测
    UWorld *World = TargetActor->GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("[攀爬检测] World为空"));
        return false;
    }

    FVector BaseLocation = TargetActor->GetActorLocation();
    float CheckDistance = 80.0f;
    FVector EndLocation = BaseLocation + NewDirection * CheckDistance;

    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(TargetActor); // 排除角色本身

    // 从最低处50到最高处200，每隔一定高度发射射线
    const float MinHeight = 30.0f;
    const float MaxHeight = 200.0f;
    const float StepHeight = 10.0f; // 每次增加的高度步长

    float DetectedEmptyHeight = MaxHeight; // 记录第一个检测为空的高度
    bool bFoundEmptySpace = false;
    float HeightOffset = MinHeight;

    // 如果当前已经在攀爬状态，直接返回
    if (ClimbingState == EClimbingState::Climbing)
    {
        UE_LOG(LogTemp, Verbose, TEXT("[攀爬检测] 当前已在攀爬状态"));
        return true;
    }

    for (HeightOffset = MinHeight; HeightOffset < MaxHeight; HeightOffset += StepHeight)
    {
        FVector StartLocation = BaseLocation;
        StartLocation.Z += HeightOffset;
        FVector CurrentEndLocation = StartLocation + NewDirection * CheckDistance;

        bool bHit = World->LineTraceSingleByChannel(
            HitResult,
            StartLocation,
            CurrentEndLocation,
            ECC_WorldStatic,
            QueryParams);

        // 绘制调试线
        DrawDebugLine(World, StartLocation, CurrentEndLocation, bHit ? FColor::Green : FColor::Red, false, 2.0f, 0, 1.0f);
        if (!bHit)
        {
            UE_LOG(LogTemp, Warning, TEXT("[攀爬检测] 在高度 %.2f 处检测到空旷空间"), HeightOffset);
            break;
        }
    }
    UE_LOG(LogTemp, Warning, TEXT("[攀爬检测] 遍历完成，HeightOffset: %.2f"), HeightOffset);
    if (HeightOffset == MaxHeight)
    {
        UE_LOG(LogTemp, Warning, TEXT("[攀爬检测] 进入攀爬状态"));
        SnapToSurface(TargetActor);
        StartClimb(TargetActor, MoveValue);
        return true;
    }
    if (HeightOffset < MaxHeight && HeightOffset > MinHeight)
    {
        UE_LOG(LogTemp, Warning, TEXT("播放翻越动画"));
        // TODO: 播放翻越动画
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("播放攀爬动画"));
        // TODO: 播放攀爬动画
    }
    return false;
}

void UClimbingSystem::StartClimb(AOperation_character *TargetActor, FVector MoveValue)
{
    if (!TargetActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("[开始攀爬] 目标角色为空"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("[开始攀爬] 进入攀爬状态"));
    ClimbingState = EClimbingState::Climbing;

    // TODO: 在这里添加开始攀爬的逻辑，例如播放攀爬动画、禁用重力等
    this->ClimbingState = EClimbingState::Climbing;

    UCharacterMovementComponent *MovementComp = TargetActor->GetCharacterMovement();
    if (MovementComp)
    {
        // 设置为Flying模式以允许自由移动
        MovementComp->SetMovementMode(MOVE_Flying);
        MovementComp->bOrientRotationToMovement = false; // 禁用旋转
        // 设置很大的惯性参数
        // MovementComp->Mass = 200.0f;                       // 增加质量产生更大惯性
        MovementComp->BrakingDecelerationFlying = 1000.0f; // 大幅降低空中制动产生惯性
        MovementComp->AirControl = 0.1f;                   // 降低空中控制增加惯性感
        MovementComp->MaxFlySpeed = 300.0f;                // 增加最大飞行速度

        UE_LOG(LogTemp, Warning, TEXT("[开始攀爬] 移动模式已设置为Flying"));
    }
}

void UClimbingSystem::StopClimb(AOperation_character *TargetActor)
{
    if (!TargetActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("[停止攀爬] 目标角色为空"));
        return;
    }
    this->ClimbingState = EClimbingState::Idle;

    UE_LOG(LogTemp, Warning, TEXT("[停止攀爬] 退出攀爬状态"));
    TargetActor->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
    TargetActor->GetCharacterMovement()->GravityScale = 1.0f;
    TargetActor->GetCharacterMovement()->AirControl = 0.35f;
    TargetActor->GetCharacterMovement()->MaxFlySpeed = 300.0f;
    TargetActor->GetCharacterMovement()->bOrientRotationToMovement = true; // 启用旋转
    TargetActor->GetCharacterMovement()->Mass = 100.0f;

    // 重置攀爬速度
    CurrentClimbSpeed = 0.0f;
    CurrentClimbVSpeed = 0.0f;
}
void UClimbingSystem::ClimbingMove(AOperation_character *TargetActor, FVector2D MoveValue)
{
    if (!TargetActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("[攀爬移动] 目标角色为空"));
        return;
    }

    if (ClimbingState != EClimbingState::Climbing)
    {
        UE_LOG(LogTemp, Verbose, TEXT("[攀爬移动] 当前不在攀爬状态"));
        return;
    }

    UWorld *World = TargetActor->GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("[攀爬移动] World为空"));
        return;
    }

    // 获取角色当前位置
    FVector CurrentLocation = TargetActor->GetActorLocation();

    // 发射射线检测移动方向是否有墙壁
    FVector ForwardVector = TargetActor->GetActorForwardVector();
    FVector StartLocation = CurrentLocation;
    FVector EndLocation = StartLocation + (TargetActor->GetActorRightVector() * MoveValue.X + TargetActor->GetActorUpVector() * MoveValue.Y).GetSafeNormal() * 50.0f;

    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(TargetActor);

    bool bHit = World->LineTraceSingleByChannel(
        HitResult,
        StartLocation,
        EndLocation,
        ECC_WorldStatic,
        QueryParams);
    // 绘制调试线
    DrawDebugLine(World, StartLocation, EndLocation, bHit ? FColor::Green : FColor::Red, false, 2.0f, 0, 1.0f);
    if (bHit) // 移动方向有墙壁阻挡
    {
        if (MoveValue.Y < 0)
        {
            // 停止攀爬
            StopClimb(TargetActor);
            return;
        }
        // 根据碰撞点位置执行旋转逻辑
        SmoothRotateToDirection(TargetActor, TargetActor->GetActorForwardVector(), -HitResult.Normal, 0.1f);
    }
    else
    {
        EndLocation = StartLocation + (TargetActor->GetActorRightVector() * MoveValue.X + TargetActor->GetActorUpVector() * MoveValue.Y).GetSafeNormal() * 17.0f;
        bool bHitRight = World->LineTraceSingleByChannel(
            HitResult,
            EndLocation,
            EndLocation + TargetActor->GetActorForwardVector() * 60.0f,
            ECC_WorldStatic,
            QueryParams);
        if (bHitRight)
        {
            // 移动逻辑
            //  TargetActor->AddMovementInput(TargetActor->GetActorForwardVector(), MoveValue.Y);
            TargetActor->AddMovementInput(TargetActor->GetActorRightVector(), MoveValue.X * 50);
            TargetActor->AddMovementInput(TargetActor->GetActorUpVector(), MoveValue.Y * 50);
            // 方向
            //  角色当前位置发射射线获取
            SnapToSurface(TargetActor);
        }
        else
        {
            // 沿射线方向向角色方向一次发射射线
            for (int i = 0; i < 10; i++)
            {
                // 发射射线
                QueryParams.AddIgnoredActor(TargetActor);

                bHit = World->LineTraceSingleByChannel(
                    HitResult,
                    EndLocation + TargetActor->GetActorForwardVector() * i * 10.0f,
                    TargetActor->GetActorLocation() + TargetActor->GetActorForwardVector() * i * 10.0f,
                    ECC_WorldStatic,
                    QueryParams);
                // 绘制调试线
                DrawDebugLine(World, EndLocation + TargetActor->GetActorForwardVector() * i * 10.0f, TargetActor->GetActorLocation() + TargetActor->GetActorForwardVector() * i * 10.0f, bHit ? FColor::Green : FColor::Red, false, 2.0f, 0, 1.0f);
                if (bHit)
                    break;
            }
            // 根据碰撞点位置执行旋转逻辑
            SmoothRotateToDirection(TargetActor, TargetActor->GetActorForwardVector(), -HitResult.Normal, 0.1f);
        }
    }
}

bool UClimbingSystem::SnapToSurface(AOperation_character *TargetActor)
{
    if (!TargetActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("[贴合表面] 目标角色为空"));
        return false;
    }

    UWorld *World = TargetActor->GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("[贴合表面] World为空"));
        return false;
    }

    // 获取角色当前位置和前方方向
    FVector CurrentLocation = TargetActor->GetActorLocation();
    FVector ForwardVector = TargetActor->GetActorForwardVector();

    // 确保前方方向有效
    if (ForwardVector.IsNearlyZero())
    {
        UE_LOG(LogTemp, Warning, TEXT("[贴合表面] 角色前方方向为零向量"));
        return false;
    }

    // 向角色前方发射射线检测物体
    float TraceDistance = 100.0f; // 检测距离
    FVector StartLocation = CurrentLocation;
    FVector EndLocation = CurrentLocation + ForwardVector * TraceDistance;

    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(TargetActor); // 排除角色本身

    // 发射射线检测墙面
    bool bHit = World->LineTraceSingleByChannel(
        HitResult,
        StartLocation,
        EndLocation,
        ECC_WorldStatic,
        QueryParams);

    if (bHit)
    {
        // 计算理想的贴合位置：在碰撞点前方一定距离处
        float CharacterOffset = 40.0f; // 角色与墙面的距离偏移量
        FVector IdealPosition = HitResult.Location + HitResult.Normal * CharacterOffset;

        // 设置角色新位置
        TargetActor->SetActorLocation(IdealPosition);

        // 调整角色朝向，使其面向墙面（取法线的反方向）
        FVector TargetForward = -HitResult.Normal;
        TargetForward.Z = 0.0f; // 保持水平旋转，防止角色倾斜

        if (!TargetForward.IsNearlyZero())
        {
            FRotator TargetRotation = TargetForward.Rotation();
            TargetActor->SetActorRotation(TargetRotation);
        }

        UE_LOG(LogTemp, Verbose, TEXT("[贴合表面] 成功贴合到表面，位置: %s"), *IdealPosition.ToString());
        return true;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[贴合表面] 未检测到可贴合的表面"));
        return false;
    }
}

bool UClimbingSystem::SmoothRotateToDirection(AOperation_character *TargetActor, const FVector &FromDirection, const FVector &ToDirection, float RotationSpeed)
{
    if (!TargetActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("[平滑旋转] 目标角色为空"));
        return false;
    }

    UWorld *World = TargetActor->GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("[平滑旋转] World为空"));
        return false;
    }

    // 确保方向向量有效
    if (FromDirection.IsNearlyZero() || ToDirection.IsNearlyZero())
    {
        UE_LOG(LogTemp, Warning, TEXT("[平滑旋转] 方向向量为零向量"));
        return false;
    }

    // 限制旋转速度在0-1之间
    RotationSpeed = FMath::Clamp(RotationSpeed, 0.0f, 1.0f);

    // 将向量转换为旋转角度（只保留水平旋转）
    FVector FromHorizontal = FromDirection;
    FromHorizontal.Z = 0.0f;
    FromHorizontal.Normalize();

    FVector ToHorizontal = ToDirection;
    ToHorizontal.Z = 0.0f;
    ToHorizontal.Normalize();

    if (FromHorizontal.IsNearlyZero() || ToHorizontal.IsNearlyZero())
    {
        UE_LOG(LogTemp, Warning, TEXT("[平滑旋转] 水平方向向量为零"));
        return false;
    }

    // 获取当前角色的旋转
    FRotator CurrentRotation = TargetActor->GetActorRotation();

    // 计算目标旋转
    FRotator TargetRotation = ToHorizontal.Rotation();

    // 使用插值计算新的旋转角度
    FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, World->GetDeltaSeconds(), RotationSpeed * 10.0f);
    // 角色向前移动1
    // TargetActor->AddMovementInput(TargetActor->GetActorForwardVector(), 1.0f);
    // 应用新的旋转
    TargetActor->SetActorRotation(NewRotation);

    // 检查是否已经接近目标旋转
    float AngleDifference = FMath::Abs(FRotator::NormalizeAxis(TargetRotation.Yaw - NewRotation.Yaw));
    bool bIsComplete = AngleDifference < 1.0f; // 如果角度差小于1度，认为旋转完成

    if (bIsComplete)
    {
        UE_LOG(LogTemp, Verbose, TEXT("[平滑旋转] 旋转完成"));
    }

    return bIsComplete;
}
