#pragma once

#include "CoreMinimal.h"
#include "Interactables/BaseInteractable.h"
#include "InteractableChest.generated.h"

class ABasePickup;
class UParticleSystem;
class USoundBase;

UCLASS()
class PROJECTCRACKEDEGG_API AInteractableChest : public ABaseInteractable
{
	GENERATED_BODY()

public:
	AInteractableChest();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chest Loot")
	TSubclassOf<ABasePickup> LootClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chest Loot")
	FVector LootSpawnOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chest Effects")
	UParticleSystem* ExplosionVFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chest Effects")
	USoundBase* ExplosionSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chest Animation")
	float ScaleUpDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chest Animation")
	float MaxScale;

	virtual void OnInteract_Implementation(AActor* Interactor) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Chest Events")
	void OnChestOpened();

public:
	UFUNCTION(BlueprintCallable, Category = "Chest Functions")
	void SpawnLoot();
};
