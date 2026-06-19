#pragma once

#include "CoreMinimal.h"
#include "Interactables/BaseInteractable.h"
#include "BreakableObject.generated.h"

class ABasePickup;
class UParticleSystem;
class USoundBase;

UCLASS()
class PROJECTCRACKEDEGG_API ABreakableObject : public ABaseInteractable
{
	GENERATED_BODY()

public:
	ABreakableObject();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Breakable Settings")
	float MaxHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Breakable Settings")
	float CurrentHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Breakable Settings")
	class UDataTable* LootTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Breakable Effects")
	UParticleSystem* DestroyVFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Breakable Effects")
	USoundBase* DestroySound;

	UFUNCTION(BlueprintCallable, Category = "Breakable Functions")
	void BreakObject();

	UFUNCTION(BlueprintImplementableEvent, Category = "Breakable Events")
	void OnObjectBroken();

public:
	virtual void TakeElementalDamage_Implementation(EElementalType Element, float Damage, AActor* DamageInstigator) override;
};
