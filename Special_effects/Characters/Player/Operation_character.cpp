// Fill out your copyright notice in the Description page of Project Settings.

#include "Operation_character.h"
#include "Camera/CameraComponent.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputComponent.h"
#include "MotionWarpingComponent.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h" // 添加这个头文件
#include "GameFramework/SpringArmComponent.h"         // ✅ 正确路径
#include "Components/CapsuleComponent.h"
#include "../../Core/Managers/MyGameInstance.h"
#include "../../Data/Tables/item/MyDataTable_iteam_weapon.h"
#include "../../Combat/Items/MyItemObjector.h"

#include <EnhancedInputSubsystems.h>
AOperation_character::AOperation_character()
{
    // 父类构造函数
    AutoPossessPlayer = EAutoReceiveInput::Player0;
    SetActorTickEnabled(true);
    PrimaryActorTick.bCanEverTick = true;
    set_character_name("Operation_character");
    InitVariableWithParams(
        int32(100), int32(100), 1.0f,
        int32(100), int32(100), 1.0f,
        int32(100), int32(100));
    // 角色面向移动方向
    SetActorRotation(GetActorForwardVector().Rotation());
    // 角色旋转速度
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
    // 挂载弹簧壁
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
    SpringArm->TargetArmLength = 400.0f;
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera失"));
    Camera->SetupAttachment(SpringArm);
    // 使用pawn控制朝向
    SpringArm->bUsePawnControlRotation = true;
    MotionWarpingComp = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComp"));
    //	TMap<EBackPack, FBackpackSlot> Backpack;
    // 在构造函数或初始化函数中
    Backpack.Empty();
    for (int32 i = 0; i < static_cast<int32>(EBackPack::MAX); i++)
    {
        EBackPack SlotType = static_cast<EBackPack>(i);
        if (SlotType != EBackPack::MAX) // 排除无效值
        {
            Backpack.Add(SlotType, FBackpackSlot()); // 显式使用默认值
        }
    }
    Weapon = CreateDefaultSubobject<UMyWeapons>(TEXT("weaponcomponent"));
    //  Weapon->SetupAttachment(GetMesh(), FName("WeaponComponent"));
}

void AOperation_character::BeginPlay()
{
    Super::BeginPlay();

    if (APlayerController *PC = Cast<APlayerController>(GetController()))
    {
        UE_LOG(LogTemp, Warning, TEXT("PC OK"));
        PC->bShowMouseCursor = false;
        PC->SetInputMode(FInputModeGameOnly());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("NO PlayerController"));
    }
    InitInput();
    // 前方位置检测
    if (UAnimInstance *AnimInstance = GetMesh()->GetAnimInstance())
    {
        AnimInstance->OnMontageEnded.AddDynamic(this, &AOperation_character::OnMontageEnded);
        UE_LOG(LogTemp, Log, TEXT("蒙太奇回调已绑定"));
    }
}
void AOperation_character::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    // checkforword();

    if (GetWorldTimerManager().IsTimerActive(TimerSprintDown))
    {
        // 判断速度
        if (GetCharacterMovement()->GetCurrentAcceleration().Size() < 600.0f)
        {
            UE_LOG(LogTemp, Log, TEXT("--sprintdown----------------------------------------------------"));
            UE_LOG(LogTemp, Log, TEXT("--sprintdownvelocity=%f"), GetCharacterMovement()->GetCurrentAcceleration().Size());
            GetWorldTimerManager().ClearTimer(TimerSprintDown);
            GetWorldTimerManager().SetTimer(TimerSprintUP, [this]()
                                            { UpdateStamina(-1.0f); }, 0.1f, true);
            // 设置最大速度
            GetCharacterMovement()->MaxWalkSpeed = 600.0f;
            issprint = false;
        }
    }
    if (GetWorldTimerManager().IsTimerActive(TimerSprintUP))
    {
        if (NowStamina >= Stamina)
            GetWorldTimerManager().ClearTimer(TimerSprintUP);
    }
}

void AOperation_character::InitVariableWithParams(
    int32 InHealth,
    int32 InNowHealth,
    float InHealthPercent,
    int32 InStamina,
    int32 InNowStamina,
    float InStaminaPercent,
    int32 InAttack,
    int32 InDefense)
{
    // 现在可以清晰地区分
    Health = InHealth;
    NowHealth = InNowHealth; // 假设 NowHealth 对应 MaxHealth
    HealthPercent = InHealthPercent;

    Stamina = InStamina;
    NowStamina = InNowStamina; // 假设 NowStamina 对应 MaxStamina
    StaminaPercent = InStaminaPercent;

    Attack = InAttack;
    Defense = InDefense;
    this->isclimb = false;
    this->ismontage = false;
    this->isdunfu = false;
    issprint = false;
    GetCharacterMovement()->MaxWalkSpeed = 600.0f;
}

