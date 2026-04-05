// ClimbingSystem.cpp
#include "ClimbingSystem.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"

UClimbingSystem::UClimbingSystem()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UClimbingSystem::BeginPlay()
{
    Super::BeginPlay();
}

FClimbDetectionResult UClimbingSystem::DetectClimbableSurface(ACharacter *Character, float DetectionDistance)
{
    FClimbDetectionResult Result;

    if (!Character)
    {
        return Result;
    }

    FVector StartLocation = Character->GetActorLocation();
    FVector ForwardVector = Character->GetActorForwardVector();
    FVector EndLocation = StartLocation + ForwardVector * DetectionDistance;

    // 绘制调试?
    DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 2.0f, 0, 2.0f);

    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(Character);

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult,
        StartLocation,
        EndLocation,
        ECC_Visibility,
        QueryParams);

    if (bHit)
    {
        FName ActorTag = HitResult.GetActor() ? HitResult.GetActor()->Tags.Num() > 0 ? HitResult.GetActor()->Tags[0] : NAME_None : NAME_None;

        if (IsValidClimbingSurface(HitResult.ImpactNormal, ActorTag))
        {
            Result.bCanClimb = true;
            Result.SurfaceNormal = HitResult.ImpactNormal;
            Result.ClimbPoint = HitResult.ImpactPoint;
            Result.DistanceToClimbPoint = FVector::Dist(StartLocation, HitResult.ImpactPoint);

            // 判断表面类型
            float DotProduct = FVector::DotProduct(HitResult.ImpactNormal, FVector::UpVector);
            if (FMath::Abs(DotProduct) > 0.8f)
            {
                Result.SurfaceType = EClimbSurfaceType::Ledge;
            }
            else
            {
                Result.SurfaceType = EClimbSurfaceType::Wall;
            }
        }
    }

    return Result;
}

bool UClimbingSystem::CanMoveToNextClimbPosition(FVector CurrentLocation, FVector MoveDirection, float MoveDistance)
{
    FVector TargetLocation = CurrentLocation + MoveDirection.GetSafeNormal() * MoveDistance;

    // 检测目标位置是否可攀爬
    FHitResult HitResult;
    FCollisionQueryParams QueryParams;

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult,
        TargetLocation,
        TargetLocation + FVector::UpVector * 100.0f,
        ECC_Visibility,
        QueryParams);

    // 绘制调试?
    DrawDebugLine(GetWorld(), CurrentLocation, TargetLocation, FColor::Blue, false, 1.0f, 0, 1.0f);

    return bHit && IsValidClimbingSurface(HitResult.ImpactNormal, NAME_None);
}

FVector UClimbingSystem::CalculateOptimalClimbPosition(const FHitResult &HitResult)
{
    FVector OptimalPosition = HitResult.ImpactPoint;

    // 根据碰撞结果调整位置，确保角色不会卡在墙?
    if (HitResult.GetActor())
    {
        UCapsuleComponent *CapsuleComp = Cast<UCapsuleComponent>(HitResult.GetActor()->GetComponentByClass(UCapsuleComponent::StaticClass()));
        if (CapsuleComp)
        {
            float CapsuleRadius = CapsuleComp->GetUnscaledCapsuleRadius();
            OptimalPosition -= HitResult.ImpactNormal * (CapsuleRadius + 20.0f);
        }
        else
        {
            // 默认偏移距离
            OptimalPosition -= HitResult.ImpactNormal * 50.0f;
        }
    }

    return OptimalPosition;
}

