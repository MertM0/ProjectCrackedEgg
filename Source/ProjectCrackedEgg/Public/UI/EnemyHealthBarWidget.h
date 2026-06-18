#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/AttributeComponent.h"
#include "GameplayInterface.h"
#include "EnemyHealthBarWidget.generated.h"

class UStatusEffectManagerComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEnemyHealthChangedSignature, float, CurrentHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnEnemyStatusEffectChangedSignature, EElementalType, ElementType, bool, bIsActive, float, RemainingRatio);

UCLASS()
class PROJECTCRACKEDEGG_API UEnemyHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnEnemyHealthChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnEnemyStatusEffectChangedSignature OnStatusEffectChanged;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetOwnerEnemy(AActor* InOwnerEnemy);

	UFUNCTION(BlueprintPure, Category = "UI")
	float GetCurrentHealth() const;

	UFUNCTION(BlueprintPure, Category = "UI")
	float GetMaxHealth() const;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	AActor* OwnerEnemy;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	UAttributeComponent* AttributeComponent;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	UStatusEffectManagerComponent* StatusEffectManager;

	UFUNCTION()
	void OnAttributeChanged(UAttributeComponent* AttributeComp, EAttributeType AttributeType, float OldValue, float NewValue);

	UFUNCTION()
	void HandleStatusEffectApplied(EElementalType ElementType, float Duration);

	UFUNCTION()
	void HandleStatusEffectRemoved(EElementalType ElementType);

	UFUNCTION()
	void HandleStatusEffectTick(EElementalType ElementType, float RemainingRatio);

	void UpdateHealthBar();
	void InitializeStatusEffectsState();
};
