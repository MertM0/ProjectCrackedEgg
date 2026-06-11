#include "StatusEffectManagerComponent.h"
#include "StatusEffect.h"

UStatusEffectManagerComponent::UStatusEffectManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UStatusEffectManagerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UStatusEffectManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (UStatusEffect* Effect : ActiveEffects)
	{
		if (IsValid(Effect) && !Effect->IsExpired())
		{
			Effect->TickEffect(DeltaTime);
		}
	}

	RemoveExpiredEffects();
}

void UStatusEffectManagerComponent::ApplyStatusEffect(TSubclassOf<UStatusEffect> EffectClass, AActor* Instigator)
{
	if (!EffectClass) return;

	for (UStatusEffect* Existing : ActiveEffects)
	{
		if (IsValid(Existing) && Existing->GetClass() == EffectClass)
		{
			Existing->ResetDuration();
			return;
		}
	}

	UStatusEffect* NewEffect = NewObject<UStatusEffect>(this, EffectClass);
	if (NewEffect)
	{
		NewEffect->ApplyEffect(GetOwner(), Instigator);
		ActiveEffects.Add(NewEffect);
	}
}

void UStatusEffectManagerComponent::RemoveExpiredEffects()
{
	for (int32 i = ActiveEffects.Num() - 1; i >= 0; i--)
	{
		if (!IsValid(ActiveEffects[i]) || ActiveEffects[i]->IsExpired())
		{
			if (IsValid(ActiveEffects[i]))
			{
				ActiveEffects[i]->RemoveEffect();
			}
			ActiveEffects.RemoveAt(i);
		}
	}
}

void UStatusEffectManagerComponent::ClearAllEffects()
{
	for (UStatusEffect* Effect : ActiveEffects)
	{
		if (IsValid(Effect))
		{
			Effect->RemoveEffect();
		}
	}
	ActiveEffects.Empty();
}

bool UStatusEffectManagerComponent::HasActiveEffect(TSubclassOf<UStatusEffect> EffectClass) const
{
	for (const UStatusEffect* Effect : ActiveEffects)
	{
		if (IsValid(Effect) && Effect->GetClass() == EffectClass && !Effect->IsExpired())
		{
			return true;
		}
	}
	return false;
}

