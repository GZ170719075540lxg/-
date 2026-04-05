// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../Characters/Player/character_my.h"
#include "MyUserWidget.generated.h"

/**
 *
 */
UCLASS()
class RPG_TU2_API UMyUserWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	// begin
	virtual void NativeConstruct() override;
	UFUNCTION(BlueprintCallable, Category = "Character",
			  meta = (DisplayName = "初始化"))
	void Init(Acharacter_my *character);
	UPROPERTY(
		BlueprintReadWrite, EditAnywhere, BlueprintReadWrite, Category = "Character",
		meta = (DisplayName = "角色"))
	TObjectPtr<Acharacter_my> Acharacter;
};
