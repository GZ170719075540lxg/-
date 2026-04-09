// Fill out your copyright notice in the Description page of Project Settings.

#include "charecteraction.h"
#include "../Player/Operation_character.h"
#include "../Player/ClimbingSystem.h"
#include "GameFramework/CharacterMovementComponent.h"

Ucharecteraction::Ucharecteraction()
{
    ClimbSpeed = 0.0f;
    ClimbVSpeed = 0.0f;
    ClimbForwardSpeed = 0.0f;
    ClimbRightSpeed = 0.0f;
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

    // 判断是否处于飞行状态（攀爬时使用飞行模式）
    if (MoveComp->IsFlying())
    {
        // 非攀爬的飞行状态，使用传统方式计算
        FVector CurrentVelocity = MoveComp->Velocity;
        FVector ActorForward = CharacterOwner->GetActorForwardVector();
        FVector ActorRight = CharacterOwner->GetActorRightVector();
        FVector ActorUp = CharacterOwner->GetActorUpVector();

        // 垂直速度：速度在角色上方方向的投影
        ClimbVSpeed = FVector::DotProduct(CurrentVelocity, ActorUp);

        // 水平速度分量：保留方向信息
        ClimbForwardSpeed = FVector::DotProduct(CurrentVelocity, ActorForward);
        ClimbRightSpeed = FVector::DotProduct(CurrentVelocity, ActorRight);

        // 水平速度大小：用于混合空间等需要标量的场景
        ClimbSpeed = FMath::Sqrt(ClimbForwardSpeed * ClimbForwardSpeed + ClimbRightSpeed * ClimbRightSpeed);
    }
    else
    {
        // 非飞行状态时重置攀爬速度
        ClimbSpeed = 0.0f;
        ClimbVSpeed = 0.0f;
        ClimbForwardSpeed = 0.0f;
        ClimbRightSpeed = 0.0f;
    }
}

void Ucharecteraction::AnimNotify_SwitchClimb()
{
    UE_LOG(LogTemp, Warning, TEXT("ClimbStart Notify"));
}