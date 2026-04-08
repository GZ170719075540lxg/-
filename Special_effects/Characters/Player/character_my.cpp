// Fill out your copyright notice in the Description page of Project Settings.
#include "character_my.h"
#include "GameFramework/CharacterMovementComponent.h" // 添加这个头文件
#include "Kismet/KismetSystemLibrary.h"               // 添加这一行
#include "Engine/EngineTypes.h"
#include "character_my.h"
#include "Engine/StaticMesh.h"       // 对于 UStaticMesh
#include "Engine/StaticMeshSocket.h" // 对于 UStaticMeshSocket
#include "AIController.h"
#include "../../Core/Base/Status.h"
#include "../../Data/Tables/item/MyDataTable_iteam.h"
#include "../../Core/Managers/MyGameInstance.h"
#include "BrainComponent.h"

void Acharacter_my::set_character_name(FString name)
{
    character_name = name;
}
Acharacter_my::Acharacter_my()
{
    Level = 1;
    MaxExp = 100;
    NowExp = 54;
    CanAttack = true;
    Health = 100;
    NowHealth = Health;
    HealthPercent = 1.0f;
    Stamina = 100;
    NowStamina = Stamina;
    StaminaPercent = 1.0f;
    Attack = 10;
    Defense = 10;
    AttackNum = 0;
    AttackMaxNum = 3;
    AttackCD.Add(0.5f);
    AttackCD.Add(0.5f);
    Tolerance = 0.0f;
    ToleranceMax = 100.0f;

    // 禁用控制器旋转Yaw
    bUseControllerRotationYaw = false;

    // 启用角色朝向移动方向
    GetCharacterMovement()->bOrientRotationToMovement = true;

    // 遍历枚举类
    for (int32 i = 0; i < static_cast<int32>(ESock::MAX); ++i)
    {
        // 将整数转换为枚举类型
        ESock WeaponType = static_cast<ESock>(i);
        // 获取枚举名称
        FString EnumName = UEnum::GetValueAsString(WeaponType);
        // 创建静态网格组件
        UStaticMeshComponent *StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(*EnumName);
        // 添加静态网格组件到武器组件
        StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        WeaponComp.Add(WeaponType, StaticMeshComponent);
        // 无碰撞
    }
    WeaponMeshs = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
    WeaponMeshs->SetupAttachment(GetMesh(), FName("WeaponSocket"));
}
/*
更新血量
*/
bool Acharacter_my::UpdateHealth(int32 value)
{
    NowHealth -= value;
    if (NowHealth <= 0)
    {
        NowHealth = 0;
        HealthPercent = 0.0f;
        return false;
    }
    if (NowHealth >= Health)
    {
        NowHealth = Health;
    }
    HealthPercent = (float)NowHealth / (float)Health;
    UE_LOG(LogTemp, Log, TEXT("血量更新完成 %f"), HealthPercent);
    return true;
}
/*
更新体力
*/
bool Acharacter_my::UpdateStamina(int32 value)
{
    UE_LOG(LogTemp, Log, TEXT("体力更新"));
    NowStamina -= value;
    if (NowStamina <= 0)
    {
        NowStamina = 0;
        StaminaPercent = 0.0f;
        if (GetWorldTimerManager().IsTimerActive(TimerSprintDown))
        {
            GetWorldTimerManager().ClearTimer(TimerSprintDown);
            GetWorldTimerManager().SetTimer(TimerSprintUP, [this]()
                                            { UpdateStamina(-1.0f); }, 0.1f, true);
            GetCharacterMovement()->MaxWalkSpeed = 600.0f;
            issprint = false;
        }
        return false;
    }
    if (NowStamina >= Stamina)
    {
        NowStamina = Stamina;
        if (GetWorldTimerManager().IsTimerActive(TimerSprintUP))
        {
            GetWorldTimerManager().ClearTimer(TimerSprintUP);
        }
    }
    StaminaPercent = (float)NowStamina / (float)Stamina;
    UE_LOG(LogTemp, Log, TEXT("体力更新完成%d"), NowStamina);
    return true;
}
/*
更新攻击
*/
void Acharacter_my::UpdateAttack(int32 value)
{
    Attack += value;
}
/*
更新防御
*/
void Acharacter_my::UpdateDefense(int32 value)
{
    Defense += value;
}
void Acharacter_my::ClearAttackNum()
{
    AttackNum = 0;
}
/*
更新攻击数
*/

