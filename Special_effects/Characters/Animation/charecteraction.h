// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyAnimInstance.h"
#include "GameFramework/Pawn.h"
#include "charecteraction.generated.h"

/**
 * 角色动画实例 - 处理攀爬/飞行状态动画逻辑
 */
UCLASS()
class RPG_TU2_API Ucharecteraction : public UMyAnimInstance
{
	GENERATED_BODY()

public:
	Ucharecteraction();

	// 重写动画更新函数
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:
	// ==================== 角色引用 ====================

	/** 使用该动画的角色 */
	UPROPERTY(BlueprintReadOnly, Category = "Reference")
	TObjectPtr<class AOperation_character> CharacterOwner;
	// ==================== 攀爬/飞行速度 ====================

	/** 水平攀爬/飞行速度 */
	UPROPERTY(BlueprintReadOnly, Category = "Movement|Climb")
	float ClimbSpeed;

	/** 垂直攀爬/飞行速度 */
	UPROPERTY(BlueprintReadOnly, Category = "Movement|Climb")
	float ClimbVSpeed;

	// ==================== 动画资源 ====================

	/** 站立动画 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Sequences",
			  meta = (DisplayName = "站立动画"))
	TObjectPtr<class UAnimSequence> IdleAnimation;

	/** 起跳动画 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Sequences",
			  meta = (DisplayName = "起跳动画"))
	TObjectPtr<class UAnimSequence> JumpStartAnimation;

	/** 下落动画 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Sequences",
			  meta = (DisplayName = "下落动画"))
	TObjectPtr<class UAnimSequence> FallingAnimation;

	/** 落地动画 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Sequences",
			  meta = (DisplayName = "落地动画"))
	TObjectPtr<class UAnimSequence> LandAnimation;

	/** 攀爬动画（混合空间） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|BlendSpaces",
			  meta = (DisplayName = "攀爬动画"))
	TObjectPtr<class UBlendSpace> ClimbBlendSpace;

	/** 站立行走混合空间（1D） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|BlendSpaces",
			  meta = (DisplayName = "站立行走混合空间"))
	TObjectPtr<class UBlendSpace1D> WalkBlendSpace1D;

	/** 蹲伏动画（混合空间1D） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|BlendSpaces",
			  meta = (DisplayName = "蹲伏动画"))
	TObjectPtr<class UBlendSpace1D> CrouchBlendSpace1D;

	/** 运动停止动画 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Sequences",
			  meta = (DisplayName = "运动停止动画"))
	TObjectPtr<class UAnimSequence> StopAnimation;

	// ==================== 动画通知 ====================

	// 蒙太奇通知
	UFUNCTION()
	void AnimNotify_SwitchClimb();
};
