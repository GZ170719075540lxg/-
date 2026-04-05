#pragma once

#include "CoreMinimal.h"
#include "character_my.h"
#include "GameFramework/SpringArmComponent.h" // ✅ 正确路径
#include "Camera/CameraComponent.h"
#include "MotionWarpingComponent.h"
#include "Components/StaticMeshComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../../Core/Base/BackPack.h"
#include "../../Combat/Items/Item.h"

#include "Components/TimelineComponent.h"
#include "Operation_character.generated.h"

class UInputMappingContext;
class UInputAction;

USTRUCT(BlueprintType)
struct FBackpackSlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TObjectPtr<UItem>> Items;
};

USTRUCT(BlueprintType)
struct Fisjump
{
	GENERATED_USTRUCT_BODY()
	// 是否可翻越
	bool isclimb;
	// 翻越点1
	FVector point1;
	// 翻越点2
	FVector point2;
};

UCLASS()
class RPG_TU2_API AOperation_character : public Acharacter_my
{
	GENERATED_BODY()
	// beginplay
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;

protected:
	// 输出操作
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> IMC_Default;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Move;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Look;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Jump;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Ctrl;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> IA_Sprint;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> IA_Attack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> IA_Pick;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> IA_Backpack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input") // 角色详情
	TObjectPtr<UInputAction> IA_CharacterDetail;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input") // 蓄力攻击
	TObjectPtr<UInputAction> IA_Attacks;

public:
	AOperation_character();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|蒙太奇",
			  meta = (DisplayName = "是否在跳跃"))
	TObjectPtr<UAnimMontage> ClimbMontage;
	// Motion Warping Component - 必须声明这个变量
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (DisplayName = "动画扭曲组件"))
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|属性",
			  meta = (DisplayName = "左翻越蒙太奇"))
	TObjectPtr<UAnimMontage> ClimbRightMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|属性",
			  meta = (DisplayName = "右翻越蒙太奇"))
	TObjectPtr<UAnimMontage> ClimbLeftMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|属性",
			  meta = (DisplayName = "左翻越蒙太奇2"))
	TObjectPtr<UAnimMontage> ClimbRightMontage2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|属性",
			  meta = (DisplayName = "右翻越蒙太奇2"))
	TObjectPtr<UAnimMontage> ClimbLeftMontage2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|属性",
			  meta = (DisplayName = "攀爬跳蒙太奇"))
	TObjectPtr<UAnimMontage> ClimbJumpMontage;
	// 拉弓蒙太奇
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|属性",
			  meta = (DisplayName = "拉弓蒙太奇"))
	TObjectPtr<UAnimMontage> DrawBowMontage;
	// 冲刺蒙太奇
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|属性",
			  meta = (DisplayName = "冲刺蒙太奇"))
	TObjectPtr<UAnimMontage> SprintMontage;

	/*状态*/
	// 攀爬状态
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|攀爬",
			  meta = (DisplayName = "是否在攀爬"))
	bool isclimb;
	// 水平攀爬速度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|攀爬",
			  meta = (DisplayName = "水平攀爬速度"))
	float climbspeed;
	// 垂直攀爬速度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|攀爬",
			  meta = (DisplayName = "垂直攀爬速度"))
	float climbvspeed;
	// 正在播放蒙太奇
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|蒙太奇",
			  meta = (DisplayName = "是否正在播放蒙太奇"))
	bool ismontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|蹲伏",
			  meta = (DisplayName = "蹲伏状态"))
	bool isdunfu;

public:
	/*
	武器
	*/
	/*
	背包
	*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|背包",
			  meta = (DisplayName = "背包"))
	TMap<EBackPack, FBackpackSlot> Backpack;

public:
	/*
	 *初始化变量
	 */
	UFUNCTION(BlueprintCallable, Category = "Character|属性",
			  meta = (DisplayName = "初始化变量"))
	void InitVariableWithParams(
		int32 InHealth,
		int32 InNowHealth,
		float InHealthPercent,
		int32 InStamina,
		int32 InNowStamina,
		float InStaminaPercent,
		int32 InAttack,
		int32 InDefense);
	/*
	 *初始化按键
	 */
	UFUNCTION(BlueprintCallable, Category = "Character|按键")
	void InitInput();
	/*
	 *按键事件
	 */
	UFUNCTION(BlueprintCallable, Category = "Character|移动")
	void Move(const FInputActionValue &Value);
	UFUNCTION(BlueprintCallable, Category = "Character|旋转")
	void Look(const FInputActionValue &Value);
	UFUNCTION(BlueprintCallable, Category = "Character|跳跃")
	void Jumps(const FInputActionValue &Value);
	UFUNCTION(BlueprintCallable, Category = "Character|控制")
	void Ctrl(const FInputActionValue &Value);
	UFUNCTION(BlueprintCallable, Category = "Character|冲刺")
	void Sprint(const FInputActionValue &Value);
	UFUNCTION(BlueprintCallable, Category = "Character|松开移动")
	void LookAxis(const FInputActionValue &Value);
	// 捡取
	UFUNCTION(BlueprintCallable, Category = "Character|捡取")
	void Pick(const FInputActionValue &Value);
	// 背包
	UFUNCTION(BlueprintCallable, Category = "Character|背包")
	void Backpacks(const FInputActionValue &Value);
	// 角色详情
	UFUNCTION(BlueprintCallable, Category = "Character|角色详情")
	void CharacterDetails(const FInputActionValue &Value);
	UFUNCTION(BlueprintCallable, Category = "Character|攻击")
	void Attacks(const FInputActionValue &Value);

public:
	/*
	攀爬判断
	*/
public:
	// 切换攀爬状态
	UFUNCTION(BlueprintCallable, Category = "Character|攀爬",
			  meta = (DisplayName = "切换攀爬状态"))
	void SwitchClimb();
	UFUNCTION(BlueprintCallable, Category = "Character|攀爬",
			  meta = (DisplayName = "是否进入攀爬状态"))
	bool ComeinClimb();
	UFUNCTION(BlueprintCallable, Category = "Character|攀爬",
			  meta = (DisplayName = "下一刻位置判定"))
	bool NextPosition(FVector NextLocation);

	/**
	蒙太奇回调函数
	*/
	UFUNCTION()
	void OnMontageEnded(UAnimMontage *Montage, bool bInterrupted);

	/*
	初始化背包
	*/
public:
	UFUNCTION(BlueprintCallable, Category = "Character|背包",
			  meta = (DisplayName = "初始化背包"))
	void InitBackPack();
	/*
	为背包添加物品
	*/
	UFUNCTION(BlueprintCallable, Category = "Character|背包",
			  meta = (DisplayName = "为背包添加物品"))
	void AddItemToBackPack(UItem *Item);

	/*
	use item
	*/
	UFUNCTION(BlueprintCallable, Category = "Character|背包",
			  meta = (DisplayName = "使用物品"))
	bool UseItem(int32 index);
};