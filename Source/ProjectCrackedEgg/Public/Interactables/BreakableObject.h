#pragma once

#include "CoreMinimal.h"
#include "Interactables/BaseInteractable.h"
#include "BreakableObject.generated.h"

class ABasePickup;
class USoundBase;
class UNiagaraSystem;

UCLASS()
class PROJECTCRACKEDEGG_API ABreakableObject : public ABaseInteractable
{
	GENERATED_BODY()

public:
	ABreakableObject();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Breakable Settings")
	class UDataTable* LootTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Breakable Settings")
	float LootCollectionDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Breakable Settings")
	bool bOnlyBreakableByDragon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Breakable Settings")
	bool bOnlyBreakableByPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Breakable Settings")
	float BreakDelay;

	FTimerHandle TimerHandle_BreakDelay;

	void StartBreakSequence();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Breakable Effects")
	UNiagaraSystem* DestroyVFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Breakable Effects")
	USoundBase* DestroySound;

	UFUNCTION(BlueprintCallable, Category = "Breakable Functions")
	void BreakObject();

	UFUNCTION(BlueprintImplementableEvent, Category = "Breakable Events")
	void OnObjectBroken();

	UFUNCTION(BlueprintImplementableEvent, Category = "Breakable Events")
	void OnBreakStarted();

public:
	virtual void Interact_Implementation(AActor* Interactor) override;

	virtual void TakeElementalDamage_Implementation(EElementalType Element, float Damage, AActor* DamageInstigator) override;
};
