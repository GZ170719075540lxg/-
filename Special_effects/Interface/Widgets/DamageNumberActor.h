// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/WidgetComponent.h"
#include "DamageNumberActor.generated.h"

UCLASS()
class RPG_TU2_API ADamageNumberActor : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ADamageNumberActor();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Widget组件用于显示伤害数字
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DamageNumber", meta = (DisplayName = "伤害数字组件"))
    TObjectPtr<UWidgetComponent> DamageNumberWidget;

    // 显示的伤害值
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageNumber", meta = (DisplayName = "伤害值"))
    int32 DamageValue;

    // 显示持续时间
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageNumber", meta = (DisplayName = "显示持续时间"))
    float DisplayDuration;

    // 移动速度（向上飘动）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageNumber", meta = (DisplayName = "上浮速度"))
    float FloatSpeed;

    // 淡出速度
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageNumber", meta = (DisplayName = "淡出速度"))
    float FadeOutSpeed;

    // 是否始终面向摄像机
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageNumber", meta = (DisplayName = "始终面向摄像机"))
    bool bAlwaysFaceCamera;

    // 开始时间记录
    float StartTime;

    // 初始化伤害数字
    UFUNCTION(BlueprintCallable, Category = "DamageNumber", meta = (DisplayName = "初始化伤害数字"))
    void InitDamageNumber(int32 Damage, FVector Location);

    // 更新朝向玩家
    void UpdateFacingDirection();
};