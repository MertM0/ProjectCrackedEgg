#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayInterface.h"
#include "BaseEnemy.generated.h"

class UAttributeComponent;
class UStatusEffectManagerComponent;
class UWidgetComponent;

UCLASS()
class PROJECTCRACKEDEGG_API ABaseEnemy : public ACharacter, public IGameplayInterface
{
	GENERATED_BODY()

public:
	ABaseEnemy();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAttributeComponent* AttributeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStatusEffectManagerComponent* StatusEffectManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UWidgetComponent* HealthBarWidgetComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Status Effects")
	TSubclassOf<class UStatusEffect> BurnEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Status Effects")
	TSubclassOf<class UStatusEffect> SlowEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class ADamageTextActor> DamageTextClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* ProximitySphere;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rewards")
	int32 RewardXP;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	class UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	class UAnimMontage* DeathMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float HitReactChance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	class UParticleSystem* HitVFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	class UAnimMontage* MeleeAttackMontage;

	UFUNCTION()
	void HandleDeath(UAttributeComponent* AttributeComp);

	UFUNCTION()
	void OnProximityOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void TriggerRagdoll();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	bool bHideHealthBarWhenFullHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	float HealthBarVisibilityDuration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	float MaxHealthBarDisplayDistance;

	FTimerHandle TimerHandle_HideHealthBar;
	FTimerHandle TimerHandle_DistanceCheck;

	UFUNCTION()
	void HideHealthBar();

	void UpdateHealthBarVisibility();

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void Interact_Implementation(AActor* Interactor) override;

	virtual void TakeElementalDamage_Implementation(EElementalType Element, float Damage, AActor* Instigator) override;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void PerformMeleeStrike();

	class UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }
	class UAnimMontage* GetMeleeAttackMontage() const { return MeleeAttackMontage; }
};
