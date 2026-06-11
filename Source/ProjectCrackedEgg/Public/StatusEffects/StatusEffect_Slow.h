#pragma once

#include "CoreMinimal.h"
#include "StatusEffect.h"
#include "StatusEffect_Slow.generated.h"

UCLASS()
class PROJECTCRACKEDEGG_API UStatusEffect_Slow : public UStatusEffect
{
	GENERATED_BODY()

public:
	UStatusEffect_Slow();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Slow")
	float SlowMultiplier;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Slow")
	class UParticleSystem* SlowVFX;

	UPROPERTY()
	class UParticleSystemComponent* SpawnedVFXComponent;

	float OriginalSpeed;

	virtual void ApplyEffect(AActor* Target, AActor* Instigator) override;
	virtual void TickEffect(float DeltaTime) override;
	virtual void RemoveEffect() override;
};