void Acharacter_my::UpdateAttackNum()
{
    AttackNum++;
    if (AttackNum >= AttackMaxNum)
    {
        AttackNum = 0;
    }
}

/*
角色受击函数
*/
void Acharacter_my::CharacterHit(int32 value, FVector AttackDir)
{
    UE_LOG(LogTemp, Log, TEXT("角色受击函数"));

    // 显示伤害数字
    ShowDamageNumber(value, GetActorLocation());

    if (!UpdateHealth(value))
    {
        // 销毁角色
        Destroy();
    }
    AttackDir.Z = 0.0f;
    // 向受击方向运动
    // 播放受击动画    进入僵持
    LaunchCharacter(
        AttackDir * 6.f, // 击退力度
        true,
        true);
    // 延迟0.1s
    //  stalemate();
    Stalemate();
    if (HitMontage)
    {
        UE_LOG(LogTemp, Log, TEXT("播放受击动画"));
        PlayAnimMontage(HitMontage);
    }
}

/*
显示伤害数字
*/
void Acharacter_my::ShowDamageNumber(int32 Damage, FVector Location)
{
    UE_LOG(LogTemp, Warning, TEXT("尝试显示伤害数字: %d, 位置: %s"), Damage, *Location.ToString());

    // 确保有有效的世界
    if (!GetWorld())
    {
        UE_LOG(LogTemp, Error, TEXT("无法获取世界对象"));
        return;
    }

    // 在受伤位置稍微上方显示伤害数字
    FVector DisplayLocation = Location + FVector(0.0f, 0.0f, 100.0f);

    UE_LOG(LogTemp, Warning, TEXT("生成位置: %s"), *DisplayLocation.ToString());

    // 首先尝试使用简单的文本渲染版本
    ASimpleDamageTextActor *SimpleDamageText = GetWorld()->SpawnActor<ASimpleDamageTextActor>(ASimpleDamageTextActor::StaticClass(), DisplayLocation, FRotator::ZeroRotator);

    if (SimpleDamageText)
    {
        UE_LOG(LogTemp, Warning, TEXT("成功创建简单伤害文本Actor"));
        SimpleDamageText->InitDamageText(Damage, DisplayLocation);
        return; // 成功创建简单版本就返回
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("创建简单伤害文本Actor失败"));
    }

    // 如果简单版本失败，再尝试复杂版本
    ADamageNumberActor *DamageNumber = GetWorld()->SpawnActor<ADamageNumberActor>(ADamageNumberActor::StaticClass(), DisplayLocation, FRotator::ZeroRotator);

    if (DamageNumber)
    {
        UE_LOG(LogTemp, Warning, TEXT("成功创建伤害数字Actor"));

        // 初始化伤害数字
        DamageNumber->InitDamageNumber(Damage, DisplayLocation);

        // 如果Widget组件有绑定的Widget类，设置伤害值
        if (DamageNumber->DamageNumberWidget)
        {
            UE_LOG(LogTemp, Warning, TEXT("Widget组件存在"));
            // 这里可以在后续连接具体的Widget类后再设置
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Widget组件为空"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("创建伤害数字Actor失败"));
    }
}
/*
普通攻击
*/
/**
 * 执行普通攻击功能
 * 该函数检查角色是否可以攻击，并播放相应的攻击动画
 * 同时设置攻击冷却时间和攻击重置定时器
 */
