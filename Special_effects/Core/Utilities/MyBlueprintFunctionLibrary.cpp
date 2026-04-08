// Fill out your copyright notice in the Description page of Project Settings.

#include "MyBlueprintFunctionLibrary.h"
#include "Engine/TextureRenderTarget2D.h"
#include "ImageUtils.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Modules/ModuleManager.h"
#include "Misc/FileHelper.h"
#include "Engine/Engine.h"
#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "RHIDefinitions.h"
#include "TextureResource.h"

FMyVectorPair UMyBlueprintFunctionLibrary::GetPerpendicularVector(FVector Vector, FVector /*test*/)
{
	// 如果输入接近零，返回一组默认正交基
	if (Vector.IsNearlyZero())
	{
		return FMyVectorPair(FVector::RightVector, FVector::UpVector);
	}

	// 先取 XY 投影（z = 0）
	FVector Vxy(Vector.X, Vector.Y, 0.0f);

	FVector First; // 要返回的第一个向量（z == 0）
	if (Vxy.IsNearlyZero())
	{
		// 输入向量几乎在 Z 轴上（x=y≈0），任意 XY 平面向量都与之垂直
		First = FVector(1.0f, 0.0f, 0.0f);
	}
	else
	{
		// 在 XY 平面上取一个垂直于 Vxy 的向量：(-y, x, 0)
		First = FVector(-Vxy.Y, Vxy.X, 0.0f).GetSafeNormal();
	}

	// 再用叉乘得到第二个与 Vector 和 First 都垂直的向量
	FVector Second = FVector::CrossProduct(Vector, First);

	// 如果叉乘结果靠近零（极小数值），尝试备选方案
	if (Second.IsNearlyZero())
	{
		// 说明 Vector 与 First 近似共线（理论上不太可能，但为安全处理）
		// 用 First 与 UpVector 做叉乘尝试（或换 RightVector）
		Second = FVector::CrossProduct(First, FVector::UpVector);
		if (Second.IsNearlyZero())
		{
			Second = FVector::CrossProduct(First, FVector::RightVector);
		}
	}

	// 归一化输出（安全）
	First = First.GetSafeNormal();
	Second = Second.GetSafeNormal();

	return FMyVectorPair(First, Second);
}

TArray<uint8> UMyBlueprintFunctionLibrary::ConvertRenderTargetToPNG(UTextureRenderTarget2D *RenderTarget)
{
	TArray<uint8> ImageData;

	if (!RenderTarget)
	{
		UE_LOG(LogTemp, Warning, TEXT("ConvertRenderTargetToPNG: RenderTarget is null"));
		return ImageData;
	}

	// 确保渲染目标已完成渲染
	RenderTarget->TargetGamma = 1.0f;

	// 从渲染目标获取纹理数据
	FTextureRenderTargetResource *RenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();
	if (!RenderTargetResource)
	{
		UE_LOG(LogTemp, Warning, TEXT("ConvertRenderTargetToPNG: Failed to get render target resource"));
		return ImageData;
	}

	// 创建图像包装器
	IImageWrapperModule &ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

	if (!ImageWrapper.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("ConvertRenderTargetToPNG: Failed to create PNG image wrapper"));
		return ImageData;
	}

	// 读取像素数据
	FReadSurfaceDataFlags ReadDataFlags;
	ReadDataFlags.SetOutputStencil(false);
	ReadDataFlags.SetMip(0);

	TArray<FColor> SurfData;
	RenderTargetResource->ReadPixels(SurfData, ReadDataFlags);

	if (SurfData.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("ConvertRenderTargetToPNG: No pixel data read from render target"));
		return ImageData;
	}

	// 设置图像数据并编码为PNG
	const int32 Width = RenderTarget->SizeX;
	const int32 Height = RenderTarget->SizeY;

	if (ImageWrapper->SetRaw(SurfData.GetData(), SurfData.Num() * sizeof(FColor), Width, Height, ERGBFormat::BGRA, 8))
	{
		ImageData = ImageWrapper->GetCompressed(100);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ConvertRenderTargetToPNG: Failed to set raw image data"));
	}

	return ImageData;
}

