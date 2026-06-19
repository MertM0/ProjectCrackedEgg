#pragma once

#include "CoreMinimal.h"
#include "Interactables/BaseInteractable.h"
#include "InteractableDoor.generated.h"

UCLASS()
class PROJECTCRACKEDEGG_API AInteractableDoor : public ABaseInteractable
{
	GENERATED_BODY()

public:
	AInteractableDoor();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Settings")
	bool bRequiresKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Settings")
	FName RequiredKeyID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Settings")
	bool bRequiresLever;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Settings")
	FString LockedDoorWarningMessage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Settings")
	FString LeverDoorWarningMessage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door Settings")
	bool bIsOpen;

	virtual void OnInteract_Implementation(AActor* Interactor) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Door Events")
	void OnDoorOpened();

	UFUNCTION(BlueprintImplementableEvent, Category = "Door Events")
	void OnDoorLocked();

public:
	UFUNCTION(BlueprintCallable, Category = "Door Functions")
	void OpenDoor();
};
