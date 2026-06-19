#include "Interactables/BaseInteractable.h"
#include "Components/StaticMeshComponent.h"

ABaseInteractable::ABaseInteractable()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(SceneRoot);

	bCanInteract = true;
	bSingleUse = true;
	bHasBeenUsed = false;
}

void ABaseInteractable::Interact_Implementation(AActor* Interactor)
{
	if (!bCanInteract)
	{
		return;
	}

	if (bSingleUse && bHasBeenUsed)
	{
		return;
	}

	bHasBeenUsed = true;
	OnInteract(Interactor);
}

void ABaseInteractable::OnInteract_Implementation(AActor* Interactor)
{
}

void ABaseInteractable::SetInteractable(bool bNewState)
{
	bCanInteract = bNewState;
}
