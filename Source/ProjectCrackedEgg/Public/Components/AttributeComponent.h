#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"

UENUM(BlueprintType)
enum class EAttributeType : uint8
{
	Health			UMETA(DisplayName = "Health"),
	MaxHealth		UMETA(DisplayName = "Max Health"),
	Mana			UMETA(DisplayName = "Mana"),
	MaxMana			UMETA(DisplayName = "Max Mana"),
	BaseDamage		UMETA(DisplayName = "Base Damage"),
	MovementSpeed	UMETA(DisplayName = "Movement Speed")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnAttributeChangedSignature, UAttributeComponent*, AttributeComp, EAttributeType, AttributeType, float, OldValue, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnLevelUpSignature, UAttributeComponent*, AttributeComp, int32, NewLevel, int32, AvailableStatPoints);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeathSignature, UAttributeComponent*, AttributeComp);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTCRACKEDEGG_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttributeComponent();

protected:
	virtual void BeginPlay() override;

	int32 CalculateRequiredXPForNextLevel() const;

	void SetAttributeValue(EAttributeType AttributeType, float NewValue);

public:	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes|Health")
	float MaxHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes|Health")
	float CurrentHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes|Mana")
	float MaxMana;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes|Mana")
	float CurrentMana;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes|Combat")
	float BaseDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes|Movement")
	float MovementSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes|Progression")
	int32 Level;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes|Progression")
	int32 CurrentXP;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes|Progression")
	int32 StatPoints;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes|Progression")
	int32 BaseXPRequirement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes|Progression")
	float XPMultiplierPerLevel;

	UPROPERTY(BlueprintAssignable, Category = "Attributes|Events")
	FOnAttributeChangedSignature OnAttributeChanged;

	UPROPERTY(BlueprintAssignable, Category = "Attributes|Events")
	FOnLevelUpSignature OnLevelUp;

	UPROPERTY(BlueprintAssignable, Category = "Attributes|Events")
	FOnDeathSignature OnDeath;

	UFUNCTION(BlueprintPure, Category = "Attributes")
	float GetAttributeValue(EAttributeType AttributeType) const;

	UFUNCTION(BlueprintCallable, Category = "Attributes|Health")
	void ApplyHealthChange(float Delta);

	UFUNCTION(BlueprintCallable, Category = "Attributes|Mana")
	void ApplyManaChange(float Delta);

	UFUNCTION(BlueprintCallable, Category = "Attributes|Progression")
	void AddXP(int32 XPToAdd);

	UFUNCTION(BlueprintCallable, Category = "Attributes|Progression")
	bool UpgradeStat(EAttributeType StatToUpgrade);

	UFUNCTION(BlueprintPure, Category = "Attributes")
	bool IsAlive() const;

	UFUNCTION(BlueprintPure, Category = "Attributes|Progression")
	float GetXPProgressRatio() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes|Progression")
	float HealthUpgradeAmount = 10.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes|Progression")
	float ManaUpgradeAmount = 5.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes|Progression")
	float DamageUpgradeAmount = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes|Progression")
	float MovementSpeedUpgradeAmount = 20.0f;
};
