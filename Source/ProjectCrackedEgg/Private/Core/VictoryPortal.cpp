#include "VictoryPortal.h"
#include "Components/BoxComponent.h"
#include "AdvancedCharacter.h"
#include "CrackedEggGameMode.h"
#include "Kismet/GameplayStatics.h"

AVictoryPortal::AVictoryPortal()
{
	PrimaryActorTick.bCanEverTick = false;

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootScene;

	OverlapBox = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapBox"));
	OverlapBox->SetupAttachment(RootScene);

	bIsActive = false;
}

void AVictoryPortal::BeginPlay()
{
	Super::BeginPlay();

	if (OverlapBox)
	{
		OverlapBox->OnComponentBeginOverlap.AddDynamic(this, &AVictoryPortal::OnOverlapBegin);
	}
}

void AVictoryPortal::SetIsActive(bool bActive)
{
	bIsActive = bActive;

	if (bIsActive)
	{
		OnPortalActivated();
	}
	else
	{
		OnPortalDeactivated();
	}
}

void AVictoryPortal::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bIsActive) return;

	if (OtherActor && OtherActor->IsA(AAdvancedCharacter::StaticClass()))
	{
		if (ACrackedEggGameMode* GM = Cast<ACrackedEggGameMode>(UGameplayStatics::GetGameMode(this)))
		{
			GM->OnVictory();
		}
	}
}

