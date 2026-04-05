// Fill out your copyright notice in the Description page of Project Settings.

#include "MyActorPreview_role.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
AMyActorPreview_role::AMyActorPreview_role()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	for (int32 i = 0; i < static_cast<int32>(ESock::MAX); ++i)
	{
		// 将整数转换为枚举类型
		ESock WeaponType = static_cast<ESock>(i);
		// 获取枚举名称
		FString EnumName = UEnum::GetValueAsString(WeaponType);
		// 创建静态网格组件
		UStaticMeshComponent *StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(*EnumName);
		// 添加静态网格组件到武器组件
		StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponComp.Add(WeaponType, StaticMeshComponent);
		// 无碰撞
	}
	Weapon = CreateDefaultSubobject<UMyWeapons>(TEXT("Weapon"));
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(RootComponent);
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AMyActorPreview_role::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMyActorPreview_role::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
void AMyActorPreview_role::EquipWeapon(UStaticMesh *WeaponMesh, FName SocketName, UStaticMeshComponent *temp)
{
	if (WeaponMesh)
	{
		temp->SetStaticMesh(WeaponMesh);
		// 挂载到插槽*
		temp->AttachToComponent(SkeletalMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
	}
}

void AMyActorPreview_role::EquipWeaponBytype()
{

	switch (Weapon->WeaponType)
	{
	case EWeaponType::taidao:
		ClearWeapon();
		EquipWeapon(Weapon->WeaponMesh1[0], "HandL2", WeaponComp[ESock::SockR]);
		EquipWeapon(Weapon->WeaponMesh1[1], "HandL", WeaponComp[ESock::SockL]);
		break;
		/* code */
		break;
	case EWeaponType::changjian:
		ClearWeapon();
		EquipWeapon(Weapon->WeaponMesh1[0], "HandR", WeaponComp[ESock::SockR]);
		/* code */
		break;
	case EWeaponType::gongjian:
		ClearWeapon();
		EquipWeapon(Weapon->WeaponMesh1[0], "HandL", WeaponComp[ESock::SockL]);
		break;
	}
}
/*
清理武器
*/
void AMyActorPreview_role::ClearWeapon()
{
	for (auto &temp : WeaponComp)
	{
		temp.Value->SetStaticMesh(nullptr);
	}
}