#include "DragonCompanion.h"
#include "AttributeComponent.h"
#include "BaseProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Engine/OverlapResult.h"
#include "Engine/World.h"
#include "BaseEnemy.h"
#include "AdvancedCharacter.h"

ADragonCompanion::ADragonCompanion()
{
	PrimaryActorTick.bCanEverTick = true;

	AttributeComponent = CreateDefaultSubobject<UAttributeComponent>(TEXT("AttributeComponent"));

	DragonElement = EElementalType::Fire;
	AirRangedMontage = nullptr;
	ProjectileClass = nullptr;

	MeleeHitboxSize = FVector(75.0f, 75.0f, 75.0f);
	ProjectileSpawnSocket = TEXT("ProjectileSocket");

	MeleeAttackRange = 180.0f;
	RangedAttackDistance = 600.0f;
	MeleeCooldown = 0.5f;
	RangedCooldown = 2.5f;
	LastMeleeAttackTime = 0.0f;
	LastRangedAttackTime = 0.0f;
	CurrentAttackTarget = nullptr;
	MaxPlayerTetherDistance = 1500.0f;
	bIsInCombat = false;

	bIsFlying = true;
	FlightDistanceThreshold = 500.0f;
	FlyingZOffset = 150.0f;
	GroundZOffset = -85.0f;
	FlightTransitionSpeed = 3.0f;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	AIC = nullptr;
	CurrentMoveTarget = nullptr;

	PriorityTarget = nullptr;
}

void ADragonCompanion::BeginPlay()
{
	Super::BeginPlay();

	TargetPlayer = UGameplayStatics::GetPlayerPawn(this, 0);

	if (AttributeComponent)
	{
		GetCharacterMovement()->MaxWalkSpeed = AttributeComponent->GetAttributeValue(EAttributeType::MovementSpeed);
		AttributeComponent->OnLevelUp.AddDynamic(this, &ADragonCompanion::HandleLevelUp);
	}

	PreviousYaw = GetActorRotation().Yaw;

	if (SpawnMontage)
	{
		PlayAnimMontage(SpawnMontage);
	}

	GetWorldTimerManager().SetTimer(TimerHandle_DecisionMaking, this, &ADragonCompanion::UpdateAI, 0.25f, true);
}

