#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatusEffectManagerComponent.generated.h"

class UStatusEffect;

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

protected:
	UPROPERTY()
	TArray<UStatusEffect*> ActiveEffects;

	void RemoveExpiredEffects();
};

