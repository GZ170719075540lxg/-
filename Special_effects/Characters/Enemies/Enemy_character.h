// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Player/character_my.h"
#include "../../Interface/Widgets/MyUserWidget.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "Enemy_character.generated.h"

/**
 *
 */
UCLASS()
class RPG_TU2_API AEnemy_character : public Acharacter_my
{
	GENERATED_BODY()
public:
	AEnemy_character();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	/*
	血条控件
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI",
			  meta = (DisplayName = "血条控件fu"))
	TObjectPtr<UWidgetComponent> HealthBar;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI",
			  meta = (DisplayName = "血条控件zi"))
	UMyUserWidget *HealthBarWidget;
	/*
	球体碰撞组件
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision",
			  meta = (DisplayName = "球体碰撞组件"))
	TObjectPtr<USphereComponent> SphereComponent;

	/*
	 */
public:
	// 重叠开始回调函数
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent *OverlappedComp, AActor *OtherActor,
						UPrimitiveComponent *OtherComp, int32 OtherBodyIndex,
						bool bFromSweep, const FHitResult &SweepResult);

	// 重叠结束回调函数
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent *OverlappedComp, AActor *OtherActor,
					  UPrimitiveComponent *OtherComp, int32 OtherBodyIndex);
};
