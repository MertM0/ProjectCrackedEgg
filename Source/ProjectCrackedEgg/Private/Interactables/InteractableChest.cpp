#include "Interactables/InteractableChest.h"
#include "Interactables/BasePickup.h"
#include "Kismet/GameplayStatics.h"

AInteractableChest::AInteractableChest()
{
	bSingleUse = true;
	LootSpawnOffset = FVector(0.0f, 0.0f, 50.0f);
	ScaleUpDuration = 0.3f;
	MaxScale = 1.5f;
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

	if (ExplosionVFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionVFX, GetActorLocation(), GetActorRotation());
	}

	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
	}

	Destroy();
}
