#pragma once

#include "CoreMinimal.h"
#include "Interactables/BaseInteractable.h"
#include "InteractableChest.generated.h"

class ABasePickup;

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

	virtual void OnInteract_Implementation(AActor* Interactor) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Chest Events")
	void OnChestOpened();

public:
	UFUNCTION(BlueprintCallable, Category = "Chest Functions")
	void SpawnLoot();
};