void ADragonCompanion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float CurrentYaw = GetActorRotation().Yaw;
	float DeltaYaw = FMath::FindDeltaAngleDegrees(PreviousYaw, CurrentYaw);
	
	float TargetDirection = FMath::Clamp((DeltaYaw / DeltaTime) * 1.5f, -180.0f, 180.0f);
	
	FlightDirection = FMath::FInterpTo(FlightDirection, TargetDirection, DeltaTime, 3.0f);
	PreviousYaw = CurrentYaw;

	if (IsValid(CurrentAttackTarget))
	{
		UAttributeComponent* TargetAttr = CurrentAttackTarget->FindComponentByClass<UAttributeComponent>();
		if (TargetAttr && TargetAttr->IsAlive())
		{
			FVector TargetLocation = CurrentAttackTarget->GetActorLocation();
			FVector CurrentLocation = GetActorLocation();
			FRotator TargetRotation = (TargetLocation - CurrentLocation).Rotation();
			TargetRotation.Pitch = 0.0f;
			TargetRotation.Roll = 0.0f;

			FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 10.0f);
			SetActorRotation(NewRotation);
		}
		else
		{
			CurrentAttackTarget = nullptr;
		}
	}

	if (IsValid(TargetPlayer))
	{
		float DistanceToPlayer = FVector::Dist(GetActorLocation(), TargetPlayer->GetActorLocation());
		if (!bIsInCombat)
		{
			bIsFlying = (DistanceToPlayer > FlightDistanceThreshold);
		}

		if (AttributeComponent && AttributeComponent->Level < 2)
		{
			bIsFlying = false;
		}

		UCharacterMovementComponent* MoveComp = GetCharacterMovement();
		if (MoveComp)
		{
			float NormalizedDistance = FMath::Clamp((DistanceToPlayer - 200.0f) / (1500.0f - 200.0f), 0.0f, 1.0f);
			float TargetSpeed = FMath::Lerp(250.0f, 1200.0f, NormalizedDistance);
			MoveComp->MaxWalkSpeed = FMath::FInterpTo(MoveComp->MaxWalkSpeed, TargetSpeed, DeltaTime, 2.0f);
		}

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

void ADragonCompanion::UpdateAI()
{
	if (!AIC)
	{
		AIC = Cast<AAIController>(GetController());
		if (!AIC) return;
	}

	if (IsValid(PriorityTarget))
	{
		bIsInCombat = true;
		bIsFlying = true;
		float DistToTarget = FVector::Dist(GetActorLocation(), PriorityTarget->GetActorLocation());

		if (DistToTarget > RangedAttackDistance)
		{
			if (AIC && (AIC->GetMoveStatus() == EPathFollowingStatus::Idle || CurrentMoveTarget != PriorityTarget))
			{
				FVector DirToMe = (GetActorLocation() - PriorityTarget->GetActorLocation()).GetSafeNormal2D();
				if (DirToMe.IsNearlyZero())
				{
					DirToMe = GetActorForwardVector();
				}
				FVector MoveGoal = PriorityTarget->GetActorLocation() + DirToMe * (RangedAttackDistance - 50.0f);
				AIC->MoveToLocation(MoveGoal, 50.0f, false, true, true, true, nullptr, true);
				CurrentMoveTarget = PriorityTarget;
			}
			return;
		}

		if (IsPlayingAttackMontage())
		{
			return;
		}

		if (AIC)
		{
			AIC->StopMovement();
		}
		CurrentMoveTarget = nullptr;

		FRotator TargetRotation = (PriorityTarget->GetActorLocation() - GetActorLocation()).Rotation();
		TargetRotation.Pitch = 0.0f;
		TargetRotation.Roll = 0.0f;
		SetActorRotation(TargetRotation);

		if (AirRangedMontage)
		{
			PlayAnimMontage(AirRangedMontage);
		}

		SpawnProjectile(PriorityTarget);
		CurrentAttackTarget = PriorityTarget;
		LastRangedAttackTime = GetWorld()->GetTimeSeconds();
		PriorityTarget = nullptr;
		return;
	}

	if (IsPlayingAttackMontage())
	{
		return;
	}

	if (IsValid(TargetPlayer))
	{
		float DistanceToPlayer = FVector::Dist(GetActorLocation(), TargetPlayer->GetActorLocation());
		if (DistanceToPlayer > MaxPlayerTetherDistance)
		{
			if (bIsInCombat || CurrentAttackTarget != nullptr || CurrentMoveTarget != TargetPlayer)
			{
				AIC->StopMovement();
			}
			CurrentAttackTarget = nullptr;
			bIsInCombat = false;
			if (AIC->GetMoveStatus() == EPathFollowingStatus::Idle || CurrentMoveTarget != TargetPlayer)
			{
				AIC->MoveToActor(TargetPlayer, 200.0f, false, true, true, 0, true);
				CurrentMoveTarget = TargetPlayer;
			}
			return;
		}
	}

	CurrentAttackTarget = nullptr;

	ABaseEnemy* ClosestEnemy = nullptr;
	float ClosestDistance = 1500.0f;
	FVector DragonLocation = GetActorLocation();

	AAdvancedCharacter* Player = Cast<AAdvancedCharacter>(TargetPlayer);
	if (Player && IsValid(Player->LastAttackedEnemy))
	{
		UAttributeComponent* TargetAttr = Player->LastAttackedEnemy->FindComponentByClass<UAttributeComponent>();
		if (TargetAttr && TargetAttr->IsAlive())
		{
			float Distance = FVector::Dist(DragonLocation, Player->LastAttackedEnemy->GetActorLocation());
			if (Distance <= 1500.0f)
			{
				ClosestEnemy = Cast<ABaseEnemy>(Player->LastAttackedEnemy);
				ClosestDistance = Distance;
			}
		}
	}

	if (!ClosestEnemy)
	{
		TArray<FOverlapResult> OverlapResults;
		FCollisionShape SearchSphere = FCollisionShape::MakeSphere(ClosestDistance);
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);

		bool bHasOverlap = GetWorld()->OverlapMultiByChannel(
			OverlapResults,
			DragonLocation,
			FQuat::Identity,
			ECC_Pawn,
			SearchSphere,
			QueryParams
		);

		if (bHasOverlap)
		{
			for (const FOverlapResult& Result : OverlapResults)
			{
				ABaseEnemy* Enemy = Cast<ABaseEnemy>(Result.GetActor());
				if (IsValid(Enemy))
				{
					UAttributeComponent* Attr = Enemy->FindComponentByClass<UAttributeComponent>();
					if (Attr && Attr->IsAlive())
					{
						float Distance = FVector::Dist(DragonLocation, Enemy->GetActorLocation());
						if (Distance < ClosestDistance)
						{
							ClosestDistance = Distance;
							ClosestEnemy = Enemy;
						}
					}
				}
			}
		}
	}

	if (ClosestEnemy)
	{
		bIsInCombat = true;
		float CurrentTime = GetWorld()->GetTimeSeconds();

		bool bCanUseRanged = AttributeComponent && AttributeComponent->Level >= 2;

		if (bCanUseRanged && CurrentTime - LastRangedAttackTime >= RangedCooldown)
		{
			bIsFlying = true;

			if (ClosestDistance < RangedAttackDistance - 100.0f)
			{
				FVector AwayDir = (GetActorLocation() - ClosestEnemy->GetActorLocation()).GetSafeNormal2D();
				if (AwayDir.IsNearlyZero())
				{
					AwayDir = -GetActorForwardVector();
				}
				FVector TargetLoc = ClosestEnemy->GetActorLocation() + AwayDir * RangedAttackDistance;
				AIC->MoveToLocation(TargetLoc, 50.0f, false, true, true, true, 0, true);
				CurrentMoveTarget = nullptr;
			}
			else
			{
				AIC->StopMovement();
				CurrentMoveTarget = nullptr;

				FRotator TargetRotation = (ClosestEnemy->GetActorLocation() - GetActorLocation()).Rotation();
				TargetRotation.Pitch = 0.0f;
				TargetRotation.Roll = 0.0f;
				SetActorRotation(TargetRotation);

				if (AirRangedMontage)
				{
					PlayAnimMontage(AirRangedMontage);
				}
				
				SpawnProjectile(ClosestEnemy);
				CurrentAttackTarget = ClosestEnemy;
				LastRangedAttackTime = CurrentTime;
			}
		}
		else
		{
			if (ClosestDistance > MeleeAttackRange + 150.0f)
			{
				bIsFlying = AttributeComponent && AttributeComponent->Level >= 2;

				if (AIC->GetMoveStatus() == EPathFollowingStatus::Idle || CurrentMoveTarget != ClosestEnemy)
				{
					AIC->MoveToActor(ClosestEnemy, MeleeAttackRange - 20.0f, false, true, true, 0, true);
					CurrentMoveTarget = ClosestEnemy;
				}
			}
			else
			{
				bIsFlying = false;

				if (ClosestDistance > MeleeAttackRange)
				{
					if (AIC->GetMoveStatus() == EPathFollowingStatus::Idle || CurrentMoveTarget != ClosestEnemy)
					{
						AIC->MoveToActor(ClosestEnemy, MeleeAttackRange - 20.0f, false, true, true, 0, true);
						CurrentMoveTarget = ClosestEnemy;
					}
				}
				else
				{
					AIC->StopMovement();
					CurrentMoveTarget = nullptr;

					FRotator TargetRotation = (ClosestEnemy->GetActorLocation() - GetActorLocation()).Rotation();
					TargetRotation.Pitch = 0.0f;
					TargetRotation.Roll = 0.0f;
					SetActorRotation(TargetRotation);

					if (CurrentTime - LastMeleeAttackTime >= MeleeCooldown)
					{
						UAnimMontage* SelectedMontage = nullptr;
						if (GroundMeleeMontages.Num() > 0)
						{
							int32 Index = FMath::RandRange(0, GroundMeleeMontages.Num() - 1);
							SelectedMontage = GroundMeleeMontages[Index];
						}

						if (SelectedMontage)
						{
							PlayAnimMontage(SelectedMontage);
							CurrentAttackTarget = ClosestEnemy;
							LastMeleeAttackTime = CurrentTime;
						}
					}
				}
			}
		}
	}
	else
	{
		bIsInCombat = false;

		if (IsValid(TargetPlayer))
		{
			float DistanceToPlayer = FVector::Dist(GetActorLocation(), TargetPlayer->GetActorLocation());
			if (DistanceToPlayer > 200.0f)
			{
				if (AIC->GetMoveStatus() == EPathFollowingStatus::Idle || CurrentMoveTarget != TargetPlayer)
				{
					AIC->MoveToActor(TargetPlayer, 200.0f, false, true, true, 0, true);
					CurrentMoveTarget = TargetPlayer;
				}
			}
			else
			{
				AIC->StopMovement();
				CurrentMoveTarget = nullptr;
			}
		}
	}
}

