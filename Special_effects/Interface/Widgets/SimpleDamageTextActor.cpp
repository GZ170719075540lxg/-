// Fill out your copyright notice in the Description page of Project Settings.

#include "SimpleDamageTextActor.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

ASimpleDamageTextActor::ASimpleDamageTextActor()
{
    PrimaryActorTick.bCanEverTick = true;

    // 创建根组件
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));

    // 创建文本渲染组件
    DamageTextComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("DamageTextComponent"));
    DamageTextComponent->SetupAttachment(RootComponent);

    // 默认设置
    DamageValue = 0;
    DisplayDuration = 2.0f;
    FloatSpeed = 80.0f;
    StartTime = 0.0f;

    // 配置文本组件
    DamageTextComponent->SetHorizontalAlignment(EHTA_Center);
    DamageTextComponent->SetVerticalAlignment(EVRTA_TextCenter);
    DamageTextComponent->SetWorldSize(50.0f);
    DamageTextComponent->SetText(FText::FromString(TEXT("0")));
    DamageTextComponent->SetMaterial(0, nullptr); // 使用默认材质
}

void ASimpleDamageTextActor::BeginPlay()
{
    Super::BeginPlay();
    StartTime = GetWorld()->GetTimeSeconds();
}

void ASimpleDamageTextActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 始终面向摄像机
    APlayerController *PC = GetWorld()->GetFirstPlayerController();
    if (PC && PC->PlayerCameraManager)
    {
        FVector CameraLocation = PC->PlayerCameraManager->GetCameraLocation();
        FVector ActorLocation = GetActorLocation();
        FRotator LookAtRotation = (CameraLocation - ActorLocation).Rotation();
        LookAtRotation.Pitch = 0.0f;
        LookAtRotation.Roll = 0.0f;
        SetActorRotation(LookAtRotation);
    }

    // 向上移动
    FVector NewLocation = GetActorLocation() + FVector(0.0f, 0.0f, FloatSpeed * DeltaTime);
    SetActorLocation(NewLocation);

    // 检查显示时间
    float CurrentTime = GetWorld()->GetTimeSeconds();
    if (CurrentTime - StartTime >= DisplayDuration)
    {
        Destroy();
    }
}

void ASimpleDamageTextActor::InitDamageText(int32 Damage, FVector Location)
{
    DamageValue = Damage;
    SetActorLocation(Location);

    // 设置文本内容
    DamageTextComponent->SetText(FText::AsNumber(Damage));

    // 根据伤害值设置颜色
    if (Damage >= 50)
    {
        DamageTextComponent->SetTextRenderColor(FColor::Red);
    }
    else if (Damage >= 30)
    {
        DamageTextComponent->SetTextRenderColor(FColor::Orange);
    }
    else if (Damage >= 10)
    {
        DamageTextComponent->SetTextRenderColor(FColor::Yellow);
    }
    else
    {
        DamageTextComponent->SetTextRenderColor(FColor::White);
    }

    StartTime = GetWorld()->GetTimeSeconds();

    UE_LOG(LogTemp, Warning, TEXT("简单伤害文本已初始化: %d"), Damage);
}