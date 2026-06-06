#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "AdvancedCharacter.generated.h"

class UAttributeComponent;
class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class PROJECTCRACKEDEGG_API AAdvancedCharacter : public ACharacter
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
	void StartSprint();
	void StopSprint();

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

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	bool bCanRoll;
	bool bIsSprinting;
	FTimerHandle TimerHandle_RollCooldown;
	void ResetRoll();

	bool bIsAttacking;
	bool bSaveAttack;
	bool bComboWindowOpen;
	bool bIsSweeping;
	int32 ComboIndex;

	TArray<AActor*> HitActorsDuringAttack;
	FVector PreviousWeaponStart;
	FVector PreviousWeaponEnd;

	void PerformWeaponSweep();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAttributeComponent* AttributeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float RollStrength;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float RollCooldown;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float SprintMultiplier;
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	float InteractionRange;
};