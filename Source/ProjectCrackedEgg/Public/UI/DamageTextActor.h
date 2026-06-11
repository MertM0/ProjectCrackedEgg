#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DamageTextActor.generated.h"

class UTextRenderComponent;

UCLASS()
class PROJECTCRACKEDEGG_API ADamageTextActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ADamageTextActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	void InitializeDamageText(float DamageAmount, bool bIsDamageToPlayer);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UTextRenderComponent* TextRender;

	UPROPERTY(EditDefaultsOnly, Category = "Damage Text")
	FColor PlayerDamageColor;

	UPROPERTY(EditDefaultsOnly, Category = "Damage Text")
	FColor EnemyDamageColor;

	UPROPERTY(EditDefaultsOnly, Category = "Damage Text")
	float LifeTime;

	UPROPERTY(EditDefaultsOnly, Category = "Damage Text")
	float Gravity;

	FVector LaunchVelocity;
	float Age;
};
