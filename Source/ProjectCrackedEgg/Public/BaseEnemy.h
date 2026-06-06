#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayInterface.h"
#include "BaseEnemy.generated.h"

class UAttributeComponent;

UCLASS()
class PROJECTCRACKEDEGG_API ABaseEnemy : public ACharacter, public IGameplayInterface
{
	GENERATED_BODY()

public:
	ABaseEnemy();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAttributeComponent* AttributeComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rewards")
	int32 RewardXP;

	UFUNCTION()
	void HandleDeath(UAttributeComponent* AttributeComp);

public:	
	virtual void Interact_Implementation(AActor* Interactor) override;

	virtual void TakeElementalDamage_Implementation(EDragonElement Element, float Damage) override;
};