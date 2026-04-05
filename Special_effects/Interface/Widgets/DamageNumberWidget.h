// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageNumberWidget.generated.h"

/**
 * 伤害数字显示Widget类
 */
UCLASS()
class RPG_TU2_API UDamageNumberWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // 构造函数
    UDamageNumberWidget(const FObjectInitializer &ObjectInitializer);

protected:
    // 初始化Widget
    virtual void NativeConstruct() override;

public:
    // 设置伤害值
    UFUNCTION(BlueprintCallable, Category = "DamageNumber", meta = (DisplayName = "设置伤害值"))
    void SetDamageValue(int32 Damage);

    // 设置伤害颜色（根据伤害大小变化）
    UFUNCTION(BlueprintCallable, Category = "DamageNumber", meta = (DisplayName = "设置伤害颜色"))
    void SetDamageColor(int32 Damage);

    // 伤害值文本块
    UPROPERTY(meta = (BindWidget))
    class UTextBlock *DamageText;

    // 当前显示的伤害值
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageNumber", meta = (DisplayName = "当前伤害值"))
    int32 CurrentDamage;

    // 最小伤害阈值（用于颜色判断）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageNumber", meta = (DisplayName = "最小伤害阈值"))
    int32 MinDamageThreshold;

    // 中等伤害阈值
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageNumber", meta = (DisplayName = "中等伤害阈值"))
    int32 MediumDamageThreshold;

    // 最大伤害阈值
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageNumber", meta = (DisplayName = "最大伤害阈值"))
    int32 MaxDamageThreshold;
};