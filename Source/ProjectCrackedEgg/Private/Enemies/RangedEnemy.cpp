#include "Enemies/RangedEnemy.h"
#include "AttributeComponent.h"
#include "Dragon/BaseProjectile.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"

ARangedEnemy::ARangedEnemy()
{
	RangedElement = EElementalType::Fire;
	ProjectileClass = nullptr;
	ProjectileSpawnSocket = TEXT("Muzzle");
	RangedAttackRange = 800.0f;
	PreferredDistance = 600.0f;
	RangedCooldown = 3.0f;
	RetreatSpeed = 600.0f;
	RangedAttackMontage = nullptr;
	LastRangedAttackTime = -100.0f;

	MeleeAttackMontage = nullptr;

	if (AttributeComponent)
	{
		AttributeComponent->MaxHealth = 80.0f;
		AttributeComponent->BaseDamage = 12.0f;
	}
}

void ARangedEnemy::SpawnProjectile(AActor* Target)
{
	if (!ProjectileClass || !Target) return;

	USkeletalMeshComponent* EnemyMesh = GetMesh();
	FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
	FRotator SpawnRotation = GetActorRotation();

	if (EnemyMesh && EnemyMesh->DoesSocketExist(ProjectileSpawnSocket))
	{
		SpawnLocation = EnemyMesh->GetSocketLocation(ProjectileSpawnSocket);
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
		float Damage = AttributeComponent ? AttributeComponent->GetAttributeValue(EAttributeType::BaseDamage) : 12.0f;
		Projectile->InitializeProjectile(RangedElement, Damage, this, Target);
	}

	LastRangedAttackTime = GetWorld()->GetTimeSeconds();
}

bool ARangedEnemy::CanRangedAttack() const
{
	return GetWorld()->GetTimeSeconds() - LastRangedAttackTime >= RangedCooldown;
}
