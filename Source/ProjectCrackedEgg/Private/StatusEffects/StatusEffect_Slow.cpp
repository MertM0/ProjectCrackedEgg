#include "StatusEffect_Slow.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Knight/AdvancedCharacter.h"

UStatusEffect_Slow::UStatusEffect_Slow()
{
	ElementType = EElementalType::Lightning;
	Duration = 3.0f;
	SlowMultiplier = 0.5f;
	OriginalSpeed = 0.0f;
	SlowVFX = nullptr;
	SpawnedVFXComponent = nullptr;
}

void UStatusEffect_Slow::ApplyEffect(AActor* Target, AActor* Instigator)
{
	Super::ApplyEffect(Target, Instigator);

	if (AAdvancedCharacter* TargetCharacter = Cast<AAdvancedCharacter>(TargetActor))
	{
		TargetCharacter->StopSprint();
	}

	if (ACharacter* TargetCharacter = Cast<ACharacter>(TargetActor))
	{
		if (UCharacterMovementComponent* MovementComp = TargetCharacter->GetCharacterMovement())
		{
			OriginalSpeed = MovementComp->MaxWalkSpeed;
			MovementComp->MaxWalkSpeed *= SlowMultiplier;
		}
	}

	if (IsValid(Target) && SlowVFX)
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
		SpawnedVFXComponent = UGameplayStatics::SpawnEmitterAttached(SlowVFX, AttachComp);
	}
}

void UStatusEffect_Slow::TickEffect(float DeltaTime)
{
	Super::TickEffect(DeltaTime);
}

void UStatusEffect_Slow::RemoveEffect()
{
	if (IsValid(TargetActor))
	{
		if (ACharacter* TargetCharacter = Cast<ACharacter>(TargetActor))
		{
			if (UCharacterMovementComponent* MovementComp = TargetCharacter->GetCharacterMovement())
			{
				MovementComp->MaxWalkSpeed = OriginalSpeed;
			}
		}
	}

	if (SpawnedVFXComponent)
	{
		SpawnedVFXComponent->DestroyComponent();
		SpawnedVFXComponent = nullptr;
	}

	Super::RemoveEffect();
}

