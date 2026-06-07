#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindRandomPatrol.generated.h"

UCLASS()
class PROJECTCRACKEDEGG_API UBTTask_FindRandomPatrol : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FindRandomPatrol();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	UPROPERTY(EditAnywhere, Category = "Node")
	struct FBlackboardKeySelector PatrolLocationKey;

	UPROPERTY(EditAnywhere, Category = "Node")
	float PatrolRadius;
};