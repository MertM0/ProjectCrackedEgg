#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/GameplayInterface.h"
#include "BaseInteractable.generated.h"

class USceneComponent;
class UStaticMeshComponent;

UCLASS()
class PROJECTCRACKEDEGG_API ABaseInteractable : public AActor, public IGameplayInterface
{
	GENERATED_BODY()
	
public:	
	ABaseInteractable();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bCanInteract;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bSingleUse;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	bool bHasBeenUsed;

public:
	virtual void Interact_Implementation(AActor* Interactor) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnInteract(AActor* Interactor);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetInteractable(bool bNewState);
};
