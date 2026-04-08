// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h" // ✅ 正确路径
#include "Camera/CameraComponent.h"
#include "../../Core/Base/Status.h"
#include "../../Core/Base/WeaponType.h"
#include "../../Core/Base/Sock.h"
#include "../../Combat/Weapons/MyWeapons.h"
#include "../../Combat/Items/MyItemObjectorThrow.h"
#include "../../Interface/Widgets/DamageNumberActor.h"
#include "../../Interface/Widgets/SimpleDamageTextActor.h"

#include "character_my.generated.h"

/**
 *
 */
UCLASS()
class RPG_TU2_API Acharacter_my : public ACharacter
{
	GENERATED_BODY()
public:
	// 修改角色名字
	void set_character_name(FString name);
	Acharacter_my();

public:
	// 角色命名
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString character_name;

	/*
		基础属性
		*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|属性",
			  meta = (DisplayName = "等级"))
	int32 Level;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|属性",
			  meta = (DisplayName = "最大经验"))
	int32 MaxExp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|属性",
			  meta = (DisplayName = "当前经验"))
	int32 NowExp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|属性",
			  meta = (DisplayName = "生命值"))
	int32 Health;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|属性",
			  meta = (DisplayName = "当前生命值"))
	int32 NowHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|属性",
			  meta = (DisplayName = "血量百分百"))
	float HealthPercent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|属性",
			  meta = (DisplayName = "体力值"))
	int32 Stamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|属性",
			  meta = (DisplayName = "当前体力值"))
	int32 NowStamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|属性",
			  meta = (DisplayName = "当前体力值"))
	float StaminaPercent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|属性",
			  meta = (DisplayName = "攻击力"))
	int32 Attack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|属性",
			  meta = (DisplayName = "防御力"))
	int32 Defense;
	// 韧性
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|属性",
			  meta = (DisplayName = "韧性"))
	int32 Tolerance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|属性",
			  meta = (DisplayName = "最大韧性"))
	int32 ToleranceMax;

	/*
	状态
	*/
public:
	UPROPERTY(AdvancedDisplay, BlueprintReadOnly, Category = "Character|状态",
			  meta = (DisplayName = "是否可攻击"))
	bool CanAttack;
	UPROPERTY(AdvancedDisplay, BlueprintReadOnly, Category = "Character|状态",
			  meta = (DisplayName = "是否可技能"))
	TArray<bool> CanSkill;
	// 角色的状态
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|状态",
			  meta = (DisplayName = "角色状态"))
	EStatus Character_Status;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|冲刺",
			  meta = (DisplayName = "冲刺状态"))
	bool issprint;
	/*
	定时器
	*/
	FTimerHandle TimerSprintUP;
	FTimerHandle TimerSprintDown;
	FTimerHandle TimerAttack;
	// 重置攻击
	FTimerHandle TimerAttackReset;
	FTimerHandle TimerSkill;
	// 攻击追踪定时器
	FTimerHandle TimerAttackTrack;
	// 僵持计时器
	FTimerHandle TimerStalemate;

	/*
	攻击
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|攻击",
			  meta = (DisplayName = "攻击蒙太奇"))
	TObjectPtr<UAnimMontage> AttackMontage;
	// 当前连招数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|攻击",
			  meta = (DisplayName = "当前连招数"))
	int32 AttackNum;
	// 最大连招数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|攻击",
			  meta = (DisplayName = "最大连招数"))
	int32 AttackMaxNum;
	/*
	攻击CD
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|攻击",
			  meta = (DisplayName = "攻击CD"))
	TArray<float> AttackCD;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|攻击",
			  meta = (DisplayName = "最晚攻击时间"))
	TArray<float> AttackCDNow;

	/*
	蒙太奇
	*/
public:
	/*
	攻击蒙太奇
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|攻击",
			  meta = (DisplayName = "普通攻击蒙太奇"))
	TArray<TObjectPtr<UAnimMontage>> AttackMontageList;
	/*
	受击蒙太奇
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|攻击",
			  meta = (DisplayName = "受击蒙太奇"))
	TObjectPtr<UAnimMontage> HitMontage;

	/*
	已被攻击对象
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|攻击",
			  meta = (DisplayName = "被攻击对象"))
	TSet<TObjectPtr<Acharacter_my>> AttackTarget;

	/*
	技能蒙太奇
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|技能",
			  meta = (DisplayName = "技能蒙太奇"))
	TArray<TObjectPtr<UAnimMontage>> SkillMontageList;
	/*
	技能CD
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|技能",
			  meta = (DisplayName = "技能CD"))
	TArray<float> SkillCD;

	/*
	武器
	*/
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "攻击",
			  meta = (DisplayName = "攻击追踪点"))
	TArray<FVector> AttackTrackPoint; // 攻击追踪点
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon",
			  meta = (DisplayName = "武器位置"))
	TMap<ESock, TObjectPtr<UStaticMeshComponent>> WeaponComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "攻击",
			  meta = (DisplayName = "武器"))
	TObjectPtr<UMyWeapons> Weapon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "攻击",
			  meta = (DisplayName = "武器骨骼网格体"))
	TObjectPtr<USkeletalMeshComponent> WeaponMeshs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "组件",
			  meta = (DisplayName = "弹簧臂"))
	TObjectPtr<USpringArmComponent> SpringArm;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "组件",
			  meta = (DisplayName = "摄像机"))
	TObjectPtr<UCameraComponent> Camera;

