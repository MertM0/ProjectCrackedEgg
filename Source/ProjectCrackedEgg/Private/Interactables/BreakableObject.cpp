#include "Interactables/BreakableObject.h"
#include "Interactables/BasePickup.h"
#include "Kismet/GameplayStatics.h"

ABreakableObject::ABreakableObject()
{
	PrimaryActorTick.bCanEverTick = false;
	MaxHealth = 30.0f;
	CurrentHealth = 30.0f;
	LootTable = nullptr;
	bSingleUse = true;
}

void ABreakableObject::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
}

void ABreakableObject::TakeElementalDamage_Implementation(EElementalType Element, float Damage, AActor* DamageInstigator)
{
	if (CurrentHealth <= 0.0f)
	{
		return;
	}

	CurrentHealth -= Damage;
	if (CurrentHealth <= 0.0f)
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
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DestroyVFX, GetActorLocation(), GetActorRotation());
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
							FActorSpawnParameters SpawnParams;
							SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
							GetWorld()->SpawnActor<AActor>(Row->PickupClass, GetActorLocation(), FRotator::ZeroRotator, SpawnParams);
						}
						break;
					}
				}
			}
		}
	}

	Destroy();
}
