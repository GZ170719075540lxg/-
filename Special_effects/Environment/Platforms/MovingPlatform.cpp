// Fill out your copyright notice in the Description page of Project Settings.

#include "MovingPlatform.h"
#include "Engine/Engine.h" // 添加日志支持

// Sets default values
/**
 * AMovingPlatform构造函数
 * 初始化移动平台的基本设置
 */
AMovingPlatform::AMovingPlatform()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMovingPlatform::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT(" ---------------------------------------------------------------------------------------------"));
}

// Called every frame
/**
 * 平台类的Tick函数，用于每帧更新平台状态
 * @param DeltaTime 距离上一帧的时间间隔（秒），用于计算平滑的动画效果
 */
void AMovingPlatform::Tick(float DeltaTime)
{
	// 调用父类的Tick函数，保持引擎的正常更新流程
	Super::Tick(DeltaTime);
	UE_LOG(LogTemp, Log, TEXT(" ---------------------------------------------------------------------------------------------"));
}