public:
	/*
	更新血量
	*/
	UFUNCTION(BlueprintCallable, Category = "Character|属性",

			  meta = (DisplayName = "更新血量"))
	bool UpdateHealth(int32 value);
	/*
	更新体力
	*/
	UFUNCTION(BlueprintCallable, Category = "Character|属性",
			  meta = (DisplayName = "更新体力"))
	bool UpdateStamina(int32 value);
	/*
更新攻击
*/
	UFUNCTION(BlueprintCallable, Category = "Character|属性",
			  meta = (DisplayName = "更新攻击"))
	void UpdateAttack(int32 value);
	/*
	更新防御
	*/
	UFUNCTION(BlueprintCallable, Category = "Character|属性",
			  meta = (DisplayName = "更新防御"))
	void UpdateDefense(int32 value);
	/*
	清零攻击数
	*/
	UFUNCTION(BlueprintCallable, Category = "Character|攻击",
			  meta = (DisplayName = "清零攻击数"))
	void ClearAttackNum();
	/*
	更新攻击数
	*/
	UFUNCTION(BlueprintCallable, Category = "Character|攻击",
			  meta = (DisplayName = "更新攻击数"))
	void UpdateAttackNum();

	/*
	普通攻击
	*/
	UFUNCTION(BlueprintCallable, Category = "Character|攻击",
			  meta = (DisplayName = "普通攻击"))
	void AttackNormal();
	/*
	技能攻击
	*/
	UFUNCTION(BlueprintCallable, Category = "Character|技能",
			  meta = (DisplayName = "技能攻击"))
	void AttackSkill(int32 SkillNum);

	/*
	角色受击函数
	* @package value
	@package AttackDir 攻击方向
	*/
	UFUNCTION(BlueprintCallable, Category = "Character|属性",
			  meta = (DisplayName = "角色受击"))
	void CharacterHit(int32 value, FVector AttackDir);

	/*
	显示伤害数字
	* @package Damage 伤害值
	* @package Location 显示位置
	*/
	UFUNCTION(BlueprintCallable, Category = "Character|UI",
			  meta = (DisplayName = "显示伤害数字"))
	void ShowDamageNumber(int32 Damage, FVector Location);
	/*
	攻击的追踪
	*/
	UFUNCTION(BlueprintCallable, Category = "Character|攻击",
			  meta = (DisplayName = "攻击的追踪定时器"))
	void AttackTrackTimer();
	// 追踪停止
	UFUNCTION(BlueprintCallable, Category = "Character|攻击",
			  meta = (DisplayName = "追踪停止"))
	void AttackTrackStop();
	UFUNCTION(BlueprintCallable, Category = "Character|攻击",
			  meta = (DisplayName = "攻击追踪"))
	void AttackTrack();
	// 进入僵持
	UFUNCTION(BlueprintCallable, Category = "Character|状态",
			  meta = (DisplayName = "进入僵持"))
	void Stalemate();

	/*
	武器
	*/
	// 装备武器
	UFUNCTION(BlueprintCallable, Category = "Character|武器",
			  meta = (DisplayName = "装备武器"))
	void EquipWeapon(UStaticMesh *WeaponMesh, FName SocketName, UStaticMeshComponent *temp);
	/*
	// 获取离自己最近对象位置
	// */
	// UFUNCTION(BlueprintCallable, Category = "Character|攻击",
	// 		  meta = (DisplayName = "获取离自己最近对象位置"))
	// FVector GetNearestTargetLocation();

	/*
	装备不同类型武器
	*/
	UFUNCTION(BlueprintCallable, Category = "Character|武器",
			  meta = (DisplayName = "装备不同类型武器"))
	void EquipWeaponBytype();
	/*
	清理武器
	*/
	UFUNCTION(BlueprintCallable, Category = "Character|武器",
			  meta = (DisplayName = "清理武器"))
	void ClearWeapon();

	/*
	武器旋转
	*/

	UFUNCTION(BlueprintCallable, Category = "Character|武器",
			  meta = (DisplayName = "武器旋转"))
	void WeaponRotate(ESock Sock, FRotator Rotator, FVector Location);
};