/**
 * 初始化按键操作
 *
 */
void AOperation_character::InitInput()
{

    if (const ULocalPlayer *Player = (GEngine && GetWorld()) ? GEngine->GetFirstGamePlayer(GetWorld()) : nullptr)
    {
        UEnhancedInputLocalPlayerSubsystem *Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(Player);
        if (Subsystem)
        {
            Subsystem->AddMappingContext(IMC_Default, 0);
        }
    }
}
/**
 * 按键操作
 */
void AOperation_character::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    // 添加按键操作
    if (UEnhancedInputComponent *EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
    {
        if (IA_Move)
        {
            EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AOperation_character::Move);
        }
        if (IA_Look_x)
        {
            UE_LOG(LogTemp, Log, TEXT("--ialook----------------------------------------------------"));
            UE_LOG(LogTemp, Log, TEXT("绑定成功"))
            EnhancedInputComponent->BindAction(IA_Look_x, ETriggerEvent::Triggered, this, &AOperation_character::Look_x);
        }
        if (IA_Look_y)
        {
            EnhancedInputComponent->BindAction(IA_Look_y, ETriggerEvent::Triggered, this, &AOperation_character::Look_y);
        }
        if (IA_Jump)
        {
            EnhancedInputComponent->BindAction(IA_Jump, ETriggerEvent::Triggered, this, &AOperation_character::Jumps);
        }
        if (IA_Ctrl)
        {
            EnhancedInputComponent->BindAction(IA_Ctrl, ETriggerEvent::Triggered, this, &AOperation_character::Ctrl);
        }
        if (IA_Sprint)
        {
            EnhancedInputComponent->BindAction(IA_Sprint, ETriggerEvent::Triggered, this, &AOperation_character::Sprint);
        }
        if (IA_Attack)
        {
            EnhancedInputComponent->BindAction(IA_Attack, ETriggerEvent::Canceled, this, &AOperation_character::AttackNormal);
            EnhancedInputComponent->BindAction(IA_Attack, ETriggerEvent::Completed, this, &AOperation_character::AttackNormal);
        }
        if (IA_Attack)
        {
            EnhancedInputComponent->BindAction(IA_Attack, ETriggerEvent::Started, this, &AOperation_character::Attacks);
        }
        if (IA_Pick)
        {
            EnhancedInputComponent->BindAction(IA_Pick, ETriggerEvent::Triggered, this, &AOperation_character::Pick);
        }
        if (IA_Backpack)
        {
            EnhancedInputComponent->BindAction(IA_Backpack, ETriggerEvent::Triggered, this, &AOperation_character::Backpacks);
        }
        if (IA_CharacterDetail)
        {
            EnhancedInputComponent->BindAction(IA_CharacterDetail, ETriggerEvent::Triggered, this, &AOperation_character::CharacterDetails);
        }
    }
}

