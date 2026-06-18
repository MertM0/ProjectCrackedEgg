#include "Dragon/DragonEgg.h"
#include "Dragon/DragonCompanion.h"
#include "Components/AttributeComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

ADragonEgg::ADragonEgg()
{
	PrimaryActorTick.bCanEverTick = false;

	EggMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("EggMeshComponent"));
	RootComponent = EggMeshComponent;
}

void ADragonEgg::BeginPlay()
{
	Super::BeginPlay();
}

void ADragonEgg::Interact_Implementation(AActor* Interactor)
{
}

void ADragonEgg::FinishHatching(EElementalType SelectedElement)
{
	TSubclassOf<ADragonCompanion> ClassToSpawn = nullptr;
	if (SelectedElement == EElementalType::Fire)
	{
		ClassToSpawn = FireDragonCompanionClass;
	}
	else if (SelectedElement == EElementalType::Lightning)
	{
		ClassToSpawn = LightningDragonCompanionClass;
	}

	if (ClassToSpawn)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ADragonCompanion* Dragon = GetWorld()->SpawnActor<ADragonCompanion>(ClassToSpawn, GetActorLocation(), GetActorRotation(), SpawnParams);
		if (Dragon)
		{
			Dragon->SetDragonElement(SelectedElement);
			UAttributeComponent* AttrComp = Dragon->FindComponentByClass<UAttributeComponent>();
			if (AttrComp)
			{
				AttrComp->MaxLevelLimit = 2;
			}
		}
	}

	if (HatchVFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HatchVFX, GetActorLocation(), GetActorRotation());
	}

	if (HatchSFX)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), HatchSFX, GetActorLocation());
	}
}
