#include "Interactables/BasePickup.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Knight/AdvancedCharacter.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Components/InterpToMovementComponent.h"

ABasePickup::ABasePickup()
{
	PrimaryActorTick.bCanEverTick = false;

	bSingleUse = true;
	CollectionDelay = 1.0f;
	CollectAnimDuration = 0.3f;
	GrowDuration = 0.15f;
	MaxScaleMultiplier = 2.0f;

	PickupRadius = CreateDefaultSubobject<USphereComponent>(TEXT("PickupRadius"));
	PickupRadius->SetupAttachment(RootComponent);
	PickupRadius->SetSphereRadius(100.0f);
	PickupRadius->SetCollisionProfileName(TEXT("Trigger"));
	PickupRadius->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickupRadius->OnComponentBeginOverlap.AddDynamic(this, &ABasePickup::OnPickupOverlap);

	RotatingMovement = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovement"));
	RotatingMovement->RotationRate = FRotator(0.0f, 180.0f, 0.0f);

	InterpMovement = CreateDefaultSubobject<UInterpToMovementComponent>(TEXT("InterpMovement"));
	InterpMovement->Duration = 1.0f;
	InterpMovement->BehaviourType = EInterpToBehaviourType::PingPong;

	FInterpControlPoint Point1(FVector(0.0f, 0.0f, 0.0f), true);
	FInterpControlPoint Point2(FVector(0.0f, 0.0f, 30.0f), true);

	InterpMovement->ControlPoints.Add(Point1);
	InterpMovement->ControlPoints.Add(Point2);
}

void ABasePickup::BeginPlay()
{
	Super::BeginPlay();

	if (PickupRadius)
	{
		PickupRadius->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	GetWorldTimerManager().SetTimer(TimerHandle_CollectionDelay, this, &ABasePickup::EnableCollection, CollectionDelay, false);
}

void ABasePickup::EnableCollection()
{
	PickupRadius->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	TArray<AActor*> OverlappingActors;
	PickupRadius->GetOverlappingActors(OverlappingActors, AAdvancedCharacter::StaticClass());
	for (AActor* Actor : OverlappingActors)
	{
		OnPickupOverlap(PickupRadius, Actor, nullptr, 0, false, FHitResult());
		break;
	}
}

void ABasePickup::OnPickupOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bCanInteract || (bSingleUse && bHasBeenUsed))
	{
		return;
	}

	AAdvancedCharacter* Player = Cast<AAdvancedCharacter>(OtherActor);
	if (Player)
	{
		bHasBeenUsed = true;
		PickupRadius->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
		ApplyPickupEffect(Player);

		if (PickupSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
		}

		if (PickupVFX)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PickupVFX, GetActorLocation(), GetActorRotation());
		}

		StartCollectionAnimation();
	}
}

void ABasePickup::StartCollectionAnimation()
{
	InitialScale = GetActorScale3D();
	CollectAnimTimeElapsed = 0.0f;

	if (InterpMovement)
	{
		InterpMovement->Deactivate();
	}

	GetWorldTimerManager().SetTimer(TimerHandle_CollectAnim, this, &ABasePickup::UpdateCollectionAnimation, 0.01f, true);
}

void ABasePickup::UpdateCollectionAnimation()
{
	CollectAnimTimeElapsed += 0.01f;

	if (CollectAnimTimeElapsed >= CollectAnimDuration)
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_CollectAnim);
		OnPickupCollected();
	}
	else
	{
		FVector TargetScale = FVector::ZeroVector;
		if (CollectAnimTimeElapsed < GrowDuration)
		{
			float Alpha = CollectAnimTimeElapsed / GrowDuration;
			TargetScale = FMath::Lerp(InitialScale, InitialScale * MaxScaleMultiplier, Alpha);
		}
		else
		{
			float ShrinkDuration = CollectAnimDuration - GrowDuration;
			float Alpha = (CollectAnimTimeElapsed - GrowDuration) / ShrinkDuration;
			TargetScale = FMath::Lerp(InitialScale * MaxScaleMultiplier, FVector::ZeroVector, Alpha);
		}
		SetActorScale3D(TargetScale);
	}
}

void ABasePickup::ApplyPickupEffect_Implementation(AActor* Target)
{
}

void ABasePickup::OnPickupCollected_Implementation()
{
	Destroy();
}
