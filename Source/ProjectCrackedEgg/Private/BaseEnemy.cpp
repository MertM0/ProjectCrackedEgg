#include "BaseEnemy.h"
#include "AttributeComponent.h"
#include "AdvancedCharacter.h"
#include "DragonCompanion.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ABaseEnemy::ABaseEnemy()
{
	PrimaryActorTick.bCanEverTick = false;

	AttributeComponent = CreateDefaultSubobject<UAttributeComponent>(TEXT("AttributeComponent"));

	RewardXP = 50;

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
}

void ABaseEnemy::Interact_Implementation(AActor* Interactor)
{
}

void ABaseEnemy::TakeElementalDamage_Implementation(EDragonElement Element, float Damage)
{
	if (AttributeComponent && AttributeComponent->IsAlive())
	{
		AttributeComponent->ApplyHealthChange(-Damage);

		if (Element == EDragonElement::Fire)
		{
		}
		else if (Element == EDragonElement::Lightning)
		{
		}
	}
}

void ABaseEnemy::HandleDeath(UAttributeComponent* AttributeComp)
{
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
	
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		MeshComp->SetCollisionProfileName(TEXT("Ragdoll"));
		MeshComp->SetSimulatePhysics(true);
	}

	SetLifeSpan(5.0f);
}