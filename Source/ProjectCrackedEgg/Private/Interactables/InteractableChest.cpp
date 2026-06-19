#include "Interactables/InteractableChest.h"
#include "Interactables/BasePickup.h"

AInteractableChest::AInteractableChest()
{
	bSingleUse = true;
	LootSpawnOffset = FVector(0.0f, 0.0f, 50.0f);
}

void AInteractableChest::OnInteract_Implementation(AActor* Interactor)
{
	OnChestOpened();
}

void AInteractableChest::SpawnLoot()
{
	if (LootClass)
	{
		FVector SpawnLoc = GetActorLocation() + LootSpawnOffset;
		GetWorld()->SpawnActor<AActor>(LootClass, SpawnLoc, FRotator::ZeroRotator);
	}
}
