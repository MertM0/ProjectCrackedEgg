#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "GameplayInterface.h"
#include "AdvancedCharacter.generated.h"

class UAttributeComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStaminaExhaustedActionSignature);
class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class USpringArmComponent;
class UStatusEffectManagerComponent;
class UStatusEffect;

UCLASS()
class PROJECTCRACKEDEGG_API AAdvancedCharacter : public ACharacter, public IGameplayInterface
{
	GENERATED_BODY()

public:
	AAdvancedCharacter();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LightAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* HeavyAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* RollAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* InteractAction;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void LightAttack();
	void HeavyAttack();
public:
	void StartSprint();
	void StopSprint();
protected:

	virtual void Jump() override;

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void ExecuteRoll();

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void ExecuteInteractionLineTrace();

	UFUNCTION(BlueprintPure, Category = "Movement")
	bool IsSprinting() const { return bIsSprinting; }

public:

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void StartWeaponSweep();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void StopWeaponSweep();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SaveCombo();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ResetCombo();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	void PlayLightAttackMontage(int32 ComboStep);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	void PlayHeavyAttackMontage();

	virtual void TakeElementalDamage_Implementation(EDragonElement Element, float Damage, AActor* DamageInstigator) override;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void PerformWeaponSweep();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void PerformHeavySweep();

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	AActor* LastAttackedEnemy;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	bool bIsDead;
	bool bCanRoll;
	bool bIsSprinting;
	FTimerHandle TimerHandle_RollCooldown;
	void ResetRoll();

	UFUNCTION()
	void HandleDeath(UAttributeComponent* AttributeComp);

	UFUNCTION(BlueprintImplementableEvent, Category = "Death")
	void OnDeath();

	FTimerHandle TimerHandle_Ragdoll;
	FTimerHandle TimerHandle_DeathSequence;

	void TriggerRagdoll();
	void OnDeathSequenceFinished();

	FTimerHandle TimerHandle_WeaponSweep;

	bool bIsAttacking;
	bool bSaveAttack;
	bool bComboWindowOpen;
	int32 ComboIndex;

	TArray<AActor*> HitActorsDuringAttack;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAttributeComponent* AttributeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStatusEffectManagerComponent* StatusEffectManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* EffectVfx;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float RollStrength;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float RollCooldown;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float SprintMultiplier;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float InteractionRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FVector InteractionBoxHalfExtents;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	class UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	class UAnimMontage* DeathMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float HitReactChance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	class UParticleSystem* HitVFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	FVector LightAttackHitboxSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float HeavyAttackHitboxRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float HeavyAttackDamageMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina Settings")
	float SprintStaminaDrainRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina Settings")
	float JumpStaminaCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina Settings")
	float RollStaminaCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina Settings")
	float LightAttackStaminaCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina Settings")
	float HeavyAttackStaminaCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina Settings")
	float StaminaRegenRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina Settings")
	float StaminaRegenDelay;

	float StaminaRegenTimer;

	UPROPERTY(BlueprintAssignable, Category = "Attributes|Events")
	FOnStaminaExhaustedActionSignature OnStaminaExhaustedAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes|Progression")
	class UParticleSystem* LevelUpVFX;

	UFUNCTION()
	void HandleLevelUp(UAttributeComponent* AttributeComp, int32 NewLevel, int32 AvailableStatPoints);

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class ADamageTextActor> DamageTextClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TSubclassOf<UStatusEffect> BurnEffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TSubclassOf<UStatusEffect> SlowEffectClass;
};
