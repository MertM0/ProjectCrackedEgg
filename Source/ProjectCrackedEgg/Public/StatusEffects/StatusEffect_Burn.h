#pragma once

#include "CoreMinimal.h"
#include "StatusEffect.h"
#include "StatusEffect_Burn.generated.h"

UCLASS()
class PROJECTCRACKEDEGG_API UStatusEffect_Burn : public UStatusEffect
{
	GENERATED_BODY()

public:
	UStatusEffect_Burn();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Burn")
	float DamagePerTick;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Burn")
	float TickInterval;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Burn")
	class UParticleSystem* BurnVFX;

	UPROPERTY()
	class UParticleSystemComponent* SpawnedVFXComponent;

	float TimeSinceLastTick;

	virtual void ApplyEffect(AActor* Target, AActor* Instigator) override;
	virtual void TickEffect(float DeltaTime) override;
	virtual void RemoveEffect() override;
};

