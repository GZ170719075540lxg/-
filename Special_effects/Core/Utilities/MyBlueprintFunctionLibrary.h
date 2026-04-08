// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/TextureRenderTarget2D.h"
#include "ImageUtils.h"
#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Modules/ModuleManager.h"
#include "MyBlueprintFunctionLibrary.generated.h"

/**
 *
 */
USTRUCT(BlueprintType)
struct FMyVectorPair
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FVector A;

	UPROPERTY(BlueprintReadWrite)
	FVector B;
};

/**
 * 机械臂关节信息结构体
 */
USTRUCT(BlueprintType)
struct FBPFunctionLibraryRoboticArmJoint
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString JointName;

	UPROPERTY(BlueprintReadWrite)
	float Angle;

	UPROPERTY(BlueprintReadWrite)
	float MinAngle;

	UPROPERTY(BlueprintReadWrite)
	float MaxAngle;

	FBPFunctionLibraryRoboticArmJoint()
		: JointName(TEXT("")), Angle(0.0f), MinAngle(-90.0f), MaxAngle(90.0f)
	{
	}
};

/**
 * 机械臂控制命令枚举
 */
UENUM(BlueprintType)
enum class EBPFunctionLibraryRoboticArmCommand : uint8
{
	MoveToPosition UMETA(DisplayName = "移动到指定位置"),
	RotateJoint UMETA(DisplayName = "旋转关节"),
	GrabObject UMETA(DisplayName = "抓取物体"),
	ReleaseObject UMETA(DisplayName = "释放物体"),
	HomePosition UMETA(DisplayName = "回到初始位置"),
	Stop UMETA(DisplayName = "停止运动")
};
UCLASS()
class RPG_TU2_API UMyBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// 获取一个向量的垂直向量
	UFUNCTION(BlueprintPure, Category = "MyBlueprintFunctionLibrary")
	static FMyVectorPair GetPerpendicularVector(FVector Vector, FVector test = FVector(0, 0, 0));

	// 将渲染目标2D转换为PNG格式的二进制数据
	UFUNCTION(BlueprintCallable, Category = "Image Conversion")
	static TArray<uint8> ConvertRenderTargetToPNG(UTextureRenderTarget2D *RenderTarget);

	// 将渲染目标2D转换为JPEG格式的二进制数据
	UFUNCTION(BlueprintCallable, Category = "Image Conversion")
	static TArray<uint8> ConvertRenderTargetToJPEG(UTextureRenderTarget2D *RenderTarget, int32 Quality = 85);

	// 将渲染目标2D保存为文件
	UFUNCTION(BlueprintCallable, Category = "Image Conversion")
	static bool SaveRenderTargetToFile(UTextureRenderTarget2D *RenderTarget, const FString &FilePath, bool bSaveAsPNG = true);

	// 获取渲染目标的原始像素数据
	UFUNCTION(BlueprintCallable, Category = "Image Conversion")
	static TArray<FColor> GetRenderTargetPixels(UTextureRenderTarget2D *RenderTarget);

	// ==================== 机械臂控制功能 ====================

	/**
	 * 控制机械臂执行指定操作
	 * @param RoboticArmActor 机械臂Actor对象
	 * @param CommandString 操作命令字符串
	 * @return 操作是否成功执行
	 */
	UFUNCTION(BlueprintCallable, Category = "Robotic Arm Control")
	static bool ControlRoboticArm(AActor *RoboticArmActor, const FString &CommandString);

	/**
	 * 解析命令字符串并执行相应操作
	 * @param RoboticArmActor 机械臂Actor对象
	 * @param Command 命令类型
	 * @param Parameters 命令参数
	 * @return 操作是否成功
	 */
	static bool ExecuteRoboticArmCommand(AActor *RoboticArmActor, EBPFunctionLibraryRoboticArmCommand Command, const TArray<FString> &Parameters);

	/**
	 * 移动机械臂到指定位置
	 * @param RoboticArmActor 机械臂Actor
	 * @param TargetPosition 目标位置
	 * @return 是否成功
	 */
	UFUNCTION(BlueprintCallable, Category = "Robotic Arm Control")
	static bool MoveRoboticArmToPosition(AActor *RoboticArmActor, FVector TargetPosition);

	/**
	 * 旋转机械臂关节
	 * @param RoboticArmActor 机械臂Actor
	 * @param JointName 关节名称
	 * @param Angle 角度
	 * @return 是否成功
	 */
	UFUNCTION(BlueprintCallable, Category = "Robotic Arm Control")
	static bool RotateRoboticArmJoint(AActor *RoboticArmActor, const FString &JointName, float Angle);

	/**
	 * 控制机械臂抓取物体
	 * @param RoboticArmActor 机械臂Actor
	 * @param ObjectToGrab 要抓取的物体
	 * @return 是否成功
	 */
	UFUNCTION(BlueprintCallable, Category = "Robotic Arm Control")
	static bool GrabObjectWithRoboticArm(AActor *RoboticArmActor, AActor *ObjectToGrab);

	/**
	 * 控制机械臂释放物体
	 * @param RoboticArmActor 机械臂Actor
	 * @return 是否成功
	 */
	UFUNCTION(BlueprintCallable, Category = "Robotic Arm Control")
	static bool ReleaseObjectFromRoboticArm(AActor *RoboticArmActor);

	/**
	 * 将机械臂移动到初始位置
	 * @param RoboticArmActor 机械臂Actor
	 * @return 是否成功
	 */
	UFUNCTION(BlueprintCallable, Category = "Robotic Arm Control")
	static bool MoveRoboticArmToHomePosition(AActor *RoboticArmActor);

	/**
	 * 停止机械臂运动
	 * @param RoboticArmActor 机械臂Actor
	 * @return 是否成功
	 */
	UFUNCTION(BlueprintCallable, Category = "Robotic Arm Control")
	static bool StopRoboticArmMovement(AActor *RoboticArmActor);

	/**
	 * 获取机械臂当前关节角度信息
	 * @param RoboticArmActor 机械臂Actor
	 * @return 关节信息数组
	 */
	UFUNCTION(BlueprintCallable, Category = "Robotic Arm Control")
	static TArray<FBPFunctionLibraryRoboticArmJoint> GetRoboticArmJointInfo(AActor *RoboticArmActor);

	/**
	 * 设置机械臂关节角度
	 * @param RoboticArmActor 机械臂Actor
	 * @param JointAngles 关节角度映射
	 * @return 是否成功
	 */
	UFUNCTION(BlueprintCallable, Category = "Robotic Arm Control")
	static bool SetRoboticArmJointAngles(AActor *RoboticArmActor, const TMap<FString, float> &JointAngles);

private:
	/**
	 * 验证机械臂Actor的有效性
	 * @param RoboticArmActor 要验证的Actor
	 * @return 是否有效
	 */
	static bool ValidateRoboticArmActor(AActor *RoboticArmActor);

	/**
	 * 解析命令字符串
	 * @param CommandString 命令字符串
	 * @param OutCommand 输出命令类型
	 * @param OutParameters 输出参数数组
	 * @return 是否解析成功
	 */
	static bool ParseCommandString(const FString &CommandString, EBPFunctionLibraryRoboticArmCommand &OutCommand, TArray<FString> &OutParameters);

	/**
	 * 获取命令对应的枚举值
	 * @param CommandString 命令字符串
	 * @return 对应的命令枚举
	 */
	static EBPFunctionLibraryRoboticArmCommand GetCommandFromString(const FString &CommandString);
};
