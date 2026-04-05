// ClimbingSystem.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ClimbingSystem.generated.h"

/**
 * 攀爬表面类型枚举
 */
UENUM(BlueprintType)
enum class EClimbSurfaceType : uint8
{
    Wall UMETA(DisplayName = "墙面"),
    Ledge UMETA(DisplayName = "边缘"),
    Corner UMETA(DisplayName = "角落"),
    Pole UMETA(DisplayName = "柱子")
};

/**
 * 攀爬动作类型枚举
 */
UENUM(BlueprintType)
enum class EClimbActionType : uint8
{
    StartClimb UMETA(DisplayName = "开始攀爬"),
    StopClimb UMETA(DisplayName = "停止攀爬"),
    MoveUp UMETA(DisplayName = "向上移动"),
    MoveDown UMETA(DisplayName = "向下移动"),
    MoveLeft UMETA(DisplayName = "向左移动"),
    MoveRight UMETA(DisplayName = "向右移动"),
    JumpClimb UMETA(DisplayName = "攀爬跳跃"),
    TraverseCorner UMETA(DisplayName = "转角"),
    VaultOver UMETA(DisplayName = "翻越")
};

/**
 * 攀爬检测结果结构体
 */
USTRUCT(BlueprintType)
struct FClimbDetectionResult
{
    GENERATED_BODY()

    /** 是否可以攀爬 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bCanClimb = false;

    /** 攀爬表面法线 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector SurfaceNormal = FVector::ZeroVector;

    /** 攀爬点位置 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector ClimbPoint = FVector::ZeroVector;

    /** 表面类型 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EClimbSurfaceType SurfaceType = EClimbSurfaceType::Wall;

    /** 到达攀爬点的距离 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DistanceToClimbPoint = 0.0f;
};

/**
 * 攀爬系统组件
 * 负责处理角色的攀爬逻辑检测和状态管理
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class RPG_TU2_API UClimbingSystem : public UActorComponent
{
    GENERATED_BODY()

public:
    // 构造函数
    UClimbingSystem();

protected:
    // 组件初始化
    virtual void BeginPlay() override;

public:
    /**
     * 检测前方是否可以攀爬
     * @param Character 角色引用
     * @param DetectionDistance 检测距离
     * @return 攀爬检测结果
     */
    UFUNCTION(BlueprintCallable, Category = "Climbing")
    FClimbDetectionResult DetectClimbableSurface(class ACharacter *Character, float DetectionDistance = 100.0f);

    /**
     * 检测下一个攀爬位置是否可行
     * @param CurrentLocation 当前位置
     * @param MoveDirection 移动方向
     * @param MoveDistance 移动距离
     * @return 是否可以移动到该位置
     */
    UFUNCTION(BlueprintCallable, Category = "Climbing")
    bool CanMoveToNextClimbPosition(FVector CurrentLocation, FVector MoveDirection, float MoveDistance = 50.0f);

    /**
     * 计算最佳攀爬位置
     * @param HitResult 碰撞检测结果
     * @return 优化后的攀爬位置
     */
    UFUNCTION(BlueprintCallable, Category = "Climbing")
    FVector CalculateOptimalClimbPosition(const FHitResult &HitResult);

    /**
     * 计算相对攀爬位置（基于角色当前位置）
     * @param Character 角色引用
     * @param HitResult 碰撞检测结果
     * @return 相对计算的攀爬位置
     */
    UFUNCTION(BlueprintCallable, Category = "Climbing")
    FVector CalculateRelativeClimbPosition(class ACharacter *Character, const FHitResult &HitResult);

    /**
     * 检测翻越点
     * @param Character 角色引用
     * @param CheckDistance 检测距离
     * @return 是否可以翻越及翻越点信息
     */
    UFUNCTION(BlueprintCallable, Category = "Climbing")
    bool DetectVaultPoint(class ACharacter *Character, FVector &OutVaultPoint, float CheckDistance = 150.0f);

    /**
     * 获取周围环境检测点
     * @param BaseLocation 基准位置
     * @param OffsetVectors 偏移向量数组
     */
    void GetSurroundingDetectionPoints(FVector BaseLocation, TArray<FVector> &OutDetectionPoints);

    /**
     * 控制角色攀爬时的速度
     * @param Character 角色引用
     * @param MaxClimbSpeed 最大攀爬速度
     */
    UFUNCTION(BlueprintCallable, Category = "Climbing")
    void ControlClimbingSpeed(class ACharacter *Character, float MaxClimbSpeed = 300.0f);

    /**
     * 应用攀爬惯性阻尼
     * @param Character 角色引用
     * @param DampingFactor 阻尼系数 (0.0-1.0)
     */
    UFUNCTION(BlueprintCallable, Category = "Climbing")
    void ApplyClimbingDamping(class ACharacter *Character, float DampingFactor = 0.9f);

    /**
     * 计算角色相对于墙面的正确朝向
     * @param WallNormal 墙面法线
     * @return 目标旋转
     */
    UFUNCTION(BlueprintCallable, Category = "Climbing")
    FRotator CalculateWallFacingRotation(const FVector &WallNormal);

private:
    /**
     * 验证表面是否适合攀爬
     * @param SurfaceNormal 表面法线
     * @param ActorTag 标签
     * @return 是否适合攀爬
     */
    bool IsValidClimbingSurface(const FVector &SurfaceNormal, const FName &ActorTag);

    /**
     * 检测边缘翻越可能性
     * @param EdgeLocation 边缘位置
     * @param CharacterHeight 角色高度
     * @return 是否可以翻越
     */
    bool CanVaultOverEdge(FVector EdgeLocation, float CharacterHeight = 180.0f);
};