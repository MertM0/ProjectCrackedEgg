#include "BTTask_MeleeAttack.h"
#include "AIController.h"
#include "BaseEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Animation/AnimInstance.h"

UBTTask_MeleeAttack::UBTTask_MeleeAttack()
{
	NodeName = "Melee Attack";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_MeleeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	ABaseEnemy* Enemy = Cast<ABaseEnemy>(AIController->GetPawn());
	if (!Enemy) return EBTNodeResult::Failed;

	UAnimMontage* AttackMontage = Enemy->GetMeleeAttackMontage();
	if (AttackMontage)
	{
		Enemy->PlayAnimMontage(AttackMontage);
		
		return EBTNodeResult::InProgress;
	}

	return EBTNodeResult::Succeeded;
}

void UBTTask_MeleeAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController)
	{
		ABaseEnemy* Enemy = Cast<ABaseEnemy>(AIController->GetPawn());
		if (Enemy)
		{
			UAnimInstance* AnimInstance = Enemy->GetMesh()->GetAnimInstance();
			if (AnimInstance && !AnimInstance->Montage_IsPlaying(Enemy->GetMeleeAttackMontage()))
			{
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			}
		}
	}
}