void Acharacter_my::AttackNormal()
{
    UE_LOG(LogTemp, Log, TEXT("普通攻击"));
    // 检查角色是否可以攻击
    if (CanAttack)
    {
        if (!Weapon)
            return;
        UE_LOG(LogTemp, Log, TEXT("武器类型：%s"), *UEnum::GetValueAsString(Weapon->WeaponType));
        switch (Weapon->WeaponType)
        {
        case EWeaponType::changjian:
            if (AttackMontageList.Num() > AttackNum)
            {
                // 播放攻击动画
                Character_Status = EStatus::CMS_Attach;
                PlayAnimMontage(AttackMontageList[AttackNum]);
                // 设置攻击状态为不可攻击
                CanAttack = false;
                // 设置攻击冷却定时器，冷却时间结束后恢复攻击能力
                GetWorldTimerManager().SetTimer(TimerAttack, [this]()
                                                { CanAttack = true; }, AttackCD[AttackNum], false);
                // 清理定时器
                // GetWorldTimerManager().ClearTimer(TimerAttackReset);

                // GetWorldTimerManager().SetTimer(TimerAttackReset, [this]()
                //                                 { ClearAttackNum(); }, AttackCDNow[AttackNum], false);
                UpdateAttackNum();
            }
            else
            {
                UE_LOG(LogTemp, Log, TEXT("没有技能"));
            }
            break;
        case EWeaponType::taidao:
            if (AttackMontageList.Num() > AttackNum)
            {
                // 播放攻击动画
                Character_Status = EStatus::CMS_Attach;
                PlayAnimMontage(AttackMontageList[AttackNum]);
                // 设置攻击状态为不可攻击
                CanAttack = false;
                // 设置攻击冷却定时器，冷却时间结束后恢复攻击能力
                GetWorldTimerManager().SetTimer(TimerAttack, [this]()
                                                { CanAttack = true; }, AttackCD[AttackNum], false);
                // 清理定时器
                // GetWorldTimerManager().ClearTimer(TimerAttackReset);

                // GetWorldTimerManager().SetTimer(TimerAttackReset, [this]()
                //                                 { ClearAttackNum(); }, AttackCDNow[AttackNum], false);
                UpdateAttackNum();
            }
            else
            {
                UE_LOG(LogTemp, Log, TEXT("没有技能"));
            }
            break;

        case EWeaponType::gongjian:
        {
            Character_Status = EStatus::CMS_Idle;
            bUseControllerRotationYaw = false;
            //
            UE_LOG(LogTemp, Log, TEXT("弓箭攻击方向%s"), *GetControlRotation().Vector().ToString());

            //
            FHitResult HitResult;

            DrawDebugLine(GetWorld(), GetActorLocation() + GetActorRightVector() * 45.f + FVector(0.0f, 0.0f, 16.0f),
                          GetActorLocation() + FVector(0.0f, 0.0f, 16.0f) + GetActorRightVector() * 45.f + (GetControlRotation().Vector() + FVector(0.0f, 0.0f, 0.07f)) * 100000.f, FColor::Red, false, 1.f, 0, 1.f);
            bool Hit = GetWorld()->LineTraceSingleByChannel(HitResult, GetActorLocation() + GetActorRightVector() * 45.f + FVector(0.0f, 0.0f, 16.0f), GetActorLocation() + FVector(0.0f, 0.0f, 16.0f) + GetActorRightVector() * 45.f + (GetControlRotation().Vector() + FVector(0.0f, 0.0f, 0.07f)) * 100000.f, ECC_Visibility);
            FRotator Rotation = GetActorRotation();
            FVector Location = GetActorLocation() + GetActorRightVector() * 45.f + FVector(0.0f, 0.0f, 16.0f);

            if (Hit)
            {
                Rotation = FRotationMatrix::MakeFromX(HitResult.ImpactPoint - GetActorLocation()).Rotator();
            }
            Weapon->DrawBowMontage(Weapon->ThrowObject, GetActorLocation(), Rotation);
            SpringArm->SetRelativeLocation(FVector(0.0f, 0.f, 0.0f));
            SpringArm->TargetArmLength = 400.f;
        }
        break;
        case EWeaponType::quantou:
            if (AttackMontageList.Num() > AttackNum)
            {
                // 播放攻击动画
                Character_Status = EStatus::CMS_Attach;
                PlayAnimMontage(AttackMontageList[AttackNum]);
                // 设置攻击状态为不可攻击
                CanAttack = false;
                // 设置攻击冷却定时器，冷却时间结束后恢复攻击能力
                GetWorldTimerManager().SetTimer(TimerAttack, [this]()
                                                { CanAttack = true; }, AttackCD[AttackNum], false);
                // 清理定时器
                // GetWorldTimerManager().ClearTimer(TimerAttackReset);

                // GetWorldTimerManager().SetTimer(TimerAttackReset, [this]()
                //                                 { ClearAttackNum(); }, AttackCDNow[AttackNum], false);
                UpdateAttackNum();
            }
            else
            {
                UE_LOG(LogTemp, Log, TEXT("没有技能"));
            }
            break;
        }
        // 检查当前攻击序号对应的动画是否存在
        // 根据武器类型选择不同方式攻击
    }
}
/*
技能攻击
*/
void Acharacter_my::AttackSkill(int32 SkillNum)
{
}
/*
攻击的追踪
*/

