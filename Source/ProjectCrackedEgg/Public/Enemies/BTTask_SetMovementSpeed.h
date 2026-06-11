#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SetMovementSpeed.generated.h"

UCLASS()
class PROJECTCRACKEDEGG_API UBTTask_SetMovementSpeed : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_SetMovementSpeed();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	UPROPERTY(EditAnywhere, Category = "Node")
	float NewSpeed;
};