bool ADragonCompanion::IsPlayingAttackMontage() const
{
	UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
	if (!AnimInstance) return false;

	return AnimInstance->IsAnyMontagePlaying();
}

void ADragonCompanion::PerformMeleeAttack()
{
	FVector StartLocation = GetActorLocation();
	FVector ForwardVector = GetActorForwardVector();
	FVector EndLocation = StartLocation + (ForwardVector * 150.0f);

	FCollisionShape BoxShape = FCollisionShape::MakeBox(MeleeHitboxSize);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	if (TargetPlayer)
	{
		QueryParams.AddIgnoredActor(TargetPlayer);
	}

	FHitResult HitResult;
	bool bHit = GetWorld()->SweepSingleByChannel(HitResult, StartLocation, EndLocation, GetActorQuat(), ECC_Pawn, BoxShape, QueryParams);

	DrawDebugBox(GetWorld(), StartLocation + (ForwardVector * 75.0f), MeleeHitboxSize, GetActorQuat(), bHit ? FColor::Green : FColor::Red, false, 2.0f);

	if (bHit && HitResult.GetActor())
	{
		AActor* HitActor = HitResult.GetActor();
		if (HitActor != TargetPlayer && HitActor->Implements<UGameplayInterface>())
		{
			float Damage = AttributeComponent ? AttributeComponent->GetAttributeValue(EAttributeType::BaseDamage) : 15.0f;
			IGameplayInterface::Execute_TakeElementalDamage(HitActor, EElementalType::None, Damage, this);
		}
	}
}

