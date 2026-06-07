#include "BaseEnemy.h"
#include "AttributeComponent.h"
#include "AdvancedCharacter.h"
#include "DragonCompanion.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

ABaseEnemy::ABaseEnemy()
{
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	AttributeComponent = CreateDefaultSubobject<UAttributeComponent>(TEXT("AttributeComponent"));
	
	ProximitySphere = CreateDefaultSubobject<USphereComponent>(TEXT("ProximitySphere"));
	ProximitySphere->SetupAttachment(RootComponent);
	ProximitySphere->SetSphereRadius(350.0f);
	ProximitySphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ProximitySphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	ProximitySphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	RewardXP = 50;
	HitReactChance = 0.5f;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (AttributeComponent)
	{
		AttributeComponent->OnDeath.AddDynamic(this, &ABaseEnemy::HandleDeath);
		
		GetCharacterMovement()->MaxWalkSpeed = AttributeComponent->GetAttributeValue(EAttributeType::MovementSpeed);
	}
	
	if (ProximitySphere)
	{
		ProximitySphere->OnComponentBeginOverlap.AddDynamic(this, &ABaseEnemy::OnProximityOverlap);
	}
}

void ABaseEnemy::OnProximityOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this && OtherActor->IsA(AAdvancedCharacter::StaticClass()))
	{
		if (AAIController* AIController = Cast<AAIController>(GetController()))
		{
			if (UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent())
			{
				BlackboardComp->SetValueAsObject("TargetActor", OtherActor);
			}
		}
	}
}

void ABaseEnemy::Interact_Implementation(AActor* Interactor)
{
}

void ABaseEnemy::TakeElementalDamage_Implementation(EDragonElement Element, float Damage, AActor* DamageInstigator)
{
	if (AttributeComponent && AttributeComponent->IsAlive())
	{
		AttributeComponent->ApplyHealthChange(-Damage);

		if (HitVFX)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitVFX, GetActorLocation(), GetActorRotation());
		}

		if (DamageInstigator && DamageInstigator->IsA(AAdvancedCharacter::StaticClass()))
		{
			if (AAIController* AIController = Cast<AAIController>(GetController()))
			{
				if (UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent())
				{
					BlackboardComp->SetValueAsObject("TargetActor", DamageInstigator);
				}
			}
		}

		if (AttributeComponent->IsAlive())
		{
			if (HitReactMontage && FMath::FRand() <= HitReactChance)
			{
				PlayAnimMontage(HitReactMontage);
			}
		}
	}
}

void ABaseEnemy::HandleDeath(UAttributeComponent* AttributeComp)
{
	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		if (UBehaviorTreeComponent* BTComp = Cast<UBehaviorTreeComponent>(AIController->GetBrainComponent()))
		{
			BTComp->StopTree();
		}
		AIController->StopMovement();
		AIController->UnPossess();
	}

	StopAnimMontage();

	AActor* PlayerActor = UGameplayStatics::GetPlayerPawn(this, 0);
	if (AAdvancedCharacter* PlayerChar = Cast<AAdvancedCharacter>(PlayerActor))
	{
		if (UAttributeComponent* PlayerAttr = PlayerChar->FindComponentByClass<UAttributeComponent>())
		{
			PlayerAttr->AddXP(RewardXP);
		}
	}

	TArray<AActor*> FoundDragons;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADragonCompanion::StaticClass(), FoundDragons);
	if (FoundDragons.Num() > 0)
	{
		if (ADragonCompanion* DragonChar = Cast<ADragonCompanion>(FoundDragons[0]))
		{
			if (UAttributeComponent* DragonAttr = DragonChar->FindComponentByClass<UAttributeComponent>())
			{
				DragonAttr->AddXP(RewardXP);
			}
		}
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->DisableMovement();

	if (DeathMontage)
	{
		float MontageDuration = PlayAnimMontage(DeathMontage);
		if (MontageDuration > 0.0f)
		{
			float RagdollDelay = FMath::Max(0.1f, MontageDuration - 0.25f);
			FTimerHandle TimerHandle_Ragdoll;
			GetWorldTimerManager().SetTimer(TimerHandle_Ragdoll, this, &ABaseEnemy::TriggerRagdoll, RagdollDelay, false);
		}
		else
		{
			TriggerRagdoll();
		}
	}
	else
	{
		TriggerRagdoll();
	}
}

void ABaseEnemy::TriggerRagdoll()
{
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->bPauseAnims = true;
		MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		MeshComp->SetCollisionProfileName(TEXT("Ragdoll"));
		MeshComp->SetSimulatePhysics(true);
	}

	SetLifeSpan(5.0f);
}

void ABaseEnemy::PerformMeleeStrike()
{
	FVector StartLocation = GetActorLocation();
	FVector ForwardVector = GetActorForwardVector();
	FVector EndLocation = StartLocation + (ForwardVector * 150.0f);

	FCollisionShape BoxShape = FCollisionShape::MakeBox(FVector(75.0f, 75.0f, 75.0f));
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	FHitResult HitResult;
	bool bHit = GetWorld()->SweepSingleByChannel(HitResult, StartLocation, EndLocation, GetActorQuat(), ECC_Pawn, BoxShape, QueryParams);

	DrawDebugBox(GetWorld(), StartLocation + (ForwardVector * 75.0f), FVector(75.0f, 75.0f, 75.0f), GetActorQuat(), bHit ? FColor::Green : FColor::Red, false, 2.0f);

	if (bHit && HitResult.GetActor())
	{
		AActor* HitActor = HitResult.GetActor();
		if (HitActor->Implements<UGameplayInterface>())
		{
			float Damage = AttributeComponent ? AttributeComponent->GetAttributeValue(EAttributeType::BaseDamage) : 15.0f;
			IGameplayInterface::Execute_TakeElementalDamage(HitActor, EDragonElement::None, Damage, this);
		}
	}
}