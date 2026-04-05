// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyAnimInstance.h"
#include "../Player/character_my.h"
#include "EnemyAnimInstance.generated.h"

/**
 *
 */
UCLASS()
class RPG_TU2_API UEnemyAnimInstance : public UMyAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeBeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "角色",
			  meta = (DisplayName = "角色"))
	TObjectPtr<Acharacter_my> Character;
};