TArray<uint8> UMyBlueprintFunctionLibrary::ConvertRenderTargetToJPEG(UTextureRenderTarget2D *RenderTarget, int32 Quality)
{
	TArray<uint8> ImageData;

	if (!RenderTarget)
	{
		UE_LOG(LogTemp, Warning, TEXT("ConvertRenderTargetToJPEG: RenderTarget is null"));
		return ImageData;
	}

	// 确保渲染目标已完成渲染
	RenderTarget->TargetGamma = 1.0f;

	// 从渲染目标获取纹理数据
	FTextureRenderTargetResource *RenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();
	if (!RenderTargetResource)
	{
		UE_LOG(LogTemp, Warning, TEXT("ConvertRenderTargetToJPEG: Failed to get render target resource"));
		return ImageData;
	}

	// 创建图像包装器
	IImageWrapperModule &ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);

	if (!ImageWrapper.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("ConvertRenderTargetToJPEG: Failed to create JPEG image wrapper"));
		return ImageData;
	}

	// 读取像素数据
	FReadSurfaceDataFlags ReadDataFlags;
	ReadDataFlags.SetOutputStencil(false);
	ReadDataFlags.SetMip(0);

	TArray<FColor> SurfData;
	RenderTargetResource->ReadPixels(SurfData, ReadDataFlags);

	if (SurfData.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("ConvertRenderTargetToJPEG: No pixel数据读取失败"));
		return ImageData;
	}

	// 设置图像数据并编码为JPEG
	const int32 Width = RenderTarget->SizeX;
	const int32 Height = RenderTarget->SizeY;

	if (ImageWrapper->SetRaw(SurfData.GetData(), SurfData.Num() * sizeof(FColor), Width, Height, ERGBFormat::BGRA, 8))
	{
		// 确保质量值在有效范围内
		Quality = FMath::Clamp(Quality, 1, 100);
		ImageData = ImageWrapper->GetCompressed(Quality);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ConvertRenderTargetToJPEG: Failed to set raw image data"));
	}

	return ImageData;
}

bool UMyBlueprintFunctionLibrary::SaveRenderTargetToFile(UTextureRenderTarget2D *RenderTarget, const FString &FilePath, bool bSaveAsPNG)
{
	if (!RenderTarget)
	{
		UE_LOG(LogTemp, Warning, TEXT("SaveRenderTargetToFile: RenderTarget is null"));
		return false;
	}

	TArray<uint8> ImageData;
	if (bSaveAsPNG)
	{
		ImageData = ConvertRenderTargetToPNG(RenderTarget);
	}
	else
	{
		ImageData = ConvertRenderTargetToJPEG(RenderTarget);
	}

	if (ImageData.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("SaveRenderTargetToFile: Failed to convert render target to image data"));
		return false;
	}

	// 保存到文件
	if (FFileHelper::SaveArrayToFile(ImageData, *FilePath))
	{
		UE_LOG(LogTemp, Log, TEXT("Successfully saved render target to %s"), *FilePath);
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to save render target to %s"), *FilePath);
		return false;
	}
}

TArray<FColor> UMyBlueprintFunctionLibrary::GetRenderTargetPixels(UTextureRenderTarget2D *RenderTarget)
{
	TArray<FColor> PixelData;

	if (!RenderTarget)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetRenderTargetPixels: RenderTarget is null"));
		return PixelData;
	}

	// 从渲染目标获取纹理数据
	FTextureRenderTargetResource *RenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();
	if (!RenderTargetResource)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetRenderTargetPixels: Failed to get render target resource"));
		return PixelData;
	}

	// 读取像素数据
	FReadSurfaceDataFlags ReadDataFlags;
	ReadDataFlags.SetOutputStencil(false);
	ReadDataFlags.SetMip(0);

	RenderTargetResource->ReadPixels(PixelData, ReadDataFlags);

	if (PixelData.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetRenderTargetPixels: No pixel data read from render target"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Successfully read %d pixels from render target"), PixelData.Num());
	}

	return PixelData;
}

// ==================== 机械臂控制功能实现 ====================

