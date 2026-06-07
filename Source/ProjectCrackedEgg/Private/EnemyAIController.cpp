#include "EnemyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BaseEnemy.h"
#include "AdvancedCharacter.h"

AEnemyAIController::AEnemyAIController()
{
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));

	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	SightConfig->SightRadius = 1200.0f;
	SightConfig->LoseSightRadius = 1500.0f;
	SightConfig->PeripheralVisionAngleDegrees = 60.0f;
	SightConfig->SetMaxAge(5.0f);
	
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
	AIPerceptionComponent->ConfigureSense(*SightConfig);
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	if (AIPerceptionComponent)
	{
		AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnTargetDetected);
	}
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (ABaseEnemy* Enemy = Cast<ABaseEnemy>(InPawn))
	{
		if (UBehaviorTree* BT = Enemy->GetBehaviorTree())
		{
			BlackboardComponent->InitializeBlackboard(*(BT->BlackboardAsset));
			BehaviorTreeComponent->StartTree(*BT);
		}
	}
}

void AEnemyAIController::OnTargetDetected(AActor* Actor, FAIStimulus Stimulus)
{
	if (AAdvancedCharacter* Player = Cast<AAdvancedCharacter>(Actor))
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			GetWorldTimerManager().ClearTimer(TimerHandle_TargetMemory);
			BlackboardComponent->SetValueAsObject("TargetActor", Player);
		}
		else
		{
			GetWorldTimerManager().SetTimer(TimerHandle_TargetMemory, this, &AEnemyAIController::ForgetTarget, 5.0f, false);
		}
	}
}

void AEnemyAIController::ForgetTarget()
{
	if (BlackboardComponent)
	{
		BlackboardComponent->ClearValue("TargetActor");
	}
}