void Acharacter_my::AttackTrackTimer()
{
    GetWorldTimerManager().ClearTimer(TimerAttackTrack);
    GetWorldTimerManager().SetTimer(TimerAttackTrack, [this]()
                                    { AttackTrack(); }, 0.01f, true);
}

void Acharacter_my::AttackTrackStop()
{
    GetWorldTimerManager().ClearTimer(TimerAttackTrack);
    AttackTrackPoint.Empty();
    AttackTarget.Empty();
}
/**
 * 执行攻击追踪功能
 * 该函数用于实现武器攻击的追踪检测，包括检测命中目标并造成伤害
 */
void Acharacter_my::AttackTrack()
{
    // 存储碰撞结果的数组
    TArray<FHitResult> HitResults;
    // 获取当前世界对象
    UWorld *World = GetWorld();
    // 获取武器组件
    UStaticMeshComponent *Weapons = WeaponComp.FindRef(ESock::SockR); // 你的 Weapon 本身就是组件
    // 获取武器网格静态网格
    UStaticMesh *WeaponMesh = Weapons->GetStaticMesh();
    // 如果没有武器网格则直接返回
    if (!WeaponMesh)
        return;
    // 初始化起始和结束位置为零向量
    FVector StartPos = FVector::ZeroVector;
    FVector EndPos = FVector::ZeroVector;

    // 遍历武器网格上的所有插槽
    for (UStaticMeshSocket *Socket : WeaponMesh->Sockets)
    {
        // 如果插槽无效则跳过
        if (!Socket)
            continue;

        // 检查是否为"开始位置"标记的插槽
        if (Socket->Tag == FName(TEXT("开始位置")))
        {
            // 获取插槽位置并设置为起始位置
            StartPos = Weapons->GetSocketLocation(Socket->SocketName);
        }
        // 检查是否为"结束位置"标记的插槽
        else if (Socket->Tag == FName(TEXT("结束位置")))
        {
            // 获取插槽位置并设置为结束位置
            EndPos = Weapons->GetSocketLocation(Socket->SocketName);
        }
    }

    if (AttackTrackPoint.Num() == 0)
    {
        AttackTrackPoint.Add(StartPos);
        AttackTrackPoint.Add(EndPos);
    }
    // 线性追踪
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);        // 忽略自己
    Params.AddIgnoredComponent(Weapons); // 忽略武器自身
    UStaticMeshComponent *Shield = WeaponComp.FindRef(ESock::SockL);
    if (WeaponComp.FindRef(ESock::SockL))
        Params.AddIgnoredComponent(Shield);
    // 多目标线性追踪
    bool bHit = World->LineTraceMultiByChannel(
        HitResults,
        AttackTrackPoint[0],
        StartPos,
        ECC_Pawn, // 或自定义 TraceChannel
        Params);
    // Debug 可视化
    // DrawDebugLine(World, StartPos, EndPos, bHit ? FColor::Red : FColor::Green, false, 2.f, 0, 5.f);
    for (auto &Hit : HitResults)
    {
        // DrawDebugPoint(World, Hit.ImpactPoint, 10.f, FColor::Yellow, false, 2.f);
        // UE_LOG(LogTemp, Log, TEXT("命中对象: %s"), *Hit.GetActor()->GetName());
        TObjectPtr<Acharacter_my> temp = Cast<Acharacter_my>(Hit.GetActor());
        if (!temp)
            continue;
        if (!AttackTarget.Contains(temp))
        {

            temp->CharacterHit(10, GetActorForwardVector() * 250);
            AttackTarget.Add(temp);
        }
    }
    // DrawDebugLine(GetWorld(), AttackTrackPoint[0], StartPos, FColor::Red, false, 1.f, 0, 5.0f);
    AttackTrackPoint[0] = StartPos;
    bHit = World->LineTraceMultiByChannel(
        HitResults,
        AttackTrackPoint[1],
        EndPos,
        ECC_Pawn, // 或自定义 TraceChannel
        Params);
    for (auto &Hit : HitResults)
    {
        // DrawDebugPoint(World, Hit.ImpactPoint, 10.f, FColor::Yellow, false, 2.f);
        UE_LOG(LogTemp, Log, TEXT("命中对象: %s"), *Hit.GetActor()->GetName());
        TObjectPtr<Acharacter_my> temp = Cast<Acharacter_my>(Hit.GetActor());
        if (!temp)
            continue;
        if (!AttackTarget.Contains(temp))
        {

            temp->CharacterHit(10, GetActorForwardVector() * 250);
            AttackTarget.Add(temp);
        }
    }
    //   DrawDebugLine(GetWorld(), AttackTrackPoint[1], EndPos, FColor::Red, false, 1.f, 0, 5.0f);
    AttackTrackPoint[1] = EndPos;
}

