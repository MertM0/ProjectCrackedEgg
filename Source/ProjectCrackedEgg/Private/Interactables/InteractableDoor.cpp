#include "Interactables/InteractableDoor.h"
#include "Knight/AdvancedCharacter.h"

AInteractableDoor::AInteractableDoor()
{
	bRequiresKey = false;
	RequiredKeyID = NAME_None;
	bRequiresLever = false;
	bIsOpen = false;
	bSingleUse = false;

	LockedDoorWarningMessage = TEXT("Door is locked! Requires a key.");
	LeverDoorWarningMessage = TEXT("This door is controlled by a lever/mechanism.");
}

void AInteractableDoor::OnInteract_Implementation(AActor* Interactor)
{
	if (bIsOpen)
	{
		return;
	}

	AAdvancedCharacter* Character = Cast<AAdvancedCharacter>(Interactor);

	if (bRequiresLever)
	{
		if (Character)
		{
			Character->ShowInteractionWarning(LeverDoorWarningMessage);
		}
		OnDoorLocked();
		return;
	}

	if (bRequiresKey)
	{
		if (Character)
		{
			if (Character->HasKey(RequiredKeyID))
			{
				OpenDoor();
			}
			else
			{
				Character->ShowInteractionWarning(LockedDoorWarningMessage);
				OnDoorLocked();
			}
		}
		return;
	}

	OpenDoor();
}

void AInteractableDoor::OpenDoor()
{
	if (bIsOpen)
	{
		return;
	}

	bIsOpen = true;
	bCanInteract = false;
	OnDoorOpened();
}