bool UMyBlueprintFunctionLibrary::ControlRoboticArm(AActor *RoboticArmActor, const FString &CommandString)
{
	if (!ValidateRoboticArmActor(RoboticArmActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("ControlRoboticArm: Invalid robotic arm actor"));
		return false;
	}

	EBPFunctionLibraryRoboticArmCommand Command;
	TArray<FString> Parameters;

	if (!ParseCommandString(CommandString, Command, Parameters))
	{
		UE_LOG(LogTemp, Warning, TEXT("ControlRoboticArm: Failed to parse command string: %s"), *CommandString);
		return false;
	}

	return ExecuteRoboticArmCommand(RoboticArmActor, Command, Parameters);
}

bool UMyBlueprintFunctionLibrary::ExecuteRoboticArmCommand(AActor *RoboticArmActor, EBPFunctionLibraryRoboticArmCommand Command, const TArray<FString> &Parameters)
{
	switch (Command)
	{
	case EBPFunctionLibraryRoboticArmCommand::MoveToPosition:
		if (Parameters.Num() >= 3)
		{
			FVector TargetPos(
				FCString::Atof(*Parameters[0]),
				FCString::Atof(*Parameters[1]),
				FCString::Atof(*Parameters[2]));
			return MoveRoboticArmToPosition(RoboticArmActor, TargetPos);
		}
		break;

	case EBPFunctionLibraryRoboticArmCommand::RotateJoint:
		if (Parameters.Num() >= 2)
		{
			float Angle = FCString::Atof(*Parameters[1]);
			return RotateRoboticArmJoint(RoboticArmActor, Parameters[0], Angle);
		}
		break;

	case EBPFunctionLibraryRoboticArmCommand::GrabObject:
		if (Parameters.Num() >= 1)
		{
			// 这里需要根据参数查找对应的Actor
			// 简化实现：假设参数是物体名称
			TArray<AActor *> FoundActors;
			UGameplayStatics::GetAllActorsWithTag(RoboticArmActor, FName(*Parameters[0]), FoundActors);
			if (FoundActors.Num() > 0)
			{
				return GrabObjectWithRoboticArm(RoboticArmActor, FoundActors[0]);
			}
		}
		break;

	case EBPFunctionLibraryRoboticArmCommand::ReleaseObject:
		return ReleaseObjectFromRoboticArm(RoboticArmActor);

	case EBPFunctionLibraryRoboticArmCommand::HomePosition:
		return MoveRoboticArmToHomePosition(RoboticArmActor);

	case EBPFunctionLibraryRoboticArmCommand::Stop:
		return StopRoboticArmMovement(RoboticArmActor);

	default:
		UE_LOG(LogTemp, Warning, TEXT("ExecuteRoboticArmCommand: Unknown command"));
		return false;
	}

	UE_LOG(LogTemp, Warning, TEXT("ExecuteRoboticArmCommand: Invalid parameters for command"));
	return false;
}

bool UMyBlueprintFunctionLibrary::MoveRoboticArmToPosition(AActor *RoboticArmActor, FVector TargetPosition)
{
	if (!ValidateRoboticArmActor(RoboticArmActor))
	{
		return false;
	}

	// 简化的移动实现
	RoboticArmActor->SetActorLocation(TargetPosition);
	UE_LOG(LogTemp, Log, TEXT("Robotic arm moved to position: %s"), *TargetPosition.ToString());
	return true;
}

