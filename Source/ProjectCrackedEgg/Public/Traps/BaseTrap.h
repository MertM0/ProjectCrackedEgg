#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayInterface.h"
#include "BaseTrap.generated.h"

class UBoxComponent;

UCLASS()
class PROJECTCRACKEDEGG_API ABaseTrap : public AActor
{
	GENERATED_BODY()
	
public:	
	ABaseTrap();

protected:
	virtual void BeginPlay() override;

public:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trap|Components")
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trap|Components")
	UBoxComponent* TriggerVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trap|Settings")
	float BaseDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trap|Settings")
	EDragonElement TrapElement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trap|Settings")
	float DamageCooldown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trap|Settings")
	bool bAffectsEnemies;

	UFUNCTION(BlueprintCallable, Category = "Trap")
	bool DealDamage(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Trap")
	void ResetCooldownForActor(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "Trap")
	void ResetAllCooldowns();

protected:
	UPROPERTY()
	TMap<AActor*, float> LastDamageTimeMap;
};
