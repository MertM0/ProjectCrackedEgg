#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayInterface.h"
#include "DragonCompanion.generated.h"

class UAttributeComponent;
class ABaseProjectile;
class AAIController;

UCLASS()
class PROJECTCRACKEDEGG_API ADragonCompanion : public ACharacter
{
	GENERATED_BODY()

public:
	ADragonCompanion();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAttributeComponent* AttributeComponent;

	UPROPERTY(BlueprintReadOnly, Category = "AI")
	AActor* TargetPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	EElementalType DragonElement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TArray<UAnimMontage*> GroundMeleeMontages;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	UAnimMontage* AirRangedMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TSubclassOf<ABaseProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	FVector MeleeHitboxSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	FName ProjectileSpawnSocket;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float MeleeAttackRange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float RangedAttackDistance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float MeleeCooldown;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float RangedCooldown;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	float MaxPlayerTetherDistance;

	float LastMeleeAttackTime;
	float LastRangedAttackTime;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	AActor* CurrentAttackTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visuals|Flight")
	bool bIsFlying;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals|Flight")
	float FlightDistanceThreshold;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals|Flight")
	float FlyingZOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals|Flight")
	float GroundZOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals|Flight")
	float FlightTransitionSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visuals|Animation")
	float FlightDirection;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals|Animation")
	UAnimMontage* SpawnMontage;

	float PreviousYaw;
	bool bIsInCombat;

	UPROPERTY()
	AAIController* AIC;

	UPROPERTY()
	AActor* CurrentMoveTarget;

	FTimerHandle TimerHandle_DecisionMaking;

	void UpdateAI();

	bool IsPlayingAttackMontage() const;

public:
	UAnimMontage* GetMeleeAttackMontage() const { return (GroundMeleeMontages.Num() > 0) ? GroundMeleeMontages[0] : nullptr; }
	UAnimMontage* GetRangedAttackMontage() const { return AirRangedMontage; }

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void PerformMeleeAttack();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SpawnProjectile(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetDragonElement(EElementalType NewElement);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Progression")
	class UParticleSystem* LevelUpVFX;

	UFUNCTION()
	void HandleLevelUp(UAttributeComponent* AttributeComp, int32 NewLevel, int32 AvailableStatPoints);
};
