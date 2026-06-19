#include "Interactables/BreakableObject.h"
#include "Interactables/BasePickup.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Dragon/DragonCompanion.h"
#include "Knight/AdvancedCharacter.h"

ABreakableObject::ABreakableObject()
{
	PrimaryActorTick.bCanEverTick = false;
	LootTable = nullptr;
	LootCollectionDelay = 0.5f;
	bOnlyBreakableByDragon = false;
	bOnlyBreakableByPlayer = false;
	BreakDelay = 0.0f;
	bSingleUse = true;
}

void ABreakableObject::Interact_Implementation(AActor* Interactor)
{
	if (!bCanInteract || bHasBeenUsed)
	{
		return;
	}

	OnInteract(Interactor);
}

void ABreakableObject::TakeElementalDamage_Implementation(EElementalType Element, float Damage, AActor* DamageInstigator)
{
	if (bHasBeenUsed)
	{
		return;
	}

	if (bOnlyBreakableByDragon)
	{
		if (!DamageInstigator || !DamageInstigator->IsA(ADragonCompanion::StaticClass()))
		{
			return;
		}
	}

	if (bOnlyBreakableByPlayer)
	{
		if (!DamageInstigator || !DamageInstigator->IsA(AAdvancedCharacter::StaticClass()))
		{
			return;
		}
	}

	bHasBeenUsed = true;
	StartBreakSequence();
}

void ABreakableObject::StartBreakSequence()
{
	OnBreakStarted();

	if (BreakDelay > 0.0f)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_BreakDelay, this, &ABreakableObject::BreakObject, BreakDelay, false);
	}
	else
	{
		BreakObject();
	}
}

void ABreakableObject::BreakObject()
{
	bCanInteract = false;
	OnObjectBroken();

	if (DestroyVFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), DestroyVFX, GetActorLocation(), GetActorRotation());
	}

	if (DestroySound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DestroySound, GetActorLocation());
	}

	if (LootTable)
	{
		TArray<FLootRow*> Rows;
		LootTable->GetAllRows<FLootRow>(TEXT(""), Rows);

		float TotalWeight = 0.0f;
		for (const FLootRow* Row : Rows)
		{
			if (Row)
			{
				TotalWeight += Row->Weight;
			}
		}

		if (TotalWeight > 0.0f)
		{
			float RandomRoll = FMath::FRandRange(0.0f, TotalWeight);
			float CurrentWeightSum = 0.0f;

			for (const FLootRow* Row : Rows)
			{
				if (Row)
				{
					CurrentWeightSum += Row->Weight;
					if (RandomRoll <= CurrentWeightSum)
					{
						if (Row->PickupClass)
						{
							FTransform SpawnTransform(FRotator::ZeroRotator, GetActorLocation());
							ABasePickup* NewPickup = GetWorld()->SpawnActorDeferred<ABasePickup>(
								Row->PickupClass,
								SpawnTransform,
								nullptr,
								nullptr,
								ESpawnActorCollisionHandlingMethod::AlwaysSpawn
							);
							if (NewPickup)
							{
								NewPickup->SetCollectionDelay(LootCollectionDelay);
								NewPickup->FinishSpawning(SpawnTransform);
							}
						}
						break;
					}
				}
			}
		}
	}

	Destroy();
}
