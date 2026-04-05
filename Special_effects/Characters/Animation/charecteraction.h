// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyAnimInstance.h"
#include "charecteraction.generated.h"

/**
 *
 */
UCLASS()
class RPG_TU2_API Ucharecteraction : public UMyAnimInstance
{
	GENERATED_BODY()

public:
	// 蒙太奇通知
	UFUNCTION()
	void AnimNotify_SwitchClimb();
};
