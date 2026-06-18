#include "BTTask_Retreat.h"
#include "AIController.h"
#include "Enemies/RangedEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavigationSystem.h"

UBTTask_Retreat::UBTTask_Retreat()
{
	NodeName = "Retreat";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_Retreat::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	ARangedEnemy* Enemy = Cast<ARangedEnemy>(AIController->GetPawn());
	if (!Enemy) return EBTNodeResult::Failed;

	AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!TargetActor) return EBTNodeResult::Failed;

	FVector EnemyLoc = Enemy->GetActorLocation();
	FVector TargetLoc = TargetActor->GetActorLocation();
	FVector RetreatDir = (EnemyLoc - TargetLoc).GetSafeNormal();
	RetreatDir.Z = 0.0f;
	FVector Destination = TargetLoc + RetreatDir * Enemy->GetPreferredDistance();

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(Enemy->GetWorld());
	if (NavSys)
	{
		FNavLocation ProjectedLoc;
		if (NavSys->ProjectPointToNavigation(Destination, ProjectedLoc, FVector(300.0f, 300.0f, 300.0f)))
		{
			Destination = ProjectedLoc.Location;
		}
	}

	Enemy->GetCharacterMovement()->MaxWalkSpeed = Enemy->GetRetreatSpeed();

	FBTRetreatTaskMemory* MyMemory = reinterpret_cast<FBTRetreatTaskMemory*>(NodeMemory);
	if (MyMemory)
	{
		MyMemory->StartTime = Enemy->GetWorld()->GetTimeSeconds();
	}

	EPathFollowingRequestResult::Type MoveResult = AIController->MoveToLocation(Destination, 5.0f, true, true, true, false, nullptr, true);
	if (MoveResult == EPathFollowingRequestResult::Failed)
	{
		return EBTNodeResult::Failed;
	}
	else if (MoveResult == EPathFollowingRequestResult::AlreadyAtGoal)
	{
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::InProgress;
}

void UBTTask_Retreat::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	FBTRetreatTaskMemory* MyMemory = reinterpret_cast<FBTRetreatTaskMemory*>(NodeMemory);
	if (MyMemory && AIController->GetPawn())
	{
		float ElapsedTime = AIController->GetPawn()->GetWorld()->GetTimeSeconds() - MyMemory->StartTime;
		if (ElapsedTime >= 1.5f)
		{
			AIController->StopMovement();
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			return;
		}
	}

	if (AIController->GetMoveStatus() == EPathFollowingStatus::Idle)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

EBTNodeResult::Type UBTTask_Retreat::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController)
	{
		AIController->StopMovement();
	}
	return Super::AbortTask(OwnerComp, NodeMemory);
}
