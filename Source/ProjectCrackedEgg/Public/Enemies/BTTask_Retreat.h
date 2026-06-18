#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Retreat.generated.h"

struct FBTRetreatTaskMemory
{
	float StartTime;
};

UCLASS()
class PROJECTCRACKEDEGG_API UBTTask_Retreat : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_Retreat();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	virtual uint16 GetInstanceMemorySize() const override { return sizeof(FBTRetreatTaskMemory); }

	UPROPERTY(EditAnywhere, Category = "Node")
	struct FBlackboardKeySelector TargetActorKey;
};
