// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy_character.h"

AEnemy_character::AEnemy_character()
{
    HealthBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
    HealthBar->SetupAttachment(RootComponent);
    SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    // 缩放
    SphereComponent->SetWorldScale3D(FVector(20.0f, 20.0f, 20.0f));
    SphereComponent->SetupAttachment(RootComponent);
    // 组件开始重叠事件
    SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemy_character::OnOverlapBegin);
    // 结束
    SphereComponent->OnComponentEndOverlap.AddDynamic(this, &AEnemy_character::OnOverlapEnd);
}
void AEnemy_character::BeginPlay()
{

    Super::BeginPlay();
    if (HealthBar->GetWidget())
    {
        HealthBarWidget = Cast<UMyUserWidget>(HealthBar->GetWidget());
    }
    if (HealthBarWidget)
    {
        HealthBarWidget->Init(this);
        // 隐藏
        HealthBarWidget->SetVisibility(ESlateVisibility::Hidden);
    }
}
void AEnemy_character::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    // 将血条面向玩家
    if (HealthBar)
    {
        APlayerController *PC = GetWorld()->GetFirstPlayerController();
        if (PC)
        {
            FVector CamLoc = PC->PlayerCameraManager->GetCameraLocation();
            FVector WidgetLoc = HealthBar->GetComponentLocation();

            FRotator LookAtRot = (CamLoc - WidgetLoc).Rotation();
            HealthBar->SetWorldRotation(LookAtRot);
        }
    }
}

void AEnemy_character::OnOverlapBegin(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    UE_LOG(LogTemp, Warning, TEXT("OnOverlapBegin"));
    if (HealthBarWidget)
    {
        HealthBarWidget->SetVisibility(ESlateVisibility::Visible);
    }
}
void AEnemy_character::OnOverlapEnd(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
{
    UE_LOG(LogTemp, Warning, TEXT("OnOverlapEnd"));
    if (HealthBarWidget)
        HealthBarWidget->SetVisibility(ESlateVisibility::Hidden);
}