FVector UClimbingSystem::CalculateRelativeClimbPosition(ACharacter *Character, const FHitResult &HitResult)
{
    if (!Character)
        return HitResult.ImpactPoint;

    FVector CharacterLocation = Character->GetActorLocation();
    FVector ImpactPoint = HitResult.ImpactPoint;
    FVector SurfaceNormal = HitResult.ImpactNormal;

    // 计算角色到碰撞点的相对偏?
    FVector RelativeOffset = ImpactPoint - CharacterLocation;
    float DistanceToWall = RelativeOffset.Size();

    UE_LOG(LogTemp, Warning, TEXT("=== 攀爬位置计算调?==="));
    UE_LOG(LogTemp, Warning, TEXT("角色位置: %s"), *CharacterLocation.ToString());
    UE_LOG(LogTemp, Warning, TEXT("碰撞? %s"), *ImpactPoint.ToString());
    UE_LOG(LogTemp, Warning, TEXT("相对偏移: %s (距离: %.2f)"), *RelativeOffset.ToString(), DistanceToWall);

    // 计算安全距离（考虑角色胶囊体半径）
    float SafeDistance = 50.0f;
    UCapsuleComponent *CharacterCapsule = Character->GetCapsuleComponent();
    if (CharacterCapsule)
    {
        SafeDistance = CharacterCapsule->GetUnscaledCapsuleRadius() + 20.0f;
    }

    UE_LOG(LogTemp, Warning, TEXT("安全距离: %.2f"), SafeDistance);

    // 方法1：基于相对偏移计算（保持XY平面的相对关系）
    FVector FinalPosition;

    if (DistanceToWall > 0.1f)
    {
        // 计算XY平面上的投影方向
        FVector HorizontalNormal = SurfaceNormal;
        HorizontalNormal.Z = 0.0f;
        HorizontalNormal.Normalize();

        // 在XY平面上保持相对位置关?
        FVector HorizontalOffset = RelativeOffset;
        HorizontalOffset.Z = 0.0f;
        float HorizontalDistance = HorizontalOffset.Size();

        if (HorizontalDistance > 0.1f)
        {
            FVector HorizontalDirection = HorizontalOffset.GetSafeNormal();

            // 计算新的水平位置：从角色当前位置加上适当的偏?
            FVector NewHorizontalPosition = FVector(CharacterLocation.X, CharacterLocation.Y, 0.0f) +
                                            HorizontalDirection * (SafeDistance + 20.0f);

            FinalPosition = NewHorizontalPosition;
            FinalPosition.Z = CharacterLocation.Z; // 保持原有高度

            UE_LOG(LogTemp, Warning, TEXT("使用相对偏移方法"));
            UE_LOG(LogTemp, Warning, TEXT("水平方向: %s"), *HorizontalDirection.ToString());
            UE_LOG(LogTemp, Warning, TEXT("水平距离: %.2f"), HorizontalDistance);
        }
        else
        {
            // 如果水平距离很小，使用简单的后退方法
            FinalPosition = CharacterLocation - SurfaceNormal * SafeDistance;
            FinalPosition.Z = CharacterLocation.Z;
            UE_LOG(LogTemp, Warning, TEXT("使用简单后退方法（水平距离过小）"));
        }
    }
    else
    {
        // 如果距离很近，直接使用角色当前位?
        FinalPosition = CharacterLocation;
        FinalPosition.Z = CharacterLocation.Z;
        UE_LOG(LogTemp, Warning, TEXT("距离过近，使用当前位置"));
    }

    UE_LOG(LogTemp, Warning, TEXT("最终位置: %s"), *FinalPosition.ToString());
    UE_LOG(LogTemp, Warning, TEXT("========================"));

    return FinalPosition;
}

bool UClimbingSystem::DetectVaultPoint(ACharacter *Character, FVector &OutVaultPoint, float CheckDistance)
{
    if (!Character)
    {
        return false;
    }

    FVector StartLocation = Character->GetActorLocation();
    FVector ForwardVector = Character->GetActorForwardVector();

    // 向前检测边?
    for (int32 i = 0; i < 10; i++)
    {
        FVector CheckLocation = StartLocation + ForwardVector * i * 20.0f;
        FVector DownLocation = CheckLocation - FVector::UpVector * CheckDistance;

        FHitResult HitResult;
        FCollisionQueryParams QueryParams;
        QueryParams.AddIgnoredActor(Character);

        bool bHit = GetWorld()->LineTraceSingleByChannel(
            HitResult,
            CheckLocation,
            DownLocation,
            ECC_Visibility,
            QueryParams);

        // 绘制调试?
        DrawDebugLine(GetWorld(), CheckLocation, DownLocation, FColor::Green, false, 1.0f, 0, 1.0f);

        if (bHit && CanVaultOverEdge(HitResult.ImpactPoint))
        {
            OutVaultPoint = HitResult.ImpactPoint + ForwardVector * 20.0f;
            return true;
        }
    }

    return false;
}

