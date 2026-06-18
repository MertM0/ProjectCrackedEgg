#include "StatusEffect_Burn.h"
#include "GameplayInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

UStatusEffect_Burn::UStatusEffect_Burn()
{
	ElementType = EElementalType::Fire;
	Duration = 4.0f;
	DamagePerTick = 3.0f;
	TickInterval = 0.5f;
	TimeSinceLastTick = 0.0f;
	BurnVFX = nullptr;
	SpawnedVFXComponent = nullptr;
}

void UStatusEffect_Burn::ApplyEffect(AActor* Target, AActor* Instigator)
{
	Super::ApplyEffect(Target, Instigator);
	TimeSinceLastTick = 0.0f;

	if (IsValid(Target) && BurnVFX)
	{
		USceneComponent* AttachComp = Target->GetRootComponent();
		TArray<USceneComponent*> Components;
		Target->GetComponents<USceneComponent>(Components);
		for (USceneComponent* SceneComp : Components)
		{
			if (SceneComp && SceneComp->GetName().Contains(TEXT("EffectVfx"), ESearchCase::IgnoreCase))
			{
				AttachComp = SceneComp;
				break;
			}
		}
		SpawnedVFXComponent = UGameplayStatics::SpawnEmitterAttached(BurnVFX, AttachComp);
	}
}

void UStatusEffect_Burn::TickEffect(float DeltaTime)
{
	Super::TickEffect(DeltaTime);

	TimeSinceLastTick += DeltaTime;

	if (TimeSinceLastTick >= TickInterval)
	{
		TimeSinceLastTick -= TickInterval;

		if (IsValid(TargetActor) && TargetActor->Implements<UGameplayInterface>())
		{
			IGameplayInterface::Execute_TakeElementalDamage(TargetActor, EElementalType::None, DamagePerTick, InstigatorActor);
		}
	}
}

void UStatusEffect_Burn::RemoveEffect()
{
	Super::RemoveEffect();

	if (SpawnedVFXComponent)
	{
		SpawnedVFXComponent->DestroyComponent();
		SpawnedVFXComponent = nullptr;
	}
}

