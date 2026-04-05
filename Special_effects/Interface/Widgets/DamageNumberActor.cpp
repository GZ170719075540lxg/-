// Fill out your copyright notice in the Description page of Project Settings.

#include "DamageNumberActor.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ADamageNumberActor::ADamageNumberActor()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    // 创建根组件
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));

    // 创建Widget组件
    DamageNumberWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("DamageNumberWidget"));
    DamageNumberWidget->SetupAttachment(RootComponent);

    // 默认参数设置
    DamageValue = 0;
    DisplayDuration = 1.5f;
    FloatSpeed = 50.0f;
    FadeOutSpeed = 0.5f;
    bAlwaysFaceCamera = true;
    StartTime = 0.0f;

    // 设置Widget组件属性
    DamageNumberWidget->SetDrawSize(FVector2D(200.0f, 100.0f));
    DamageNumberWidget->SetWidgetSpace(EWidgetSpace::World);
    DamageNumberWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    DamageNumberWidget->SetDrawAtDesiredSize(true);
}

// Called when the game starts or when spawned
void ADamageNumberActor::BeginPlay()
{
    Super::BeginPlay();

    StartTime = GetWorld()->GetTimeSeconds();
}

// Called every frame
void ADamageNumberActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 更新朝向
    if (bAlwaysFaceCamera)
    {
        UpdateFacingDirection();
    }

    // 向上移动
    FVector NewLocation = GetActorLocation() + FVector(0.0f, 0.0f, FloatSpeed * DeltaTime);
    SetActorLocation(NewLocation);

    // 检查是否超过显示时间
    float CurrentTime = GetWorld()->GetTimeSeconds();
    float ElapsedTime = CurrentTime - StartTime;

    if (ElapsedTime >= DisplayDuration)
    {
        Destroy();
    }
    else if (ElapsedTime >= DisplayDuration * 0.7f) // 开始淡出
    {
        // 计算淡出比例
        float FadeRatio = (ElapsedTime - DisplayDuration * 0.7f) / (DisplayDuration * 0.3f);
        FadeRatio = FMath::Clamp(FadeRatio, 0.0f, 1.0f);

        // 应用淡出效果
        SetActorScale3D(FMath::Lerp(FVector(1.0f), FVector(0.1f), FadeRatio));
    }
}

void ADamageNumberActor::InitDamageNumber(int32 Damage, FVector Location)
{
    DamageValue = Damage;
    SetActorLocation(Location);

    UE_LOG(LogTemp, Warning, TEXT("伤害数字初始化: %d, 位置: %s"), Damage, *Location.ToString());

    // 记录开始时间
    StartTime = GetWorld()->GetTimeSeconds();
}

void ADamageNumberActor::UpdateFacingDirection()
{
    // 获取玩家控制器
    APlayerController *PC = GetWorld()->GetFirstPlayerController();
    if (PC && PC->PlayerCameraManager)
    {
        // 获取摄像机位置
        FVector CameraLocation = PC->PlayerCameraManager->GetCameraLocation();
        FVector ActorLocation = GetActorLocation();

        // 计算朝向摄像机的旋转
        FRotator LookAtRotation = (CameraLocation - ActorLocation).Rotation();

        // 只保持Y轴旋转，避免上下翻转
        LookAtRotation.Pitch = 0.0f;
        LookAtRotation.Roll = 0.0f;

        SetActorRotation(LookAtRotation);
    }
}