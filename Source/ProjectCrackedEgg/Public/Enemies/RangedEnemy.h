#pragma once

#include "CoreMinimal.h"
#include "Enemies/BaseEnemy.h"
#include "RangedEnemy.generated.h"

UCLASS()
class PROJECTCRACKEDEGG_API ARangedEnemy : public ABaseEnemy
{
	GENERATED_BODY()

public:
	ARangedEnemy();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged Enemy")
	EElementalType RangedElement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged Enemy")
	TSubclassOf<class ABaseProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged Enemy")
	FName ProjectileSpawnSocket;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged Enemy")
	float RangedAttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged Enemy")
	float PreferredDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged Enemy")
	float RangedCooldown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged Enemy")
	float RetreatSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged Enemy")
	class UAnimMontage* RangedAttackMontage;

	float LastRangedAttackTime;

public:
	UFUNCTION(BlueprintCallable, Category = "Ranged Enemy")
	void SpawnProjectile(AActor* Target);

	UFUNCTION(BlueprintPure, Category = "Ranged Enemy")
	bool CanRangedAttack() const;

	EElementalType GetRangedElement() const { return RangedElement; }
	class UAnimMontage* GetRangedAttackMontage() const { return RangedAttackMontage; }
	float GetPreferredDistance() const { return PreferredDistance; }
	float GetRetreatSpeed() const { return RetreatSpeed; }
};