/*
按键事件
*/
void AOperation_character::Move(const FInputActionValue &Value)
{
    UE_LOG(LogTemp, Log, TEXT("--move----------------------------------------------------"));

    if (Character_Status == EStatus::CMS_MoveNotBreak)
        return;
    if (Character_Status == EStatus::CMS_MoveBreak)
    {
        // 停止蒙太奇
        int temp = AttackNum;
        temp--;
        temp == -1 ? temp = AttackMontageList.Num() - 1 : temp;
        if (temp != -1)
        {
            UE_LOG(LogTemp, Log, TEXT("temp==%d"), temp);
            StopAnimMontage(AttackMontageList[temp]);
            AttackNum = 0;
        }
    }

    if (ismontage)
    {
        UE_LOG(LogTemp, Log, TEXT("--move----------------------------------------------------"));
        return;
    }
    FVector2D MoveValue = Value.Get<FVector2D>();
    if (!isclimb)
    {
        if (ComeinClimb())
        {
            AddMovementInput(GetActorUpVector(), MoveValue.Y);
            AddMovementInput(GetActorRightVector(), MoveValue.X);
        }
        else
        {
            // 添加移动逻辑
            const FRotator Rotation = Controller->GetControlRotation();
            const FRotator YawRotation(0, Rotation.Yaw, 0);                                      // 获取角色的朝向
            const FVector ForeardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X); // 获取角色的朝向向量
            const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
            AddMovementInput(ForeardDirection, MoveValue.Y);
            AddMovementInput(RightDirection, MoveValue.X);
        }
    }
    else
    {

        // 是否可继续攀爬
        FVector temp = (GetActorUpVector() * MoveValue.Y + GetActorRightVector() * MoveValue.X);

        if (NextPosition((temp / temp.Size()) * 100.0f))
        {
        }

        FHitResult HitResult;
        FVector Location = GetActorLocation();
        bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Location,
                                                         Location + GetActorForwardVector() * 100.0f, ECC_Visibility);
        if (bHit)
        {
            SetActorRotation(FRotator(HitResult.ImpactNormal.Rotation().Pitch * -1,
                                      HitResult.ImpactNormal.Rotation().Yaw + 180, 0));
            bHit = GetWorld()->LineTraceSingleByChannel(HitResult, GetActorLocation(),
                                                        GetActorLocation() + GetActorForwardVector() * 80.0f, ECC_Visibility);
            if (bHit)
            {
                SetActorLocation(HitResult.ImpactPoint - (GetActorForwardVector() * (GetCapsuleComponent()->GetUnscaledCapsuleRadius() + 20.0f)));
            }
        }
        AddMovementInput(GetActorUpVector(), MoveValue.Y);
        AddMovementInput(GetActorRightVector(), MoveValue.X);
        climbspeed = MoveValue.Y * 100.0f;
        // 垂直攀爬速度
        climbvspeed = MoveValue.X * 100.0f;
    }
}

void AOperation_character::Look_x(const FInputActionValue &Value)
{
    float LookValue = Value.Get<float>();
    // UE_LOG(LogTemp, Warning, TEXT("Look_x: Value=%f"), LookValue);

    if (GetController())
    {
        AddControllerYawInput(LookValue);
        // UE_LOG(LogTemp, Warning, TEXT("Look_x: Yaw applied=%f"), LookValue);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Look_x: No controller!"));
    }
}

