#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VictoryPortal.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class PROJECTCRACKEDEGG_API AVictoryPortal : public AActor
{
	GENERATED_BODY()

public:
	AVictoryPortal();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* RootScene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* OverlapBox;

	UPROPERTY(BlueprintReadOnly, Category = "Portal")
	bool bIsActive;

public:
	UFUNCTION(BlueprintCallable, Category = "Portal")
	void SetIsActive(bool bActive);

	UFUNCTION(BlueprintPure, Category = "Portal")
	bool GetIsActive() const { return bIsActive; }

	UFUNCTION(BlueprintImplementableEvent, Category = "Portal")
	void OnPortalActivated();

	UFUNCTION(BlueprintImplementableEvent, Category = "Portal")
	void OnPortalDeactivated();

protected:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};

