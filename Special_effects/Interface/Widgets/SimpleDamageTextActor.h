// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TextRenderComponent.h"
#include "SimpleDamageTextActor.generated.h"

UCLASS()
class RPG_TU2_API ASimpleDamageTextActor : public AActor
{
    GENERATED_BODY()

public:
    ASimpleDamageTextActor();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    // 文本渲染组件
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DamageText")
    TObjectPtr<UTextRenderComponent> DamageTextComponent;

    // 显示的伤害值
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageText")
    int32 DamageValue;

    // 显示持续时间
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageText")
    float DisplayDuration;

    // 移动速度
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageText")
    float FloatSpeed;

    // 开始时间
    float StartTime;

    // 初始化伤害文本
    UFUNCTION(BlueprintCallable, Category = "DamageText")
    void InitDamageText(int32 Damage, FVector Location);
};