void AOperation_character::Look_y(const FInputActionValue &Value)
{
    float LookValue = Value.Get<float>();
    // UE_LOG(LogTemp, Warning, TEXT("Look_y: Value=%f"), LookValue);

    if (GetController())
    {
        AddControllerPitchInput(LookValue);
        // UE_LOG(LogTemp, Warning, TEXT("Look_y: Pitch applied=%f"), LookValue);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Look_y: No controller!"));
    }
}
void AOperation_character::Jumps(const FInputActionValue &Value)
{
    if (isclimb)
    {
        UE_LOG(LogTemp, Log, TEXT("--jumps----------------------------------------------------"));
        // 速度方向
        FVector vector = GetVelocity().GetSafeNormal();
        UE_LOG(LogTemp, Log, TEXT("vector.Size() = %s"), *vector.ToString());
        FVector Location = GetActorLocation();
        FVector EndLocation;
        if (vector.Size() == 0)
        {
            UE_LOG(LogTemp, Log, TEXT("vector.Size() == 0"));

            EndLocation = Location + GetActorUpVector() * 100.0f;
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("vector.Size() != 0%s"), *vector.ToString());
            EndLocation = Location + vector * 100.0f;
        }
        FHitResult HitResult1;
        FHitResult HitResult2;
        DrawDebugLine(GetWorld(), Location, Location + GetActorForwardVector() * 100.0f, FColor::Red, 1.0f, 1.0f);
        bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult1, EndLocation, EndLocation + GetActorForwardVector() * 100.0f, ECC_Visibility);
        DrawDebugLine(GetWorld(), EndLocation, EndLocation + GetActorForwardVector() * 100.0f, FColor::Blue, 1.0f, 1.0f);
        EndLocation = EndLocation + GetActorUpVector() * 150.0f;
        bool bHit2 = GetWorld()->LineTraceSingleByChannel(HitResult2, EndLocation, EndLocation + GetActorForwardVector() * 100.0f, ECC_Visibility);
        if (bHit2)
        {
            if (!ismontage)
            {
                MotionWarpingComp->RemoveWarpTarget(FName("jumpclimb"));
                FMotionWarpingTarget StartTarget;
                StartTarget.Name = FName("jumpclimb");
                StartTarget.Location = HitResult1.ImpactPoint - GetActorUpVector() * 90.0f;
                FRotator Rotation = HitResult1.ImpactNormal.Rotation();
                Rotation.Yaw += 180;
                StartTarget.Rotation = Rotation;
                MotionWarpingComp->AddOrUpdateWarpTarget(StartTarget);
                PlayAnimMontage(ClimbJumpMontage);
                ismontage = true;
            }
        }
        else
        {
            for (int i = 0; i < 10; i++)
            {
                EndLocation = EndLocation + GetActorForwardVector() * i * 10.0f;
                bHit2 = GetWorld()->LineTraceSingleByChannel(HitResult2, EndLocation, EndLocation - GetActorUpVector() * 300.0f, ECC_Visibility);
                // 画线
                DrawDebugLine(GetWorld(), EndLocation, EndLocation - GetActorUpVector() * 300.0f, FColor::Red, 1.0f, 1.0f);
                if (bHit2)
                {
                    if (!ismontage)
                    {
                        MotionWarpingComp->RemoveWarpTarget(FName("bianyuan"));
                        MotionWarpingComp->RemoveWarpTarget(FName("zhanli"));
                        FMotionWarpingTarget StartTarget;
                        StartTarget.Name = FName("bianyuan");
                        StartTarget.Location = HitResult2.ImpactPoint;
                        UE_LOG(LogTemp, Warning, TEXT("翻越%s"), *HitResult2.ImpactPoint.ToString());
                        StartTarget.Rotation = GetActorRotation();
                        MotionWarpingComp->AddOrUpdateWarpTarget(StartTarget);
                        FMotionWarpingTarget EndTarget;
                        EndTarget.Name = FName("zhanli");
                        EndTarget.Location = HitResult2.ImpactPoint + GetActorForwardVector() * 20;
                        EndTarget.Rotation = GetActorRotation();
                        MotionWarpingComp->AddOrUpdateWarpTarget(EndTarget);
                        PlayAnimMontage(ClimbMontage);
                        ismontage = true;
                        UE_LOG(LogTemp, Warning, TEXT("准备翻越"));
                    }
                    break;
                }
                else
                    UE_LOG(LogTemp, Warning, TEXT("没有翻越"));
            }
            // 翻越蒙太奇
        }
    }
    else
    {
        if (isdunfu)
        {
            isdunfu = false;
        }
        // 停止动画蒙太奇
        int temp = AttackNum;
        temp--;
        temp == -1 ? temp = AttackMontageList.Num() - 1 : temp;
        if (temp != -1)
            StopAnimMontage(AttackMontageList[temp]);
        Jump();
    }
}
void AOperation_character::Ctrl(const FInputActionValue &Value)
{
    if (GetCharacterMovement()->MovementMode == MOVE_Flying)
    {
        SwitchClimb();
    }
    else if (GetCharacterMovement()->MovementMode == MOVE_Walking)
    {
        this->isdunfu = !isdunfu;
    }
}
void AOperation_character::Sprint(const FInputActionValue &Value)
{

    UE_LOG(LogTemp, Warning, TEXT("Sprint"));

    if (issprint)
    {
        // issprint = false;
        // GetCharacterMovement()->MaxWalkSpeed = 600;
        // // 定时器是否存在
        // if (GetWorldTimerManager().IsTimerActive(TimerSprintDown))
        // {
        //     GetWorldTimerManager().ClearTimer(TimerSprintDown);
        //     GetWorldTimerManager().SetTimer(
        //         TimerSprintUP,
        //         [this]()
        //         {
        //             UpdateStamina(-1.0f);
        //         },
        //         0.1f, // 计时器触发间隔
        //         true  // 是否循环
        //     );
        // }
    }
    else
    {
        // 开启定时器执行体力衰减
        if (GetWorldTimerManager().IsTimerActive(TimerSprintUP))
        {
            GetWorldTimerManager().ClearTimer(TimerSprintUP);
        }
        GetWorldTimerManager().SetTimer(
            TimerSprintDown,
            [this]()
            {
                UpdateStamina(1.0f);
            },
            0.1f, // 计时器触发间隔
            true  // 是否循环
        );
        GetCharacterMovement()->MaxWalkSpeed = 1000;
        issprint = true;
        UE_LOG(LogTemp, Warning, TEXT(" 1000"));
    }
}
void AOperation_character::LookAxis(const FInputActionValue &Value)
{
    // if (GetWorldTimerManager().IsTimerActive(TimerSprintDown))
    // {
    //     GetWorldTimerManager().ClearTimer(TimerSprintDown);
    // }
}