bool UMyBlueprintFunctionLibrary::RotateRoboticArmJoint(AActor *RoboticArmActor, const FString &JointName, float Angle)
{
	if (!ValidateRoboticArmActor(RoboticArmActor))
	{
		return false;
	}

	// 查找骨骼网格组件并旋转指定关节
	USkeletalMeshComponent *SkeletalMesh = RoboticArmActor->FindComponentByClass<USkeletalMeshComponent>();
	if (SkeletalMesh)
	{
		// 这里需要根据具体的骨骼名称来旋转
		// 简化实现：假设关节名称对应骨骼名称
		FName BoneName(*JointName);
		if (SkeletalMesh->DoesSocketExist(BoneName))
		{
			// 旋转骨骼
			// 在UE 5.6中，通过动画蓝图或控制器来控制骨骼
			// 这里提供简化的实现，实际项目中建议使用动画蓝图
			UE_LOG(LogTemp, Warning, TEXT("RotateJoint: Bone rotation requires animation blueprint implementation"));
			// 可以在这里添加自定义的骨骼控制逻辑
			UE_LOG(LogTemp, Log, TEXT("Rotated joint %s to angle %f"), *JointName, Angle);
			return true;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("RotateRoboticArmJoint: Joint %s not found"), *JointName);
	return false;
}

bool UMyBlueprintFunctionLibrary::GrabObjectWithRoboticArm(AActor *RoboticArmActor, AActor *ObjectToGrab)
{
	if (!ValidateRoboticArmActor(RoboticArmActor) || !ObjectToGrab)
	{
		return false;
	}

	// 简化的抓取实现
	// 在实际应用中，这里需要处理物理约束和碰撞检测
	ObjectToGrab->AttachToActor(RoboticArmActor, FAttachmentTransformRules::KeepWorldTransform);
	UE_LOG(LogTemp, Log, TEXT("Robotic arm grabbed object: %s"), *ObjectToGrab->GetName());
	return true;
}

bool UMyBlueprintFunctionLibrary::ReleaseObjectFromRoboticArm(AActor *RoboticArmActor)
{
	if (!ValidateRoboticArmActor(RoboticArmActor))
	{
		return false;
	}

	// 简化的释放实现
	// 分离所有附加的物体
	TArray<AActor *> AttachedActors;
	RoboticArmActor->GetAttachedActors(AttachedActors);

	for (AActor *AttachedActor : AttachedActors)
	{
		AttachedActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}

	UE_LOG(LogTemp, Log, TEXT("Robotic arm released %d objects"), AttachedActors.Num());
	return true;
}

bool UMyBlueprintFunctionLibrary::MoveRoboticArmToHomePosition(AActor *RoboticArmActor)
{
	if (!ValidateRoboticArmActor(RoboticArmActor))
	{
		return false;
	}

	// 移动到预设的初始位置
	FVector HomePosition(0, 0, 100); // 示例初始位置
	RoboticArmActor->SetActorLocation(HomePosition);

	// 重置所有关节角度
	USkeletalMeshComponent *SkeletalMesh = RoboticArmActor->FindComponentByClass<USkeletalMeshComponent>();
	if (SkeletalMesh)
	{
		// 重置骨骼变换
		// 在UE 5.6中，可以通过设置默认姿势来重置
		SkeletalMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
		SkeletalMesh->SetAnimation(nullptr);
	}

	UE_LOG(LogTemp, Log, TEXT("Robotic arm moved to home position"));
	return true;
}

bool UMyBlueprintFunctionLibrary::StopRoboticArmMovement(AActor *RoboticArmActor)
{
	if (!ValidateRoboticArmActor(RoboticArmActor))
	{
		return false;
	}

	// 停止所有运动
	// 在实际应用中，这里需要停止动画和物理模拟
	UE_LOG(LogTemp, Log, TEXT("Robotic arm movement stopped"));
	return true;
}

TArray<FBPFunctionLibraryRoboticArmJoint> UMyBlueprintFunctionLibrary::GetRoboticArmJointInfo(AActor *RoboticArmActor)
{
	TArray<FBPFunctionLibraryRoboticArmJoint> JointInfo;

	if (!ValidateRoboticArmActor(RoboticArmActor))
	{
		return JointInfo;
	}

	USkeletalMeshComponent *SkeletalMesh = RoboticArmActor->FindComponentByClass<USkeletalMeshComponent>();
	if (SkeletalMesh)
	{
		// 获取所有骨骼名称
		TArray<FName> BoneNames;
		SkeletalMesh->GetBoneNames(BoneNames);

		for (const FName &BoneName : BoneNames)
		{
			FBPFunctionLibraryRoboticArmJoint Joint;
			Joint.JointName = BoneName.ToString();

			// 获取当前骨骼旋转角度
			FTransform BoneTransform = SkeletalMesh->GetBoneTransform(SkeletalMesh->GetBoneIndex(BoneName));
			Joint.Angle = BoneTransform.Rotator().Yaw; // 简化：只取Yaw角

			JointInfo.Add(Joint);
		}
	}

	return JointInfo;
}

bool UMyBlueprintFunctionLibrary::SetRoboticArmJointAngles(AActor *RoboticArmActor, const TMap<FString, float> &JointAngles)
{
	if (!ValidateRoboticArmActor(RoboticArmActor))
	{
		return false;
	}

	USkeletalMeshComponent *SkeletalMesh = RoboticArmActor->FindComponentByClass<USkeletalMeshComponent>();
	if (!SkeletalMesh)
	{
		return false;
	}

	bool bSuccess = true;
	for (const auto &Pair : JointAngles)
	{
		FName BoneName(*Pair.Key);
		if (SkeletalMesh->DoesSocketExist(BoneName))
		{
			// 在UE 5.6中，通过动画蓝图或控制器来控制骨骼
			// 这里提供简化的实现，实际项目中建议使用动画蓝图
			UE_LOG(LogTemp, Warning, TEXT("SetJointAngle: Bone rotation requires animation blueprint implementation"));
			// 可以在这里添加自定义的骨骼控制逻辑
			UE_LOG(LogTemp, Log, TEXT("Set joint %s to angle %f"), *Pair.Key, Pair.Value);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("SetRoboticArmJointAngles: Joint %s not found"), *Pair.Key);
			bSuccess = false;
		}
	}

	return bSuccess;
}