/**
 * 获取最近的目标位置
 * 该方法通过射线检测来寻找角色前方最近的目标
 * @return 返回最近目标的碰撞点，如果没有命中则返回角色前方的方向向量
 */
// FVector Acharacter_my::GetNearestTargetLocation()
// // {
// //     if (!GetWorld())
// //         return GetActorLocation() + GetActorForwardVector() * 500.f;

// //     FVector Start = GetActorLocation();
// //     float Radius = 300.f;
// //     FVector End = Start; // 静态球形检测，也可以向前延伸 End = Start + Forward*500

// //     TArray<FHitResult> HitResults;
// //     FCollisionQueryParams Params;
// //     Params.AddIgnoredActor(this);
// //     bool bHit = GetWorld()->SweepMultiByChannel(
// //         HitResults,
// //         Start,
// //         End,
// //         FQuat::Identity,
// //         ECC_Pawn,
// //         FCollisionShape::MakeSphere(Radius),
// //         Params);
// //     if (bHit)
// //     {
// //         for (auto &Hit : HitResults)
// //         {
// //             AActor *HitActor = Hit.GetActor();
// //             if (HitActor && HitActor->ActorHasTag(FName("istack")))
// //             {
// //                 UE_LOG(LogTemp, Log, TEXT("可攻击: "));
// //                 return Hit.GetActorLocation();
// //             }
// //         }
// //     }
// //     // 没命中就返回角色前方 500 单位位置
// //     return Start + GetActorForwardVector() * 500.f;
// }