// 捡取
void AOperation_character::Pick(const FInputActionValue &Value)
{
    UE_LOG(LogTemp, Warning, TEXT("开始拾取"));
    TArray<FHitResult> HitResult;
    FVector Location = GetActorLocation();
    float Radius = 100.0f;
    FCollisionShape Sphere = FCollisionShape::MakeSphere(Radius);
    bool bHit = GetWorld()->SweepMultiByChannel(
        HitResult,
        Location,
        Location,
        FQuat::Identity,
        ECC_Visibility,
        Sphere // ✅ 这里传 FCollisionShape
    );
    if (bHit)
    {
        for (const FHitResult &Hit : HitResult)
        {
            if (Hit.GetActor() && Hit.GetActor()->ActorHasTag(FName("pick")))
            {
                UE_LOG(LogTemp, Warning, TEXT("拾取物品"));
                AMyItemObjector *actor = Cast<AMyItemObjector>(Hit.GetActor());
                if (actor)
                {
                    AddItemToBackPack(actor->Item);
                    actor->Destroy();
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("拾取物品失败"));
                }
            }
        }
    }
}
// 背包

void AOperation_character::Attacks(const FInputActionValue &Value)
{
    UE_LOG(LogTemp, Warning, TEXT("攻击"));
    if (Weapon && Weapon->WeaponType == EWeaponType::gongjian)
    {
        Character_Status = EStatus::CMS_Arrow;
        // 控制器转向
        bUseControllerRotationYaw = true;
        SpringArm->SetRelativeLocation(FVector(0.0f, 40.f, 0.0f));
        SpringArm->TargetArmLength = 250.f;
    }
    return;
}

void AOperation_character::Backpacks(const FInputActionValue &Value)
{
}
// 角色详情

void AOperation_character::CharacterDetails(const FInputActionValue &Value)
{
}

void AOperation_character::OnMontageEnded(UAnimMontage *Montage, bool bInterrupted)
{
    // isclimb = false;
    // GetCharacterMovement()->SetMovementMode(MOVE_Walking);
    // GetCharacterMovement()->bOrientRotationToMovement = true;
    ismontage = false;
    // UE_LOG(LogTemp, Warning, TEXT("切换状态"));
    if (Montage == ClimbMontage)
    {
        SwitchClimb();
    }
}

void AOperation_character::SwitchClimb()
{
    UE_LOG(LogTemp, Warning, TEXT("切换攀爬"));
    if (isclimb)
    {
        isclimb = false;
        GetCharacterMovement()->SetMovementMode(MOVE_Walking);
        GetCharacterMovement()->bOrientRotationToMovement = true;

        // 恢复正常的物理参数
        GetCharacterMovement()->Mass = 80.0f;                       // 恢复正常质量
        GetCharacterMovement()->BrakingDecelerationFlying = 512.0f; // 恢复正常空中制动
        GetCharacterMovement()->AirControl = 0.8f;                  // 恢复正常空中控制
        GetCharacterMovement()->MaxFlySpeed = 600.0f;               // 恢复正常飞行速度
        GetCharacterMovement()->MaxWalkSpeed = 600.0f;

        UE_LOG(LogTemp, Warning, TEXT("退出攀爬，恢复正常物理参数"));
    }
    else
    {
        isclimb = true;
        GetCharacterMovement()->SetMovementMode(MOVE_Flying);
        GetCharacterMovement()->bOrientRotationToMovement = false;

        // 设置很大的惯性参数
        GetCharacterMovement()->Mass = 200.0f;                       // 增加质量产生更大惯性
        GetCharacterMovement()->BrakingDecelerationFlying = 1000.0f; // 大幅降低空中制动产生惯性
        GetCharacterMovement()->AirControl = 0.1f;                   // 降低空中控制增加惯性感

        // 设置飞行速度
        GetCharacterMovement()->MaxFlySpeed = 100.0f; // 设置较高的飞行速度

        UE_LOG(LogTemp, Warning, TEXT("设置飞行速度: %.1f"), GetCharacterMovement()->MaxFlySpeed);
        UE_LOG(LogTemp, Warning, TEXT("进入攀爬，设置大惯性参数"));
    }
}

/**
 * 是否进入攀爬
 */
