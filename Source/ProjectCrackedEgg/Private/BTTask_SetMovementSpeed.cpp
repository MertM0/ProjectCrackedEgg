#include "BTTask_SetMovementSpeed.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTTask_SetMovementSpeed::UBTTask_SetMovementSpeed()
{
	NodeName = "Set Movement Speed";
	NewSpeed = 600.0f;
}

EBTNodeResult::Type UBTTask_SetMovementSpeed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	ACharacter* Character = Cast<ACharacter>(AIController->GetPawn());
	if (!Character) return EBTNodeResult::Failed;

	Character->GetCharacterMovement()->MaxWalkSpeed = NewSpeed;

	return EBTNodeResult::Succeeded;
}