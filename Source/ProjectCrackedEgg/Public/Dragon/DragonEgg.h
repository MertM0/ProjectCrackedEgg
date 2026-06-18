#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/GameplayInterface.h"
#include "DragonEgg.generated.h"

class ADragonCompanion;
class UParticleSystem;
class USoundBase;
class USkeletalMeshComponent;
class UUserWidget;

UCLASS()
class PROJECTCRACKEDEGG_API ADragonEgg : public AActor, public IGameplayInterface
{
	GENERATED_BODY()
	
public:	
	ADragonEgg();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* EggMeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hatching")
	TSubclassOf<ADragonCompanion> FireDragonCompanionClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hatching")
	TSubclassOf<ADragonCompanion> LightningDragonCompanionClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hatching")
	TSubclassOf<UUserWidget> ElementSelectionWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hatching")
	UParticleSystem* HatchVFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hatching")
	USoundBase* HatchSFX;

public:	
	virtual void Interact_Implementation(AActor* Interactor) override;

	UFUNCTION(BlueprintCallable, Category = "Hatching")
	void FinishHatching(EElementalType SelectedElement);
};