EClimbType AOperation_character::CheckClimbType()
{
    FVector CharacterLocation = GetActorLocation();
    FHitResult HitResult;
    bool IsHit = GetWorld()->LineTraceSingleByChannel(HitResult, CharacterLocation, CharacterLocation + GetActorForwardVector() * 50.0f, ECC_Visibility);

    if (!IsHit || HitResult.GetActor()->ActorHasTag(FName("notclimb")))
    {
        return EClimbType::None;
    }

    // 计算障碍物相对于角色脚底的高度
    float ObstacleHeight = HitResult.ImpactPoint.Z - (GetActorLocation().Z - GetCapsuleComponent()->GetScaledCapsuleHalfHeight());

    UE_LOG(LogTemp, Warning, TEXT("障碍物高度: %.1f cm"), ObstacleHeight);

    // 根据高度判断类型 (单位: cm)
    if (ObstacleHeight < 80.0f)
    {
        return EClimbType::None; // 太矮，直接走过去
    }
    else if (ObstacleHeight <= 160.0f)
    {
        return EClimbType::Vault; // 胸口高度，适合翻越
    }
    else if (ObstacleHeight <= 320.0f)
    {
        return EClimbType::Climb; // 较高，需要攀爬
    }
    else
    {
        return EClimbType::None; // 太高，无法攀爬
    }
}

bool AOperation_character::ComeinClimb()
{
    EClimbType Type = CheckClimbType();
    if (Type == EClimbType::None)
    {
        return false;
    }

    FVector CharacterLocation = GetActorLocation();
    FHitResult HitResult;
    bool IsHit = GetWorld()->LineTraceSingleByChannel(HitResult, CharacterLocation, CharacterLocation + GetActorForwardVector() * 50.0f, ECC_Visibility);

    if (IsHit)
    {
        FRotator Rotation = HitResult.ImpactNormal.Rotation();
        Rotation.Yaw += 180.0f;
        Rotation.Pitch = Rotation.Pitch * -1.0f;
        SetActorRotation(Rotation);

        IsHit = GetWorld()->LineTraceSingleByChannel(HitResult, GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 50.0f, ECC_Visibility);
        if (IsHit)
        {
            SetActorLocation(HitResult.ImpactPoint - (GetActorForwardVector() * (GetCapsuleComponent()->GetUnscaledCapsuleRadius() + 20.0f)));
        }

        SwitchClimb();

        if (Type == EClimbType::Vault)
        {
            UE_LOG(LogTemp, Warning, TEXT("进入翻越状态 (Vault)"));
            // 可在此处添加翻越蒙太奇或逻辑
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("进入攀爬状态 (Climb)"));
        }

        return true;
    }
    return false;
}

