// Fill out your copyright notice in the Description page of Project Settings.

#include "MyUserWidget.h"
#include "../../Characters/Player/character_my.h"

void UMyUserWidget::NativeConstruct()
{
    Super::NativeConstruct();
    // ...
    // 调用者
}
void UMyUserWidget::Init(Acharacter_my *character)
{
    this->Acharacter = character;
}