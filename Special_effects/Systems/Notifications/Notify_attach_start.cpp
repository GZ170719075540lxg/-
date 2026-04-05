// Fill out your copyright notice in the Description page of Project Settings.

#include "Notify_attach_start.h"

bool UNotify_attach_start::Received_Notify(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *Animation, const FAnimNotifyEventReference &EventReference)
{
    UE_LOG(LogTemp, Warning, TEXT("Animation Duration"));
    return true;
}