bool AOperation_character::NextPosition(FVector NextLocation)
{
    FVector CharacterLocation = GetActorLocation();
    FVector NextLocations = CharacterLocation + NextLocation;
    // 画线
    DrawDebugLine(GetWorld(), GetActorLocation() + GetActorUpVector() * 80.0f, GetActorLocation() + GetActorUpVector() * 80.0f + GetActorForwardVector() * 80.0f, FColor::Red, false, 0.0f, 0, 1.0f);       // 上
    DrawDebugLine(GetWorld(), GetActorLocation() - GetActorUpVector() * 80.0f, GetActorLocation() - GetActorUpVector() * 80.0f + GetActorForwardVector() * 80.0f, FColor::Red, false, 0.0f, 0, 1.0f);       // 下
    DrawDebugLine(GetWorld(), GetActorLocation() - GetActorRightVector() * 50.0f, GetActorLocation() - GetActorRightVector() * 50.0f + GetActorForwardVector() * 80.0f, FColor::Red, false, 0.0f, 0, 1.0f); // 左
    DrawDebugLine(GetWorld(), GetActorLocation() + GetActorRightVector() * 50.0f, GetActorLocation() + GetActorRightVector() * 50.0f + GetActorForwardVector() * 80.0f, FColor::Red, false, 0.0f, 0, 1.0f); // 右
    FHitResult HitResult;
    FHitResult HitResultUP;
    FHitResult HitResultDown;
    FHitResult HitResultLeft;
    FHitResult HitResultRight;
    bool IsHitUP = GetWorld()->LineTraceSingleByChannel(HitResultUP, CharacterLocation + GetActorUpVector() * 80.0f, CharacterLocation + GetActorUpVector() * 80.0f + GetActorForwardVector() * 80.0f, ECC_Visibility);
    bool IsHitDown = GetWorld()->LineTraceSingleByChannel(HitResultDown, CharacterLocation - GetActorUpVector() * 80.0f, CharacterLocation - GetActorUpVector() * 80.0f + GetActorForwardVector() * 80.0f, ECC_Visibility);
    bool IsHitLeft = GetWorld()->LineTraceSingleByChannel(HitResultLeft, CharacterLocation - GetActorRightVector() * 30.0f, CharacterLocation - GetActorRightVector() * 30.0f + GetActorForwardVector() * 80.0f, ECC_Visibility);
    bool IsHitRight = GetWorld()->LineTraceSingleByChannel(HitResultRight, CharacterLocation + GetActorRightVector() * 30.0f, CharacterLocation + GetActorRightVector() * 30.0f + GetActorForwardVector() * 80.0f, ECC_Visibility);

    bool IsHit = GetWorld()->LineTraceSingleByChannel(HitResult, CharacterLocation, NextLocations, ECC_Visibility);

    if (!IsHitRight)
    {

        IsHit = GetWorld()->LineTraceSingleByChannel(HitResult, CharacterLocation + GetActorRightVector() * 50.0f + GetActorForwardVector() * 7 * 20, CharacterLocation + GetActorRightVector() * 50.0f + GetActorForwardVector() * 7 * 20 - GetActorRightVector() * 30.0f, ECC_Visibility);
        if (IsHit)
        { // 播放蒙太奇
            if (!ismontage)
            {
                MotionWarpingComp->RemoveWarpTarget(FName("second"));
                FMotionWarpingTarget StartTarget;
                StartTarget.Name = FName("second");
                StartTarget.Location = HitResult.ImpactPoint + HitResult.ImpactNormal * 50 + GetActorForwardVector() * 100;
                StartTarget.Rotation = FRotator(HitResult.ImpactNormal.Rotation().Pitch, HitResult.ImpactNormal.Rotation().Yaw + 180, 0);
                MotionWarpingComp->AddOrUpdateWarpTarget(StartTarget);
                PlayAnimMontage(ClimbRightMontage);
                UE_LOG(LogTemp, Warning, TEXT("ClimbRightMontage"));
                ismontage = true;
            }
        }
    }
    if (!IsHitLeft)
    {

        IsHit = GetWorld()->LineTraceSingleByChannel(HitResult, CharacterLocation - GetActorRightVector() * 50.0f + GetActorForwardVector() * 6 * 20, CharacterLocation - GetActorRightVector() * 50.0f + GetActorForwardVector() * 6 * 20 + GetActorRightVector() * 30.0f, ECollisionChannel::ECC_Visibility);
        if (IsHit)
        { // 画命中点
            if (!ismontage)
            {
                MotionWarpingComp->RemoveWarpTarget(FName("first"));
                FMotionWarpingTarget StartTarget;
                StartTarget.Name = FName("first");
                StartTarget.Location = HitResult.ImpactPoint + HitResult.ImpactNormal * 50;
                StartTarget.Rotation = FRotator(HitResult.ImpactNormal.Rotation().Pitch, HitResult.ImpactNormal.Rotation().Yaw + 180, 0);
                MotionWarpingComp->AddOrUpdateWarpTarget(StartTarget);
                PlayAnimMontage(ClimbLeftMontage);
                UE_LOG(LogTemp, Warning, TEXT("ClimbRightMontage"));
                ismontage = true;
            }
        }
    }
    if (!IsHitUP) // 翻越
    {
        IsHit = GetWorld()->LineTraceSingleByChannel(HitResult, NextLocations, NextLocations + GetActorForwardVector() * 50.0f, ECC_Visibility);
        if (!IsHit) // 不可攀爬准备翻越或转向
        {
            for (int i = 0; i < 10; i++)
            {
                IsHit = GetWorld()->LineTraceSingleByChannel(HitResult, NextLocations + GetActorForwardVector() * i * 20, NextLocations + GetActorForwardVector() * i * 20 - GetActorUpVector() * 100.0f, ECC_Visibility);
                DrawDebugLine(GetWorld(), NextLocations + GetActorForwardVector() * i * 20, NextLocations + GetActorForwardVector() * i * 20 - GetActorUpVector() * 50.0f, FColor::Red, false, 0.0f, 0, 1.0f);
                if (IsHit)
                {
                    // 移除指定名字的 Warp Target
                    if (!ismontage)
                    {
                        FVector temp = GetActorLocation();
                        temp.Z += 80.0f;
                        temp += GetActorForwardVector() * 10;
                        MotionWarpingComp->RemoveWarpTarget(FName("bianyuan"));
                        MotionWarpingComp->RemoveWarpTarget(FName("zhanli"));
                        FMotionWarpingTarget StartTarget;
                        StartTarget.Name = FName("bianyuan");
                        StartTarget.Location = temp;
                        StartTarget.Rotation = GetActorRotation();
                        MotionWarpingComp->AddOrUpdateWarpTarget(StartTarget);
                        FMotionWarpingTarget EndTarget;
                        EndTarget.Name = FName("zhanli");
                        EndTarget.Location = HitResult.ImpactPoint + GetActorForwardVector() * 20;
                        EndTarget.Rotation = GetActorRotation();
                        MotionWarpingComp->AddOrUpdateWarpTarget(EndTarget);
                        PlayAnimMontage(ClimbMontage);
                        ismontage = true;
                        // SwitchClimb();

                        UE_LOG(LogTemp, Warning, TEXT("准备翻越"));
                    }
                    else
                    {
                        UE_LOG(LogTemp, Warning, TEXT("-----------------------------------------"));
                    }
                    return true;
                }
            }
        }
    }

    IsHitLeft = GetWorld()->LineTraceSingleByChannel(HitResult, GetActorLocation(),
                                                     GetActorLocation() - GetActorRightVector() * 70.0f, ECC_Visibility);
    if (IsHitLeft)
    {
        if (!ismontage)
        {

            PlayAnimMontage(ClimbLeftMontage2);
            ismontage = true;
        }
        UE_LOG(LogTemp, Warning, TEXT("向左转"));
    }
    IsHitRight = GetWorld()->LineTraceSingleByChannel(HitResult, GetActorLocation(), GetActorLocation() + GetActorRightVector() * 70.0f, ECC_Visibility);
    if (IsHitRight)
    {
        if (!ismontage)
        {
            PlayAnimMontage(ClimbRightMontage2);
            ismontage = true;
        }
        UE_LOG(LogTemp, Warning, TEXT("向右转"));
    }

    return true;
}

