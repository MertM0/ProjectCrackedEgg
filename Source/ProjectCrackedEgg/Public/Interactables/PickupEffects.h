#pragma once

#include "CoreMinimal.h"
#include "Interactables/BasePickup.h"
#include "PickupEffects.generated.h"

UCLASS()
class PROJECTCRACKEDEGG_API APickup_HealthRestore : public ABasePickup
{
	GENERATED_BODY()

public:
	APickup_HealthRestore();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Restore")
	float HealthAmount;

	virtual void ApplyPickupEffect_Implementation(AActor* Target) override;
};

UCLASS()
class PROJECTCRACKEDEGG_API APickup_XPGrant : public ABasePickup
{
	GENERATED_BODY()

public:
	APickup_XPGrant();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	int32 XPAmount;

	virtual void ApplyPickupEffect_Implementation(AActor* Target) override;
};

UCLASS()
class PROJECTCRACKEDEGG_API APickup_DamageBoost : public ABasePickup
{
	GENERATED_BODY()

public:
	APickup_DamageBoost();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
	float DamageMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
	float Duration;

	virtual void ApplyPickupEffect_Implementation(AActor* Target) override;
};

UCLASS()
class PROJECTCRACKEDEGG_API APickup_StaminaReduction : public ABasePickup
{
	GENERATED_BODY()

public:
	APickup_StaminaReduction();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
	float StaminaCostMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
	float Duration;

	virtual void ApplyPickupEffect_Implementation(AActor* Target) override;
};

UCLASS()
class PROJECTCRACKEDEGG_API APickup_StaminaFull : public ABasePickup
{
	GENERATED_BODY()

public:
	APickup_StaminaFull();

protected:
	virtual void ApplyPickupEffect_Implementation(AActor* Target) override;
};

UCLASS()
class PROJECTCRACKEDEGG_API APickup_Key : public ABasePickup
{
	GENERATED_BODY()

public:
	APickup_Key();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key")
	FName KeyID;

	virtual void ApplyPickupEffect_Implementation(AActor* Target) override;
};
