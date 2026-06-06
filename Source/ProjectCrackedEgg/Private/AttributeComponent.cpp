#include "AttributeComponent.h"
#include "Math/UnrealMathUtility.h"

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;
	MaxMana = 50.0f;
	CurrentMana = MaxMana;
	BaseDamage = 10.0f;
	MovementSpeed = 600.0f;

	Level = 1;
	CurrentXP = 0;
	StatPoints = 0;
	BaseXPRequirement = 100;
	XPMultiplierPerLevel = 1.5f;
}

void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = FMath::Clamp(CurrentHealth, 0.0f, MaxHealth);
	CurrentMana = FMath::Clamp(CurrentMana, 0.0f, MaxMana);
}

int32 UAttributeComponent::CalculateRequiredXPForNextLevel() const
{
	return FMath::RoundToInt(BaseXPRequirement * FMath::Pow(XPMultiplierPerLevel, Level - 1));
}

void UAttributeComponent::SetAttributeValue(EAttributeType AttributeType, float NewValue)
{
	float OldValue = 0.0f;
	bool bChanged = false;

	switch (AttributeType)
	{
		case EAttributeType::Health:
		{
			OldValue = CurrentHealth;
			CurrentHealth = FMath::Clamp(NewValue, 0.0f, MaxHealth);
			bChanged = !FMath::IsNearlyEqual(OldValue, CurrentHealth);
			break;
		}
		case EAttributeType::MaxHealth:
		{
			OldValue = MaxHealth;
			MaxHealth = FMath::Max(0.0f, NewValue);
			CurrentHealth = FMath::Clamp(CurrentHealth, 0.0f, MaxHealth);
			bChanged = !FMath::IsNearlyEqual(OldValue, MaxHealth);
			break;
		}
		case EAttributeType::Mana:
		{
			OldValue = CurrentMana;
			CurrentMana = FMath::Clamp(NewValue, 0.0f, MaxMana);
			bChanged = !FMath::IsNearlyEqual(OldValue, CurrentMana);
			break;
		}
		case EAttributeType::MaxMana:
		{
			OldValue = MaxMana;
			MaxMana = FMath::Max(0.0f, NewValue);
			CurrentMana = FMath::Clamp(CurrentMana, 0.0f, MaxMana);
			bChanged = !FMath::IsNearlyEqual(OldValue, MaxMana);
			break;
		}
		case EAttributeType::BaseDamage:
		{
			OldValue = BaseDamage;
			BaseDamage = FMath::Max(0.0f, NewValue);
			bChanged = !FMath::IsNearlyEqual(OldValue, BaseDamage);
			break;
		}
		case EAttributeType::MovementSpeed:
		{
			OldValue = MovementSpeed;
			MovementSpeed = FMath::Max(0.0f, NewValue);
			bChanged = !FMath::IsNearlyEqual(OldValue, MovementSpeed);
			break;
		}
	}

	if (bChanged)
	{
		OnAttributeChanged.Broadcast(this, AttributeType, OldValue, GetAttributeValue(AttributeType));
	}
}

float UAttributeComponent::GetAttributeValue(EAttributeType AttributeType) const
{
	switch (AttributeType)
	{
		case EAttributeType::Health: return CurrentHealth;
		case EAttributeType::MaxHealth: return MaxHealth;
		case EAttributeType::Mana: return CurrentMana;
		case EAttributeType::MaxMana: return MaxMana;
		case EAttributeType::BaseDamage: return BaseDamage;
		case EAttributeType::MovementSpeed: return MovementSpeed;
	}
	return 0.0f;
}

void UAttributeComponent::ApplyHealthChange(float Delta)
{
	if (!IsAlive() && Delta < 0.0f)
	{
		return;
	}

	float OldHealth = CurrentHealth;
	SetAttributeValue(EAttributeType::Health, CurrentHealth + Delta);

	if (CurrentHealth <= 0.0f && OldHealth > 0.0f)
	{
		OnDeath.Broadcast(this);
	}
}

void UAttributeComponent::ApplyManaChange(float Delta)
{
	SetAttributeValue(EAttributeType::Mana, CurrentMana + Delta);
}

void UAttributeComponent::AddXP(int32 XPToAdd)
{
	if (XPToAdd <= 0) return;

	CurrentXP += XPToAdd;

	bool bLeveledUp = false;
	int32 RequiredXP = CalculateRequiredXPForNextLevel();

	while (CurrentXP >= RequiredXP)
	{
		CurrentXP -= RequiredXP;
		Level++;
		StatPoints++;
		bLeveledUp = true;
		RequiredXP = CalculateRequiredXPForNextLevel();
	}

	if (bLeveledUp)
	{
		OnLevelUp.Broadcast(this, Level, StatPoints);
	}
}

bool UAttributeComponent::UpgradeStat(EAttributeType StatToUpgrade)
{
	if (StatPoints <= 0) return false;

	switch (StatToUpgrade)
	{
		case EAttributeType::MaxHealth:
		case EAttributeType::Health:
		{
			SetAttributeValue(EAttributeType::MaxHealth, MaxHealth + HealthUpgradeAmount);
			ApplyHealthChange(HealthUpgradeAmount);
			break;
		}
		case EAttributeType::MaxMana:
		case EAttributeType::Mana:
		{
			SetAttributeValue(EAttributeType::MaxMana, MaxMana + ManaUpgradeAmount);
			ApplyManaChange(ManaUpgradeAmount);
			break;
		}
		case EAttributeType::BaseDamage:
		{
			SetAttributeValue(EAttributeType::BaseDamage, BaseDamage + DamageUpgradeAmount);
			break;
		}
		case EAttributeType::MovementSpeed:
		{
			SetAttributeValue(EAttributeType::MovementSpeed, MovementSpeed + MovementSpeedUpgradeAmount);
			break;
		}
		default:
			return false;
	}

	StatPoints--;
	return true;
}

bool UAttributeComponent::IsAlive() const
{
	return CurrentHealth > 0.0f;
}

float UAttributeComponent::GetXPProgressRatio() const
{
	float RequiredXP = static_cast<float>(CalculateRequiredXPForNextLevel());
	if (RequiredXP > 0.0f)
	{
		return static_cast<float>(CurrentXP) / RequiredXP;
	}
	return 0.0f;
}