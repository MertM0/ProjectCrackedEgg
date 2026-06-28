#include "BaseEnemy.h"
#include "AttributeComponent.h"
#include "Interactables/BasePickup.h"
#include "AdvancedCharacter.h"
#include "DragonCompanion.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "CrackedEggGameMode.h"
#include "StatusEffectManagerComponent.h"
#include "StatusEffect_Burn.h"
#include "StatusEffect_Slow.h"
#include "UI/DamageTextActor.h"
#include "Components/WidgetComponent.h"
#include "UI/EnemyHealthBarWidget.h"

ABaseEnemy::ABaseEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	AttributeComponent = CreateDefaultSubobject<UAttributeComponent>(TEXT("AttributeComponent"));

	StatusEffectManager = CreateDefaultSubobject<UStatusEffectManagerComponent>(TEXT("StatusEffectManager"));
	
	ProximitySphere = CreateDefaultSubobject<USphereComponent>(TEXT("ProximitySphere"));
	ProximitySphere->SetupAttachment(RootComponent);
	ProximitySphere->SetSphereRadius(350.0f);
	ProximitySphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ProximitySphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	ProximitySphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	HealthBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidgetComponent"));
	HealthBarWidgetComponent->SetupAttachment(RootComponent);
	HealthBarWidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 110.0f));
	HealthBarWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	HealthBarWidgetComponent->SetDrawAtDesiredSize(true);

	RewardXP = 50;
	HitReactChance = 0.5f;
	bHideHealthBarWhenFullHealth = true;
	HealthBarVisibilityDuration = 5.0f;
	MaxHealthBarDisplayDistance = 1500.0f;

	LootSpawnOffset = FVector(0.0f, 0.0f, 0.0f);
	LootTable = nullptr;
	LootCollectionDelay = 0.5f;

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

	if (HealthBarWidgetComponent)
	{
		HealthBarWidgetComponent->SetVisibility(false);
		GetWorldTimerManager().SetTimer(TimerHandle_DistanceCheck, this, &ABaseEnemy::UpdateHealthBarVisibility, 0.2f, true);
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

void ABaseEnemy::UpdateHealthBarVisibility()
{
	if (AttributeComponent && !AttributeComponent->IsAlive())
	{
		return;
	}

	if (HealthBarWidgetComponent)
	{
		bool bShouldBeVisible = false;
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
		if (PlayerPawn)
		{
			float DistanceSq = FVector::DistSquared(GetActorLocation(), PlayerPawn->GetActorLocation());
			float MaxDistanceSq = FMath::Square(MaxHealthBarDisplayDistance);

			if (DistanceSq <= MaxDistanceSq)
			{
				bShouldBeVisible = true;
			}
		}

		if (GetWorldTimerManager().IsTimerActive(TimerHandle_HideHealthBar))
		{
			bShouldBeVisible = true;
		}

		if (bShouldBeVisible)
		{
			if (!HealthBarWidgetComponent->IsVisible())
			{
				HealthBarWidgetComponent->SetVisibility(true);
				SetActorTickEnabled(true);
			}

			UUserWidget* UserWidget = HealthBarWidgetComponent->GetUserWidgetObject();
			if (UserWidget)
			{
				UEnemyHealthBarWidget* HealthBarWidget = Cast<UEnemyHealthBarWidget>(UserWidget);
				if (HealthBarWidget)
				{
					HealthBarWidget->SetOwnerEnemy(this);
				}
			}
		}
		else
		{
			if (HealthBarWidgetComponent->IsVisible())
			{
				HealthBarWidgetComponent->SetVisibility(false);
				SetActorTickEnabled(false);
			}
		}
	}
}

void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HealthBarWidgetComponent && HealthBarWidgetComponent->IsVisible())
	{
		APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
		if (CameraManager)
		{
			FRotator CameraRotation = CameraManager->GetCameraRotation();
			FRotator TargetRotation = FRotator(-CameraRotation.Pitch, CameraRotation.Yaw + 180.0f, 0.0f);
			HealthBarWidgetComponent->SetWorldRotation(TargetRotation);
		}
	}
}

void ABaseEnemy::HideHealthBar()
{
	UpdateHealthBarVisibility();
}

