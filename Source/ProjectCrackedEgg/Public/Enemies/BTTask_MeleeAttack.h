#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MeleeAttack.generated.h"

UCLASS()
class PROJECTCRACKEDEGG_API UBTTask_MeleeAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_MeleeAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