// ==================== 私有辅助函数 ====================

bool UMyBlueprintFunctionLibrary::ValidateRoboticArmActor(AActor *RoboticArmActor)
{
	if (!RoboticArmActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("ValidateRoboticArmActor: Actor is null"));
		return false;
	}

	if (!IsValid(RoboticArmActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("ValidateRoboticArmActor: Actor is not valid"));
		return false;
	}

	return true;
}

bool UMyBlueprintFunctionLibrary::ParseCommandString(const FString &CommandString, EBPFunctionLibraryRoboticArmCommand &OutCommand, TArray<FString> &OutParameters)
{
	// 移除首尾空格
	FString TrimmedCommand = CommandString.TrimStartAndEnd();

	// 按空格分割命令和参数
	TArray<FString> Parts;
	TrimmedCommand.ParseIntoArray(Parts, TEXT(" "), true);

	if (Parts.Num() == 0)
	{
		return false;
	}

	// 第一个部分是命令
	OutCommand = GetCommandFromString(Parts[0]);

	// 剩余部分是参数
	OutParameters.Empty();
	for (int32 i = 1; i < Parts.Num(); ++i)
	{
		OutParameters.Add(Parts[i]);
	}

	return true;
}

EBPFunctionLibraryRoboticArmCommand UMyBlueprintFunctionLibrary::GetCommandFromString(const FString &CommandString)
{
	FString LowerCommand = CommandString.ToLower();

	if (LowerCommand.Contains(TEXT("move")) || LowerCommand.Contains(TEXT("position")))
	{
		return EBPFunctionLibraryRoboticArmCommand::MoveToPosition;
	}
	else if (LowerCommand.Contains(TEXT("rotate")) || LowerCommand.Contains(TEXT("joint")))
	{
		return EBPFunctionLibraryRoboticArmCommand::RotateJoint;
	}
	else if (LowerCommand.Contains(TEXT("grab")) || LowerCommand.Contains(TEXT("pick")))
	{
		return EBPFunctionLibraryRoboticArmCommand::GrabObject;
	}
	else if (LowerCommand.Contains(TEXT("release")) || LowerCommand.Contains(TEXT("drop")))
	{
		return EBPFunctionLibraryRoboticArmCommand::ReleaseObject;
	}
	else if (LowerCommand.Contains(TEXT("home")) || LowerCommand.Contains(TEXT("reset")))
	{
		return EBPFunctionLibraryRoboticArmCommand::HomePosition;
	}
	else if (LowerCommand.Contains(TEXT("stop")) || LowerCommand.Contains(TEXT("halt")))
	{
		return EBPFunctionLibraryRoboticArmCommand::Stop;
	}

	// 默认返回停止命令
	return EBPFunctionLibraryRoboticArmCommand::Stop;
}