void ABaseEnemy::TakeElementalDamage_Implementation(EElementalType Element, float Damage, AActor* DamageInstigator)
{
	if (AttributeComponent && AttributeComponent->IsAlive())
	{
		AttributeComponent->ApplyHealthChange(-Damage);

		if (HealthBarWidgetComponent)
		{
			GetWorldTimerManager().ClearTimer(TimerHandle_HideHealthBar);
			GetWorldTimerManager().SetTimer(TimerHandle_HideHealthBar, this, &ABaseEnemy::HideHealthBar, HealthBarVisibilityDuration, false);

			UpdateHealthBarVisibility();
		}

		if (DamageTextClass)
		{
			FVector SpawnLoc = GetActorLocation() + FVector(0.0f, 0.0f, 100.0f);
			ADamageTextActor* TextActor = GetWorld()->SpawnActor<ADamageTextActor>(DamageTextClass, SpawnLoc, FRotator::ZeroRotator);
			if (TextActor)
			{
				TextActor->InitializeDamageText(Damage, false);
			}
		}

		if (HitVFX)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitVFX, GetActorLocation(), GetActorRotation());
		}

		AActor* ActualTarget = nullptr;
		if (DamageInstigator)
		{
			if (DamageInstigator->IsA(AAdvancedCharacter::StaticClass()))
			{
				ActualTarget = DamageInstigator;
			}
			else if (DamageInstigator->IsA(ADragonCompanion::StaticClass()))
			{
				ActualTarget = UGameplayStatics::GetPlayerCharacter(this, 0);
			}
		}

		if (ActualTarget)
		{
			if (AAIController* AIController = Cast<AAIController>(GetController()))
			{
				if (UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent())
				{
					BlackboardComp->SetValueAsObject("TargetActor", ActualTarget);
				}
			}
		}

		if (AttributeComponent->IsAlive())
		{
			if (HitReactMontage && FMath::FRand() <= HitReactChance)
			{
				PlayAnimMontage(HitReactMontage);
			}

			if (StatusEffectManager && Element != EElementalType::None)
			{
				switch (Element)
				{
					case EElementalType::Fire:
						StatusEffectManager->ApplyStatusEffect(BurnEffectClass, DamageInstigator);
						break;
					case EElementalType::Lightning:
						StatusEffectManager->ApplyStatusEffect(SlowEffectClass, DamageInstigator);
						break;
					default:
						break;
				}
			}
		}
	}
}

void ABaseEnemy::HandleDeath(UAttributeComponent* AttributeComp)
{
	if (HealthBarWidgetComponent)
	{
		HealthBarWidgetComponent->SetVisibility(false);
	}
	GetWorldTimerManager().ClearTimer(TimerHandle_HideHealthBar);
	GetWorldTimerManager().ClearTimer(TimerHandle_DistanceCheck);

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

	if (ACrackedEggGameMode* GM = Cast<ACrackedEggGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GM->OnEnemyKilled(this);
	}

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

	SpawnLootDrops();

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

	if (bHit && HitResult.GetActor())
	{
		AActor* HitActor = HitResult.GetActor();
		if (HitActor->Implements<UGameplayInterface>() && !HitActor->IsA(ABaseEnemy::StaticClass()))
		{
			float Damage = AttributeComponent ? AttributeComponent->GetAttributeValue(EAttributeType::BaseDamage) : 15.0f;
			IGameplayInterface::Execute_TakeElementalDamage(HitActor, EElementalType::None, Damage, this);
		}
	}
}

void ABaseEnemy::SpawnLootDrops()
{
	if (LootTable)
	{
		TArray<FLootRow*> Rows;
		LootTable->GetAllRows<FLootRow>(TEXT(""), Rows);

		float TotalWeight = 0.0f;
		for (const FLootRow* Row : Rows)
		{
			if (Row)
			{
				TotalWeight += Row->Weight;
			}
		}

		if (TotalWeight > 0.0f)
		{
			float RandomRoll = FMath::FRandRange(0.0f, TotalWeight);
			float CurrentWeightSum = 0.0f;

			for (const FLootRow* Row : Rows)
			{
				if (Row)
				{
					CurrentWeightSum += Row->Weight;
					if (RandomRoll <= CurrentWeightSum)
					{
						if (Row->PickupClass)
						{
							FVector SpawnLoc = GetActorLocation() + LootSpawnOffset;
							FTransform SpawnTransform(FRotator::ZeroRotator, SpawnLoc);
							ABasePickup* NewPickup = GetWorld()->SpawnActorDeferred<ABasePickup>(
								Row->PickupClass,
								SpawnTransform,
								nullptr,
								nullptr,
								ESpawnActorCollisionHandlingMethod::AlwaysSpawn
							);
							if (NewPickup)
							{
								NewPickup->SetCollectionDelay(LootCollectionDelay);
								NewPickup->FinishSpawning(SpawnTransform);
							}
						}
						break;
					}
				}
			}
		}
	}
}

