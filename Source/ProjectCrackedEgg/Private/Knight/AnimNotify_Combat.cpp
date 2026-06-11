#include "AnimNotify_Combat.h"
#include "AdvancedCharacter.h"
#include "Components/SkeletalMeshComponent.h"

void UAnimNotify_StartSweep::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (MeshComp && MeshComp->GetOwner())
	{
		if (AAdvancedCharacter* PlayerChar = Cast<AAdvancedCharacter>(MeshComp->GetOwner()))
		{
			PlayerChar->StartWeaponSweep();
		}
	}
}

void UAnimNotify_StartHeavySweep::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (MeshComp && MeshComp->GetOwner())
	{
		if (AAdvancedCharacter* PlayerChar = Cast<AAdvancedCharacter>(MeshComp->GetOwner()))
		{
			PlayerChar->PerformHeavySweep();
		}
	}
}

void UAnimNotify_StopSweep::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (MeshComp && MeshComp->GetOwner())
	{
		if (AAdvancedCharacter* PlayerChar = Cast<AAdvancedCharacter>(MeshComp->GetOwner()))
		{
			PlayerChar->StopWeaponSweep();
		}
	}
}

void UAnimNotify_SaveCombo::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (MeshComp && MeshComp->GetOwner())
	{
		if (AAdvancedCharacter* PlayerChar = Cast<AAdvancedCharacter>(MeshComp->GetOwner()))
		{
			PlayerChar->SaveCombo();
		}
	}
}

void UAnimNotify_ResetCombo::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (MeshComp && MeshComp->GetOwner())
	{
		if (AAdvancedCharacter* PlayerChar = Cast<AAdvancedCharacter>(MeshComp->GetOwner()))
		{
			PlayerChar->ResetCombo();
		}
	}
}
