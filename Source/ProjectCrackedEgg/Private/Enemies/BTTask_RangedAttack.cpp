#include "BTTask_RangedAttack.h"
#include "AIController.h"
#include "Enemies/RangedEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Animation/AnimInstance.h"

UBTTask_RangedAttack::UBTTask_RangedAttack()
{
	NodeName = "Ranged Attack";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_RangedAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	ARangedEnemy* Enemy = Cast<ARangedEnemy>(AIController->GetPawn());
	if (!Enemy) return EBTNodeResult::Failed;

	if (!Enemy->CanRangedAttack()) return EBTNodeResult::Failed;

	AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!TargetActor) return EBTNodeResult::Failed;

	FVector TargetLoc = TargetActor->GetActorLocation();
	FVector PawnLoc = Enemy->GetActorLocation();
	FVector Direction = (TargetLoc - PawnLoc).GetSafeNormal();
	Direction.Z = 0.0f;
	Enemy->SetActorRotation(Direction.Rotation());

	Enemy->SpawnProjectile(TargetActor);

	UAnimMontage* AttackMontage = Enemy->GetRangedAttackMontage();
	if (AttackMontage)
	{
		Enemy->PlayAnimMontage(AttackMontage);
		return EBTNodeResult::InProgress;
	}

	return EBTNodeResult::Succeeded;
}

void UBTTask_RangedAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController)
	{
		ARangedEnemy* Enemy = Cast<ARangedEnemy>(AIController->GetPawn());
		if (Enemy)
		{
			UAnimInstance* AnimInstance = Enemy->GetMesh()->GetAnimInstance();
			if (AnimInstance && !AnimInstance->Montage_IsPlaying(Enemy->GetRangedAttackMontage()))
			{
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			}
		}
	}
}

EBTNodeResult::Type UBTTask_RangedAttack::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController)
	{
		ARangedEnemy* Enemy = Cast<ARangedEnemy>(AIController->GetPawn());
		if (Enemy)
		{
			Enemy->StopAnimMontage();
		}
	}
	return Super::AbortTask(OwnerComp, NodeMemory);
}