void ADragonCompanion::SpawnProjectile(AActor* Target)
{
	if (!ProjectileClass || !Target) return;

	USkeletalMeshComponent* DragonMesh = GetMesh();
	FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
	FRotator SpawnRotation = GetActorRotation();

	if (DragonMesh && DragonMesh->DoesSocketExist(ProjectileSpawnSocket))
	{
		SpawnLocation = DragonMesh->GetSocketLocation(ProjectileSpawnSocket);
	}

	FVector TargetLocation = Target->GetActorLocation();
	FVector Direction = (TargetLocation - SpawnLocation).GetSafeNormal();
	SpawnRotation = Direction.Rotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ABaseProjectile* Projectile = GetWorld()->SpawnActor<ABaseProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);
	if (Projectile)
	{
		float Damage = AttributeComponent ? AttributeComponent->GetAttributeValue(EAttributeType::BaseDamage) : 15.0f;
		Projectile->InitializeProjectile(DragonElement, Damage, this, Target);

		if (Projectile->GetMovementDelay() > 0.0f && DragonMesh && DragonMesh->DoesSocketExist(ProjectileSpawnSocket))
		{
			Projectile->AttachToComponent(DragonMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, ProjectileSpawnSocket);
		}
	}
}

void ADragonCompanion::HandleLevelUp(UAttributeComponent* AttributeComp, int32 NewLevel, int32 AvailableStatPoints)
{
	if (LevelUpVFX)
	{
		UGameplayStatics::SpawnEmitterAttached(LevelUpVFX, GetMesh(), NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, true);
	}
}

void ADragonCompanion::SetDragonElement(EElementalType NewElement)
{
	DragonElement = NewElement;
}

void ADragonCompanion::SetPriorityTarget(AActor* Target)
{
	if (Target && AttributeComponent && AttributeComponent->Level < 2)
	{
		return;
	}
	PriorityTarget = Target;
}

void ADragonCompanion::ClearPriorityTarget()
{
	PriorityTarget = nullptr;
}
