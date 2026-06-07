#include "BTService_DetectPlayer.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AdvancedCharacter.h"
#include "Kismet/GameplayStatics.h"

UBTService_DetectPlayer::UBTService_DetectPlayer()
{
	NodeName = "Detect Player";
	DetectionRadius = 1200.0f;
	bNotifyTick = true;
}

void UBTService_DetectPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return;

	APawn* AIPawn = AIController->GetPawn();
	if (!AIPawn) return;

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (PlayerCharacter)
	{
		float Distance = FVector::Dist(AIPawn->GetActorLocation(), PlayerCharacter->GetActorLocation());
		if (Distance <= DetectionRadius)
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsObject(TargetActorKey.SelectedKeyName, PlayerCharacter);
		}
		else
		{
			OwnerComp.GetBlackboardComponent()->ClearValue(TargetActorKey.SelectedKeyName);
		}
	}
}