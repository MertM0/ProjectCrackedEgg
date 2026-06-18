#include "BaseProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameplayInterface.h"
#include "AdvancedCharacter.h"
#include "DragonCompanion.h"
#include "Enemies/BaseEnemy.h"

ABaseProjectile::ABaseProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	SetRootComponent(CollisionSphere);
	CollisionSphere->InitSphereRadius(15.0f);
	CollisionSphere->SetCollisionProfileName(TEXT("Projectile"));
	CollisionSphere->CanCharacterStepUpOn = ECB_No;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = CollisionSphere;
	ProjectileMovement->InitialSpeed = 2000.0f;
	ProjectileMovement->MaxSpeed = 2000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.0f;

	ProjectileSpeed = 2000.0f;
	ProjectileDamage = 15.0f;
	ProjectileElement = EElementalType::None;
	ProjectileInstigator = nullptr;
	LifeSpan = 5.0f;

	MovementDelay = 0.25f;
	TargetActor = nullptr;
	bIsDelayed = false;
	SpawnTime = 0.0f;
}

void ABaseProjectile::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(LifeSpan);
	CollisionSphere->OnComponentHit.AddDynamic(this, &ABaseProjectile::OnHit);
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &ABaseProjectile::OnOverlap);

	if (MovementDelay > 0.0f)
	{
		bIsDelayed = true;
		SpawnTime = GetWorld()->GetTimeSeconds();
		SetActorScale3D(FVector(0.01f));
		if (ProjectileMovement)
		{
			ProjectileMovement->bSimulationEnabled = false;
		}
		GetWorldTimerManager().SetTimer(TimerHandle_MovementDelay, this, &ABaseProjectile::StartMovement, MovementDelay, false);
	}
	else
	{
		SetActorTickEnabled(false);
	}
}

void ABaseProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsDelayed)
	{
		float Elapsed = GetWorld()->GetTimeSeconds() - SpawnTime;
		float Alpha = FMath::Clamp(Elapsed / MovementDelay, 0.0f, 1.0f);
		SetActorScale3D(FMath::Lerp(FVector(0.01f), FVector(1.0f), Alpha));
	}
}

void ABaseProjectile::InitializeProjectile(EElementalType Element, float Damage, AActor* InInstigator, AActor* InTarget)
{
	ProjectileElement = Element;
	ProjectileDamage = Damage;
	ProjectileInstigator = InInstigator;
	TargetActor = InTarget;
	SetOwner(InInstigator);

	if (InInstigator)
	{
		CollisionSphere->IgnoreActorWhenMoving(InInstigator, true);
	}
}

void ABaseProjectile::StartMovement()
{
	bIsDelayed = false;
	SetActorScale3D(FVector(1.0f));
	SetActorTickEnabled(false);

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	if (IsValid(TargetActor))
	{
		FVector P_start = GetActorLocation();
		FVector X_0 = TargetActor->GetActorLocation();
		FVector V_t = TargetActor->GetVelocity();
		float s_p = ProjectileSpeed;

		FVector D = X_0 - P_start;

		float A = s_p * s_p - V_t.SizeSquared();
		float B = -2.0f * FVector::DotProduct(D, V_t);
		float C = -D.SizeSquared();

		float Discriminant = B * B - 4.0f * A * C;
		float t = -1.0f;

		if (FMath::Abs(A) > 0.0001f && Discriminant >= 0.0f)
		{
			float t1 = (-B + FMath::Sqrt(Discriminant)) / (2.0f * A);
			float t2 = (-B - FMath::Sqrt(Discriminant)) / (2.0f * A);

			if (t1 > 0.0f && t2 > 0.0f)
			{
				t = FMath::Min(t1, t2);
			}
			else if (t1 > 0.0f)
			{
				t = t1;
			}
			else if (t2 > 0.0f)
			{
				t = t2;
			}
		}

		FVector TargetLocation = X_0;
		if (t > 0.0f)
		{
			t = FMath::Min(t, 3.0f);
			TargetLocation = X_0 + V_t * t;
		}

		FVector Direction = (TargetLocation - P_start).GetSafeNormal();
		FRotator NewRotation = Direction.Rotation();
		SetActorRotation(NewRotation);
	}

	if (ProjectileMovement)
	{
		ProjectileMovement->bSimulationEnabled = true;
		ProjectileMovement->Velocity = GetActorForwardVector() * ProjectileSpeed;
	}
}

void ABaseProjectile::ProcessImpact(AActor* OtherActor)
{
	if (OtherActor && OtherActor != this && OtherActor != ProjectileInstigator)
	{
		bool bInstigatorFriendly = false;
		if (ProjectileInstigator)
		{
			bInstigatorFriendly = ProjectileInstigator->IsA(AAdvancedCharacter::StaticClass()) || ProjectileInstigator->IsA(ADragonCompanion::StaticClass());
		}

		bool bTargetFriendly = OtherActor->IsA(AAdvancedCharacter::StaticClass()) || OtherActor->IsA(ADragonCompanion::StaticClass());

		if (bInstigatorFriendly && bTargetFriendly)
		{
			return;
		}

		bool bInstigatorEnemy = false;
		if (ProjectileInstigator)
		{
			bInstigatorEnemy = ProjectileInstigator->IsA(ABaseEnemy::StaticClass());
		}

		bool bTargetEnemy = OtherActor->IsA(ABaseEnemy::StaticClass());

		if (bInstigatorEnemy && bTargetEnemy)
		{
			return;
		}

		if (OtherActor->Implements<UGameplayInterface>())
		{
			IGameplayInterface::Execute_TakeElementalDamage(OtherActor, ProjectileElement, ProjectileDamage, ProjectileInstigator);
		}
		Destroy();
	}
}

void ABaseProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ProcessImpact(OtherActor);
}

void ABaseProjectile::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ProcessImpact(OtherActor);
}

