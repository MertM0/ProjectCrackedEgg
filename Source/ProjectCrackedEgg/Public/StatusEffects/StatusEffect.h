#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayInterface.h"
#include "StatusEffect.generated.h"

UCLASS(Abstract, Blueprintable)
class PROJECTCRACKEDEGG_API UStatusEffect : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status Effect")
	EDragonElement ElementType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status Effect")
	float Duration;

	float ElapsedTime;

	UPROPERTY()
	AActor* TargetActor;

	UPROPERTY()
	AActor* InstigatorActor;

	UStatusEffect();

	virtual void ApplyEffect(AActor* Target, AActor* Instigator);
	virtual void TickEffect(float DeltaTime);
	virtual void RemoveEffect();

	UFUNCTION(BlueprintPure, Category = "Status Effect")
	bool IsExpired() const;

	UFUNCTION(BlueprintPure, Category = "Status Effect")
	EDragonElement GetElementType() const { return ElementType; }

	void ResetDuration();
};

