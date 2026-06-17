#include "UI/EnemyHealthBarWidget.h"
#include "AttributeComponent.h"
#include "StatusEffectManagerComponent.h"
#include "StatusEffect.h"

void UEnemyHealthBarWidget::SetOwnerEnemy(AActor* InOwnerEnemy)
{
	if (!InOwnerEnemy || OwnerEnemy == InOwnerEnemy)
	{
		return;
	}

	if (AttributeComponent)
	{
		AttributeComponent->OnAttributeChanged.RemoveAll(this);
	}

	if (StatusEffectManager)
	{
		StatusEffectManager->OnStatusEffectApplied.RemoveAll(this);
		StatusEffectManager->OnStatusEffectRemoved.RemoveAll(this);
		StatusEffectManager->OnStatusEffectTick.RemoveAll(this);
	}

	OwnerEnemy = InOwnerEnemy;
	AttributeComponent = OwnerEnemy->FindComponentByClass<UAttributeComponent>();
	StatusEffectManager = OwnerEnemy->FindComponentByClass<UStatusEffectManagerComponent>();

	if (AttributeComponent)
	{
		AttributeComponent->OnAttributeChanged.AddDynamic(this, &UEnemyHealthBarWidget::OnAttributeChanged);
		UpdateHealthBar();
	}

	if (StatusEffectManager)
	{
		StatusEffectManager->OnStatusEffectApplied.AddDynamic(this, &UEnemyHealthBarWidget::HandleStatusEffectApplied);
		StatusEffectManager->OnStatusEffectRemoved.AddDynamic(this, &UEnemyHealthBarWidget::HandleStatusEffectRemoved);
		StatusEffectManager->OnStatusEffectTick.AddDynamic(this, &UEnemyHealthBarWidget::HandleStatusEffectTick);

		InitializeStatusEffectsState();
	}
}

float UEnemyHealthBarWidget::GetCurrentHealth() const
{
	return AttributeComponent ? AttributeComponent->GetAttributeValue(EAttributeType::Health) : 0.0f;
}

float UEnemyHealthBarWidget::GetMaxHealth() const
{
	return AttributeComponent ? AttributeComponent->GetAttributeValue(EAttributeType::MaxHealth) : 0.0f;
}

void UEnemyHealthBarWidget::OnAttributeChanged(UAttributeComponent* AttributeComp, EAttributeType AttributeType, float OldValue, float NewValue)
{
	if (AttributeType == EAttributeType::Health || AttributeType == EAttributeType::MaxHealth)
	{
		UpdateHealthBar();
	}
}

void UEnemyHealthBarWidget::UpdateHealthBar()
{
	if (!AttributeComponent)
	{
		return;
	}

	float CurrentHealth = AttributeComponent->GetAttributeValue(EAttributeType::Health);
	float MaxHealth = AttributeComponent->GetAttributeValue(EAttributeType::MaxHealth);

	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void UEnemyHealthBarWidget::HandleStatusEffectApplied(EDragonElement ElementType, float Duration)
{
	OnStatusEffectChanged.Broadcast(ElementType, true, 1.0f);
}

void UEnemyHealthBarWidget::HandleStatusEffectRemoved(EDragonElement ElementType)
{
	OnStatusEffectChanged.Broadcast(ElementType, false, 0.0f);
}

void UEnemyHealthBarWidget::HandleStatusEffectTick(EDragonElement ElementType, float RemainingRatio)
{
	OnStatusEffectChanged.Broadcast(ElementType, true, RemainingRatio);
}

void UEnemyHealthBarWidget::InitializeStatusEffectsState()
{
	if (!StatusEffectManager)
	{
		return;
	}

	bool bHasBurn = false;
	float BurnRatio = 0.0f;

	bool bHasSlow = false;
	float SlowRatio = 0.0f;

	const TArray<UStatusEffect*>& ActiveEffects = StatusEffectManager->GetActiveEffects();
	for (UStatusEffect* Effect : ActiveEffects)
	{
		if (IsValid(Effect) && !Effect->IsExpired())
		{
			float RemainingRatio = Effect->Duration > 0.0f ? ((Effect->Duration - Effect->ElapsedTime) / Effect->Duration) : 0.0f;
			if (Effect->ElementType == EDragonElement::Fire)
			{
				bHasBurn = true;
				BurnRatio = RemainingRatio;
			}
			else if (Effect->ElementType == EDragonElement::Lightning)
			{
				bHasSlow = true;
				SlowRatio = RemainingRatio;
			}
		}
	}

	OnStatusEffectChanged.Broadcast(EDragonElement::Fire, bHasBurn, BurnRatio);
	OnStatusEffectChanged.Broadcast(EDragonElement::Lightning, bHasSlow, SlowRatio);
}
