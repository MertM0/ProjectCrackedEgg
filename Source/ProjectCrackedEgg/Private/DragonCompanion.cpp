#include "DragonCompanion.h"
#include "AttributeComponent.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "TimerManager.h"

ADragonCompanion::ADragonCompanion()
{
	PrimaryActorTick.bCanEverTick = true;

	AttributeComponent = CreateDefaultSubobject<UAttributeComponent>(TEXT("AttributeComponent"));

	MeleeAttackThreshold = 400.0f;
	AttackCheckInterval = 2.0f;
	DragonElement = EDragonElement::Fire;

	bIsFlying = true;
	FlightDistanceThreshold = 500.0f;
	FlyingZOffset = 150.0f;
	GroundZOffset = -85.0f;
	FlightTransitionSpeed = 3.0f;

	MaxSpeedDistance = 1500.0f;
	MaxFlightSpeed = 1200.0f;
	MinWalkSpeed = 250.0f;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void ADragonCompanion::BeginPlay()
{
	Super::BeginPlay();

	DragonAIController = Cast<AAIController>(GetController());
	TargetPlayer = UGameplayStatics::GetPlayerPawn(this, 0);

	GetWorldTimerManager().SetTimer(TimerHandle_AttackCheck, this, &ADragonCompanion::CheckTargetAndAttack, AttackCheckInterval, true);

	if (AttributeComponent)
	{
		GetCharacterMovement()->MaxWalkSpeed = AttributeComponent->GetAttributeValue(EAttributeType::MovementSpeed);
	}

	PreviousYaw = GetActorRotation().Yaw;
}

void ADragonCompanion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float CurrentYaw = GetActorRotation().Yaw;
	float DeltaYaw = FMath::FindDeltaAngleDegrees(PreviousYaw, CurrentYaw);
	
	float TargetDirection = FMath::Clamp((DeltaYaw / DeltaTime) * 1.5f, -180.0f, 180.0f);
	
	FlightDirection = FMath::FInterpTo(FlightDirection, TargetDirection, DeltaTime, 3.0f);
	PreviousYaw = CurrentYaw;

	if (DragonAIController && IsValid(TargetPlayer))
	{
		float DistanceToPlayer = FVector::Dist(GetActorLocation(), TargetPlayer->GetActorLocation());

		float NormalizedDistance = FMath::Clamp((DistanceToPlayer - 200.0f) / (MaxSpeedDistance - 200.0f), 0.0f, 1.0f);
		float TargetSpeed = FMath::Lerp(MinWalkSpeed, MaxFlightSpeed, NormalizedDistance);
		
		GetCharacterMovement()->MaxWalkSpeed = FMath::FInterpTo(GetCharacterMovement()->MaxWalkSpeed, TargetSpeed, DeltaTime, 2.0f);

		DragonAIController->MoveToActor(TargetPlayer, 200.0f, false, true, true, 0, true);

		bIsFlying = (DistanceToPlayer > FlightDistanceThreshold);

		if (USkeletalMeshComponent* DragonMesh = GetMesh())
		{
			float TargetZOffset = bIsFlying ? FlyingZOffset : GroundZOffset;
			
			FVector CurrentRelativeLocation = DragonMesh->GetRelativeLocation();
			float NewZ = FMath::FInterpTo(CurrentRelativeLocation.Z, TargetZOffset, DeltaTime, FlightTransitionSpeed);
			
			CurrentRelativeLocation.Z = NewZ;
			DragonMesh->SetRelativeLocation(CurrentRelativeLocation);
		}
	}
}

void ADragonCompanion::CheckTargetAndAttack()
{
	if (!IsValid(TargetPlayer) || !DragonAIController)
	{
		return;
	}

	AActor* ClosestEnemy = nullptr;

	if (IsValid(ClosestEnemy))
	{
		float DistanceToEnemy = FVector::Dist(GetActorLocation(), ClosestEnemy->GetActorLocation());

		if (DistanceToEnemy <= MeleeAttackThreshold)
		{
			ExecuteMeleeAttack();
		}
		else
		{
			ExecuteRangedAttack();
		}
	}
}

void ADragonCompanion::ExecuteMeleeAttack()
{
}

void ADragonCompanion::ExecuteRangedAttack()
{
}