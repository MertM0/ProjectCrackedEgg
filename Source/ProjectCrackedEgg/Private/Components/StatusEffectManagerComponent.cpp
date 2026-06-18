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
			float RemainingRatio = Effect->Duration > 0.0f ? ((Effect->Duration - Effect->ElapsedTime) / Effect->Duration) : 0.0f;
			OnStatusEffectTick.Broadcast(Effect->GetElementType(), RemainingRatio);
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
			OnStatusEffectApplied.Broadcast(Existing->GetElementType(), Existing->Duration);
			return;
		}
	}

	UStatusEffect* NewEffect = NewObject<UStatusEffect>(this, EffectClass);
	if (NewEffect)
	{
		NewEffect->ApplyEffect(GetOwner(), Instigator);
		ActiveEffects.Add(NewEffect);
		OnStatusEffectApplied.Broadcast(NewEffect->GetElementType(), NewEffect->Duration);
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
				EElementalType ElementType = ActiveEffects[i]->GetElementType();
				ActiveEffects[i]->RemoveEffect();
				OnStatusEffectRemoved.Broadcast(ElementType);
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
			EElementalType ElementType = Effect->GetElementType();
			Effect->RemoveEffect();
			OnStatusEffectRemoved.Broadcast(ElementType);
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

