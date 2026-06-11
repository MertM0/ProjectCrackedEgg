#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_DetectPlayer.generated.h"

UCLASS()
class PROJECTCRACKEDEGG_API UBTService_DetectPlayer : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_DetectPlayer();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

public:
	UPROPERTY(EditAnywhere, Category = "Node")
	struct FBlackboardKeySelector TargetActorKey;

	UPROPERTY(EditAnywhere, Category = "Node")
	float DetectionRadius;
};
