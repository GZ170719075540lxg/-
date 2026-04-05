// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttachAnimNotify.h"
#include "Notify_attach_start.generated.h"

/**
 *
 */
UCLASS()
class RPG_TU2_API UNotify_attach_start : public UAttachAnimNotify
{
	GENERATED_BODY()
public:
	virtual bool Received_Notify(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *Animation, const FAnimNotifyEventReference &EventReference);
};
