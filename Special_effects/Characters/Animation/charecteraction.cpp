// Fill out your copyright notice in the Description page of Project Settings.

#include "charecteraction.h"
#include "../Player/Operation_character.h"
#include "GameFramework/CharacterMovementComponent.h"

Ucharecteraction::Ucharecteraction()
{
    ClimbSpeed = 0.0f;
    ClimbVSpeed = 0.0f;
    CharacterOwner = nullptr;
}

void Ucharecteraction::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    // 如果角色引用为空，尝试获取
    if (!CharacterOwner)
    {
        APawn *Pawn = TryGetPawnOwner();
        if (Pawn)
        {
            CharacterOwner = Cast<AOperation_character>(Pawn);
        }
    }

    if (!CharacterOwner)
        return;

    UCharacterMovementComponent *MoveComp = CharacterOwner->GetCharacterMovement();
    if (!MoveComp)
        return;

    // 判断是否处于飞行状态
    if (MoveComp->IsFlying())
    {
        FVector CurrentVelocity = CharacterOwner->GetVelocity();
        FVector ActorForward = CharacterOwner->GetActorForwardVector();
        FVector ActorRight = CharacterOwner->GetActorRightVector();
        FVector ActorUp = CharacterOwner->GetActorUpVector();

        // 垂直速度：速度在角色上方方向的投影
        ClimbVSpeed = FVector::DotProduct(CurrentVelocity, ActorUp);

        // 水平速度：速度在角色前方和右方平面的投影大小
        float FwdSpeed = FVector::DotProduct(CurrentVelocity, ActorForward);
        float RightSpeed = FVector::DotProduct(CurrentVelocity, ActorRight);
        ClimbSpeed = FMath::Sqrt(FwdSpeed * FwdSpeed + RightSpeed * RightSpeed);
    }
    else
    {
        // 非飞行状态时重置攀爬速度
        ClimbSpeed = 0.0f;
        ClimbVSpeed = 0.0f;
    }
}

void Ucharecteraction::AnimNotify_SwitchClimb()
{
    UE_LOG(LogTemp, Warning, TEXT("ClimbStart Notify"));
}