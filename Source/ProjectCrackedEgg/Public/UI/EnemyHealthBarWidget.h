#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/AttributeComponent.h"
#include "EnemyHealthBarWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEnemyHealthChangedSignature, float, CurrentHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnEnemyStatusEffectChangedSignature, EDragonElement, ElementType, bool, bIsActive, float, RemainingRatio);

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
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	AActor* OwnerEnemy;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	UAttributeComponent* AttributeComponent;

	UFUNCTION()
	void OnAttributeChanged(UAttributeComponent* AttributeComp, EAttributeType AttributeType, float OldValue, float NewValue);

	void UpdateHealthBar();
	void UpdateStatusEffects();

private:
	bool bLastHasBurn = false;
	bool bLastHasSlow = false;
};
