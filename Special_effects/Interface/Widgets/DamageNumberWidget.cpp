// Fill out your copyright notice in the Description page of Project Settings.

#include "DamageNumberWidget.h"
#include "Components/TextBlock.h"

UDamageNumberWidget::UDamageNumberWidget(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
    // 初始化默认阈值
    MinDamageThreshold = 10;
    MediumDamageThreshold = 30;
    MaxDamageThreshold = 50;
    CurrentDamage = 0;
}

void UDamageNumberWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 初始化文本样式
    if (DamageText)
    {
        // 设置默认字体大小和对齐方式
        DamageText->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 24));
        DamageText->SetJustification(ETextJustify::Center);
    }
}

void UDamageNumberWidget::SetDamageValue(int32 Damage)
{
    CurrentDamage = Damage;

    if (DamageText)
    {
        // 设置文本内容
        DamageText->SetText(FText::AsNumber(Damage));

        // 根据伤害值设置颜色
        SetDamageColor(Damage);
    }
}

void UDamageNumberWidget::SetDamageColor(int32 Damage)
{
    if (!DamageText)
        return;

    // 根据伤害大小设置不同的颜色
    FLinearColor DamageColor;

    if (Damage >= MaxDamageThreshold)
    {
        // 高伤害 - 红色
        DamageColor = FLinearColor::Red;
    }
    else if (Damage >= MediumDamageThreshold)
    {
        // 中等伤害 - 橙色
        DamageColor = FLinearColor(1.0f, 0.5f, 0.0f, 1.0f); // 橙色
    }
    else if (Damage >= MinDamageThreshold)
    {
        // 普通伤害 - 黄色
        DamageColor = FLinearColor::Yellow;
    }
    else
    {
        // 低伤害 - 白色
        DamageColor = FLinearColor::White;
    }

    // 应用颜色到文本
    DamageText->SetColorAndOpacity(DamageColor);
}