#include "StatusEffect.h"

UStatusEffect::UStatusEffect()
{
	ElementType = EElementalType::None;
	Duration = 5.0f;
	ElapsedTime = 0.0f;
	TargetActor = nullptr;
	InstigatorActor = nullptr;
}

void UStatusEffect::ApplyEffect(AActor* Target, AActor* Instigator)
{
	TargetActor = Target;
	InstigatorActor = Instigator;
	ElapsedTime = 0.0f;
}

void UStatusEffect::TickEffect(float DeltaTime)
{
	ElapsedTime += DeltaTime;
}

void UStatusEffect::RemoveEffect()
{
	TargetActor = nullptr;
	InstigatorActor = nullptr;
}

bool UStatusEffect::IsExpired() const
{
	return ElapsedTime >= Duration;
}

void UStatusEffect::ResetDuration()
{
	ElapsedTime = 0.0f;
}