void AOperation_character::InitBackPack()
{
}
/*
为背包添加物品
*/
void AOperation_character::AddItemToBackPack(UItem *Item)
{
    if (!Item)
    {
        UE_LOG(LogTemp, Warning, TEXT("物品为空"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("添加物品%d"), Item->id);

    FBackpackSlot *BackpackSlot = Backpack.Find(Item->backtype);
    if (BackpackSlot)
    {
        // 已存在，直接添加
        BackpackSlot->Items.Add(Item);
    }
    else
    {
        // Key 不存在，先创建一个 FBackPackType 再添加
        FBackpackSlot NewSlot;
        NewSlot.Items.Add(Item);
        Backpack.Add(Item->backtype, NewSlot);
    }
}
bool AOperation_character::UseItem(int32 index)
{
    // 获取游戏实例
    UMyGameInstance *GameInstance = Cast<UMyGameInstance>(GetGameInstance());
    if (!GameInstance)
    {
        UE_LOG(LogTemp, Warning, TEXT("游戏实例为空"));
        return false;
    }
    UDataTable *ItemTable = GameInstance->ItemTable;
    if (!ItemTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("物品表为空"));
        return false;
    }
    FMyDataTable *Row = ItemTable->FindRow<FMyDataTable>(FName(*FString::Printf(TEXT("%d"), index)), TEXT(""));
    if (Row && Row->ItemID == index)
    {
        UE_LOG(LogTemp, Warning, TEXT("物品名称%s"), *Row->Name);

        switch (Row->ItemType)
        {
        case EBackPack::wuqi:
        { /* code */
            UDataTable *DetailTable = GameInstance->ItemDetailTable.FindRef(Row->ItemType);
            if (!DetailTable)
            {
                UE_LOG(LogTemp, Error, TEXT("找不到物品详细表: %d"), static_cast<int32>(Row->ItemType));
                return false;
            }

            FMyDataTable_iteam_weapon *temp = DetailTable->FindRow<FMyDataTable_iteam_weapon>(FName(*FString::Printf(TEXT("%d"), index)), TEXT(""));
            Weapon->WeaponType = temp->WeaponType;
            Weapon->WeaponMesh1.Empty(); // 清空数组
            if (temp->bHasSkeletalMesh)
            {
                Weapon->WeaponMeshs = temp->SkeletalMesh;
                Weapon->isMesh = true;
                Weapon->ThrowObject = temp->ThrowObject;
            }
            else
            {
                Weapon->WeaponMesh1.Append(temp->StaticMesh);
                Weapon->isMesh = false;
                UE_LOG(LogTemp, Warning, TEXT("普通武器"));
            }
            EquipWeaponBytype();
            break;
        }
        case EBackPack::fangju:
            /* code */
            break;
        case EBackPack::yaoji:
            /* code */

            break;

        default:
            break;
        }

        return true;
    }

    return true;
}
