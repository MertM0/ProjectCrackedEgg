#include "Interactables/PickupEffects.h"
#include "Knight/AdvancedCharacter.h"
#include "Components/AttributeComponent.h"

APickup_HealthRestore::APickup_HealthRestore()
{
	HealthAmount = 50.0f;
}

void APickup_HealthRestore::ApplyPickupEffect_Implementation(AActor* Target)
{
	AAdvancedCharacter* Player = Cast<AAdvancedCharacter>(Target);
	if (Player)
	{
		UAttributeComponent* AttrComp = Player->FindComponentByClass<UAttributeComponent>();
		if (AttrComp)
		{
			AttrComp->ApplyHealthChange(HealthAmount);
		}
	}
}

APickup_XPGrant::APickup_XPGrant()
{
	XPAmount = 100;
}

void APickup_XPGrant::ApplyPickupEffect_Implementation(AActor* Target)
{
	AAdvancedCharacter* Player = Cast<AAdvancedCharacter>(Target);
	if (Player)
	{
		UAttributeComponent* AttrComp = Player->FindComponentByClass<UAttributeComponent>();
		if (AttrComp)
		{
			AttrComp->AddXP(XPAmount);
		}
	}
}

APickup_DamageBoost::APickup_DamageBoost()
{
	DamageMultiplier = 1.3f;
	Duration = 15.0f;
}

void APickup_DamageBoost::ApplyPickupEffect_Implementation(AActor* Target)
{
	AAdvancedCharacter* Player = Cast<AAdvancedCharacter>(Target);
	if (Player)
	{
		Player->ApplyDamageBoost(DamageMultiplier, Duration);
	}
}

APickup_StaminaReduction::APickup_StaminaReduction()
{
	StaminaCostMultiplier = 0.5f;
	Duration = 20.0f;
}

void APickup_StaminaReduction::ApplyPickupEffect_Implementation(AActor* Target)
{
	AAdvancedCharacter* Player = Cast<AAdvancedCharacter>(Target);
	if (Player)
	{
		Player->ApplyStaminaCostReduction(StaminaCostMultiplier, Duration);
	}
}

APickup_StaminaFull::APickup_StaminaFull()
{
}

void APickup_StaminaFull::ApplyPickupEffect_Implementation(AActor* Target)
{
	AAdvancedCharacter* Player = Cast<AAdvancedCharacter>(Target);
	if (Player)
	{
		UAttributeComponent* AttrComp = Player->FindComponentByClass<UAttributeComponent>();
		if (AttrComp)
		{
			float MaxStam = AttrComp->GetAttributeValue(EAttributeType::MaxStamina);
			float CurrStam = AttrComp->GetAttributeValue(EAttributeType::Stamina);
			AttrComp->ApplyStaminaChange(MaxStam - CurrStam);
		}
	}
}

APickup_Key::APickup_Key()
{
	KeyID = NAME_None;
}

void APickup_Key::ApplyPickupEffect_Implementation(AActor* Target)
{
	AAdvancedCharacter* Player = Cast<AAdvancedCharacter>(Target);
	if (Player)
	{
		Player->AddKey(KeyID);
	}
}
