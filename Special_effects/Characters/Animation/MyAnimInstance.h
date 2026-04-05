#pragma once

#include "CoreMinimal.h"
class UAnimInstance;
#include "MyAnimInstance.generated.h"

/**
 *
 */
UCLASS()
class RPG_TU2_API UMyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UMyAnimInstance();
	// 尝试获取速度
	void UpdateSpeed();
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:
	// �ٶ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyAnimInstance")
	float Speed;
	// 角色速度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyAnimInstance",
			  meta = (DisplayName = "角色速度"))
	FVector Velocity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyAnimInstance",
			  meta = (DisplayName = "下落状态"))
	bool isfalling;

	virtual void NativeBeginPlay();
};
