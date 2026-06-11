#include "AnimNotify_DragonCombat.h"
#include "DragonCompanion.h"
#include "Components/SkeletalMeshComponent.h"

void UAnimNotify_DragonMeleeStrike::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp)
	{
		AActor* Owner = MeshComp->GetOwner();
		if (Owner)
		{
			ADragonCompanion* Dragon = Cast<ADragonCompanion>(Owner);
			if (Dragon)
			{
				Dragon->PerformMeleeAttack();
			}
		}
	}
}

