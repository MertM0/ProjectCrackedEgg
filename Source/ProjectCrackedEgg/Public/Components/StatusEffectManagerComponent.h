#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayInterface.h"
#include "StatusEffectManagerComponent.generated.h"

class UStatusEffect;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStatusEffectApplied, EElementalType, ElementType, float, Duration);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStatusEffectRemoved, EElementalType, ElementType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStatusEffectTick, EElementalType, ElementType, float, RemainingRatio);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTCRACKEDEGG_API UStatusEffectManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UStatusEffectManagerComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Status Effects")
	void ApplyStatusEffect(TSubclassOf<UStatusEffect> EffectClass, AActor* Instigator);

	UFUNCTION(BlueprintCallable, Category = "Status Effects")
	void ClearAllEffects();

	UFUNCTION(BlueprintPure, Category = "Status Effects")
	bool HasActiveEffect(TSubclassOf<UStatusEffect> EffectClass) const;

	const TArray<UStatusEffect*>& GetActiveEffects() const { return ActiveEffects; }

	UPROPERTY(BlueprintAssignable, Category = "Status Effects")
	FOnStatusEffectApplied OnStatusEffectApplied;

	UPROPERTY(BlueprintAssignable, Category = "Status Effects")
	FOnStatusEffectRemoved OnStatusEffectRemoved;

	UPROPERTY(BlueprintAssignable, Category = "Status Effects")
	FOnStatusEffectTick OnStatusEffectTick;

protected:
	UPROPERTY()
	TArray<UStatusEffect*> ActiveEffects;

	void RemoveExpiredEffects();
};