void UClimbingSystem::GetSurroundingDetectionPoints(FVector BaseLocation, TArray<FVector> &OutDetectionPoints)
{
    OutDetectionPoints.Empty();

    // 定义周围的检测点偏移
    TArray<FVector> Offsets = {
        FVector(0, 0, 80),  // 上方
        FVector(0, 0, -80), // 下方
        FVector(0, 50, 0),  // 右侧
        FVector(0, -50, 0), // 左侧
        FVector(80, 0, 0),  // 前方
        FVector(-80, 0, 0)  // 后方
    };

    for (const FVector &Offset : Offsets)
    {
        OutDetectionPoints.Add(BaseLocation + Offset);
    }
}

bool UClimbingSystem::IsValidClimbingSurface(const FVector &SurfaceNormal, const FName &ActorTag)
{
    // 检查标签是否禁止攀爬
    if (ActorTag == FName("notclimb"))
    {
        return false;
    }

    // 检查表面角度（不能是完全水平或垂直的表面）
    float VerticalDot = FMath::Abs(FVector::DotProduct(SurfaceNormal, FVector::UpVector));
    float HorizontalDot = FMath::Abs(FVector::DotProduct(SurfaceNormal, FVector::ForwardVector));

    // 表面法线不能过于垂直或水?
    if (VerticalDot > 0.95f || HorizontalDot > 0.95f)
    {
        return false;
    }

    return true;
}

FRotator UClimbingSystem::CalculateWallFacingRotation(const FVector &WallNormal)
{
    FRotator TargetRotation = WallNormal.Rotation();
    TargetRotation.Yaw += 180.0f;  // 角色面向墙面
    TargetRotation.Pitch *= -1.0f; // 调整俯仰?

    return TargetRotation;
}

bool UClimbingSystem::CanVaultOverEdge(FVector EdgeLocation, float CharacterHeight)
{
    // 检测边缘上方是否有足够的空?
    FHitResult HitResult;
    FVector UpCheckStart = EdgeLocation + FVector::UpVector * CharacterHeight;
    FVector UpCheckEnd = EdgeLocation + FVector::UpVector * (CharacterHeight + 50.0f);

    bool bHitAbove = GetWorld()->LineTraceSingleByChannel(
        HitResult,
        UpCheckStart,
        UpCheckEnd,
        ECC_Visibility);

    // 绘制调试?
    DrawDebugLine(GetWorld(), UpCheckStart, UpCheckEnd, bHitAbove ? FColor::Red : FColor::Green, false, 2.0f, 0, 2.0f);

    return !bHitAbove; // 如果上方没有障碍物，则可以翻?
}

void UClimbingSystem::ControlClimbingSpeed(ACharacter *Character, float MaxClimbSpeed)
{
    if (!Character)
        return;

    UCharacterMovementComponent *MovementComp = Character->GetCharacterMovement();
    if (!MovementComp)
        return;

    FVector CurrentVelocity = MovementComp->Velocity;
    float CurrentSpeed = CurrentVelocity.Size();

    // 如果当前速度超过最大攀爬速度，则限制?
    if (CurrentSpeed > MaxClimbSpeed)
    {
        FVector LimitedVelocity = CurrentVelocity.GetClampedToMaxSize(MaxClimbSpeed);
        MovementComp->Velocity = LimitedVelocity;

        UE_LOG(LogTemp, Verbose, TEXT("限制攀爬速度: %.2f -> %.2f"), CurrentSpeed, MaxClimbSpeed);
    }
}

void UClimbingSystem::ApplyClimbingDamping(ACharacter *Character, float DampingFactor)
{
    if (!Character || DampingFactor < 0.0f || DampingFactor > 1.0f)
        return;

    UCharacterMovementComponent *MovementComp = Character->GetCharacterMovement();
    if (!MovementComp)
        return;

    // 应用阻尼来减少惯?
    FVector DampedVelocity = MovementComp->Velocity * DampingFactor;
    MovementComp->Velocity = DampedVelocity;

    // 如果速度很小，直接设为零
    if (DampedVelocity.Size() < 10.0f)
    {
        MovementComp->Velocity = FVector::ZeroVector;
    }

    UE_LOG(LogTemp, Verbose, TEXT("应用攀爬阻尼，系数: %.2f"), DampingFactor);
}
