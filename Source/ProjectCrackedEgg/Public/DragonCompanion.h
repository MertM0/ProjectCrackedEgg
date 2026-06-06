#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayInterface.h"
#include "DragonCompanion.generated.h"

class UAttributeComponent;
class AAIController;

UCLASS()
class PROJECTCRACKEDEGG_API ADragonCompanion : public ACharacter
{
	GENERATED_BODY()

public:
	ADragonCompanion();

protected:
	virtual void BeginPlay() override;

	FTimerHandle TimerHandle_AttackCheck;

	void CheckTargetAndAttack();

	void ExecuteMeleeAttack();

	void ExecuteRangedAttack();

public:	
	virtual void Tick(float DeltaTime) override;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAttributeComponent* AttributeComponent;

	UPROPERTY(BlueprintReadOnly, Category = "AI")
	AActor* TargetPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	EDragonElement DragonElement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float MeleeAttackThreshold;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float AttackCheckInterval;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visuals|Flight")
	bool bIsFlying;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals|Flight")
	float FlightDistanceThreshold;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals|Flight")
	float FlyingZOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals|Flight")
	float GroundZOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals|Flight")
	float FlightTransitionSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float MaxSpeedDistance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float MaxFlightSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float MinWalkSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visuals|Animation")
	float FlightDirection;

	float PreviousYaw;

	UPROPERTY()
	AAIController* DragonAIController;
};