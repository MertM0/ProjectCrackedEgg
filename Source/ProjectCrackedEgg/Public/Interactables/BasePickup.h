#pragma once

#include "CoreMinimal.h"
#include "Interactables/BaseInteractable.h"
#include "Engine/DataTable.h"
#include "BasePickup.generated.h"

USTRUCT(BlueprintType)
struct FLootRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class ABasePickup> PickupClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0"))
	float Weight;
};

class USphereComponent;
class USoundBase;
class UParticleSystem;
class URotatingMovementComponent;
class UInterpToMovementComponent;

UCLASS()
class PROJECTCRACKEDEGG_API ABasePickup : public ABaseInteractable
{
	GENERATED_BODY()

public:
	ABasePickup();

	void SetCollectionDelay(float InDelay) { CollectionDelay = InDelay; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* PickupRadius;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	URotatingMovementComponent* RotatingMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UInterpToMovementComponent* InterpMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	USoundBase* PickupSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UParticleSystem* PickupVFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Settings")
	float CollectionDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Settings")
	float CollectAnimDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Settings")
	float GrowDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Settings")
	float MaxScaleMultiplier;

	FTimerHandle TimerHandle_CollectionDelay;
	FTimerHandle TimerHandle_CollectAnim;

	FVector InitialScale;
	float CollectAnimTimeElapsed;

	void EnableCollection();
	void StartCollectionAnimation();
	void UpdateCollectionAnimation();

	UFUNCTION()
	virtual void OnPickupOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup")
	void ApplyPickupEffect(AActor* Target);

	UFUNCTION(BlueprintNativeEvent, Category = "Pickup")
	void OnPickupCollected();
};