/**
 * 角色进入僵持状态的函数
 * 该函数会立即停止角色的移动，并禁用移动功能
 * 同时会停止AI的当前行为，并设置一个定时器在1.5秒后退出僵持状态
 */
void Acharacter_my::Stalemate()
{

    // 1. 立即停止角色移动并禁用移动功能
    GetCharacterMovement()->StopMovementImmediately(); // 立即停止所有移动
    GetCharacterMovement()->DisableMovement();         // 完全禁用移动能力

    // 2. 停止 AI 行为
    // 尝试将当前控制器转换为AI控制器
    if (AAIController *AICon = Cast<AAIController>(GetController()))
    {
        AICon->StopMovement();                               // 停止AI的移动
        AICon->GetBrainComponent()->StopLogic(TEXT("Stun")); // 停止AI的决策逻辑，传入"Stun"作为原因
    }
    // TDOO:根据攻击不同使用不同僵持时间

    // 3. 定时退出僵持
    GetWorldTimerManager().SetTimer(
        TimerStalemate, [this]()
        {
            // 恢复移动
            GetCharacterMovement()->SetMovementMode(MOVE_Walking);

            // 恢复 AI
            if (AAIController *AICon = Cast<AAIController>(GetController()))
            {
                AICon->GetBrainComponent()->RestartLogic();
            } },
        0.5f, false);
}
/*
为角色装备武器
*/
void Acharacter_my::EquipWeapon(UStaticMesh *WeaponMesh, FName SocketName, UStaticMeshComponent *temp)
{
    if (WeaponMesh)
    {
        temp->SetStaticMesh(WeaponMesh);
        // 挂载到插槽*
        temp->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
    }
}

void Acharacter_my::EquipWeaponBytype()
{

    switch (Weapon->WeaponType)
    {
    case EWeaponType::taidao:
        ClearWeapon();
        EquipWeapon(Weapon->WeaponMesh1[0], "HandL2", WeaponComp[ESock::SockR]);
        EquipWeapon(Weapon->WeaponMesh1[1], "HandL", WeaponComp[ESock::SockL]);
        break;
        /* code */
        break;
    case EWeaponType::changjian:
        ClearWeapon();
        EquipWeapon(Weapon->WeaponMesh1[0], "HandR", WeaponComp[ESock::SockR]);
        /* code */
        break;
    case EWeaponType::gongjian:
        ClearWeapon();
        WeaponMeshs->SetSkeletalMesh(Weapon->WeaponMeshs);
        WeaponMeshs->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "HandL");
        WeaponMeshs->SetRelativeLocation(FVector(-3.000000, 4.000000, 0.000000));
        WeaponMeshs->SetRelativeRotation(FRotator(55.172525, 116.585503, -213.561016));

        break;
    }
}
/*
清理武器
*/
void Acharacter_my::ClearWeapon()
{
    for (auto &temp : WeaponComp)
    {
        temp.Value->SetStaticMesh(nullptr);
    }
    WeaponMeshs->SetSkeletalMesh(nullptr);
}

/**
 * 设置指定玩家的武器旋转角度
 * @param Sock 玩家的套接字标识符，用于确定要操作的玩家
 * @param Rotator 一个FRotator结构体，包含武器需要旋转到的目标角度
 */
/**
    // 根据套接字标识获取对应的武器组件，并设置其世界旋转角度
 * 武器旋转函数
 * @param Sock 套接字标识，用于确定是哪个角色的武器组件
 * @param Rotator 旋转器，包含旋转的角度信息
 */
void Acharacter_my::WeaponRotate(ESock Sock, FRotator Rotator, FVector Location)
{
    WeaponComp[Sock]->SetRelativeRotation(Rotator);
    WeaponComp[Sock]->SetRelativeLocation(Location);
}
