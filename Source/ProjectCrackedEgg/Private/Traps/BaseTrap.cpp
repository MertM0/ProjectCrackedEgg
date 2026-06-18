#include "Traps/BaseTrap.h"
#include "Components/BoxComponent.h"
#include "Knight/AdvancedCharacter.h"
#include "Enemies/BaseEnemy.h"
#include "Engine/World.h"

ABaseTrap::ABaseTrap()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	TriggerVolume->SetupAttachment(RootComponent);
	TriggerVolume->SetCollisionProfileName(TEXT("Trigger"));

	BaseDamage = 20.0f;
	TrapElement = EElementalType::None;
	DamageCooldown = 1.0f;
	bAffectsEnemies = false;
}

void ABaseTrap::BeginPlay()
{
	Super::BeginPlay();
}

bool ABaseTrap::DealDamage(AActor* Target)
{
	if (!Target)
	{
		return false;
	}

	if (!Target->Implements<UGameplayInterface>())
	{
		return false;
	}

	bool bIsValidTarget = false;
	if (Target->IsA(AAdvancedCharacter::StaticClass()))
	{
		bIsValidTarget = true;
	}
	else if (bAffectsEnemies && Target->IsA(ABaseEnemy::StaticClass()))
	{
		bIsValidTarget = true;
	}

	if (!bIsValidTarget)
	{
		return false;
	}

	for (auto It = LastDamageTimeMap.CreateIterator(); It; ++It)
	{
		if (!IsValid(It.Key()))
		{
			It.RemoveCurrent();
		}
	}

	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (DamageCooldown > 0.0f && LastDamageTimeMap.Contains(Target))
	{
		if (CurrentTime - LastDamageTimeMap[Target] < DamageCooldown)
		{
			return false;
		}
	}

	LastDamageTimeMap.Add(Target, CurrentTime);

	IGameplayInterface::Execute_TakeElementalDamage(Target, TrapElement, BaseDamage, this);
	return true;
}

void ABaseTrap::ResetCooldownForActor(AActor* Actor)
{
	if (Actor)
	{
		LastDamageTimeMap.Remove(Actor);
	}
}

void ABaseTrap::ResetAllCooldowns()
{
	LastDamageTimeMap.Empty();
}
