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

	OwnerEnemy = InOwnerEnemy;
	AttributeComponent = OwnerEnemy->FindComponentByClass<UAttributeComponent>();

	if (AttributeComponent)
	{
		AttributeComponent->OnAttributeChanged.AddDynamic(this, &UEnemyHealthBarWidget::OnAttributeChanged);
		UpdateHealthBar();
	}

	bLastHasBurn = false;
	bLastHasSlow = false;
	UpdateStatusEffects();
}

float UEnemyHealthBarWidget::GetCurrentHealth() const
{
	return AttributeComponent ? AttributeComponent->GetAttributeValue(EAttributeType::Health) : 0.0f;
}

float UEnemyHealthBarWidget::GetMaxHealth() const
{
	return AttributeComponent ? AttributeComponent->GetAttributeValue(EAttributeType::MaxHealth) : 0.0f;
}

void UEnemyHealthBarWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	UpdateStatusEffects();
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

void UEnemyHealthBarWidget::UpdateStatusEffects()
{
	if (!OwnerEnemy)
	{
		return;
	}

	UStatusEffectManagerComponent* StatusManager = OwnerEnemy->FindComponentByClass<UStatusEffectManagerComponent>();
	if (!StatusManager)
	{
		return;
	}

	bool bHasBurn = false;
	float BurnRatio = 0.0f;

	bool bHasSlow = false;
	float SlowRatio = 0.0f;

	const TArray<UStatusEffect*>& ActiveEffects = StatusManager->GetActiveEffects();
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

	if (bHasBurn || bLastHasBurn != bHasBurn)
	{
		OnStatusEffectChanged.Broadcast(EDragonElement::Fire, bHasBurn, BurnRatio);
		bLastHasBurn = bHasBurn;
	}

	if (bHasSlow || bLastHasSlow != bHasSlow)
	{
		OnStatusEffectChanged.Broadcast(EDragonElement::Lightning, bHasSlow, SlowRatio);
		bLastHasSlow = bHasSlow;
	}
}
