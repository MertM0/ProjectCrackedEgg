#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayInterface.generated.h"

UENUM(BlueprintType)
enum class EDragonElement : uint8
{
	None		UMETA(DisplayName = "None"),
	Fire		UMETA(DisplayName = "Fire"),
	Lightning	UMETA(DisplayName = "Lightning")
};

UINTERFACE(MinimalAPI)
class UGameplayInterface : public UInterface
{
	GENERATED_BODY()
};

class PROJECTCRACKEDEGG_API IGameplayInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void Interact(AActor* Interactor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damage")
	void TakeElementalDamage(EDragonElement Element, float Damage, AActor* DamageInstigator);
};