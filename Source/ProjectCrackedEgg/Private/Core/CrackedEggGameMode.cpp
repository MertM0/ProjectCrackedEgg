#include "CrackedEggGameMode.h"
#include "BaseEnemy.h"
#include "VictoryPortal.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "EngineUtils.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

ACrackedEggGameMode::ACrackedEggGameMode()
{
	AliveEnemyCount = 0;
	ActiveDeathWidget = nullptr;
	ActiveVictoryWidget = nullptr;
	VictoryPortalRef = nullptr;
}

void ACrackedEggGameMode::BeginPlay()
{
	Super::BeginPlay();

	RegisterAllEnemies();
	FindVictoryPortal();

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		PC->SetShowMouseCursor(false);
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
	}
}

void ACrackedEggGameMode::RegisterAllEnemies()
{
	RegisteredEnemies.Empty();
	AliveEnemyCount = 0;

	for (TActorIterator<ABaseEnemy> It(GetWorld()); It; ++It)
	{
		ABaseEnemy* Enemy = *It;
		if (IsValid(Enemy))
		{
			RegisteredEnemies.Add(Enemy);
			AliveEnemyCount++;
		}
	}
}

void ACrackedEggGameMode::FindVictoryPortal()
{
	for (TActorIterator<AVictoryPortal> It(GetWorld()); It; ++It)
	{
		VictoryPortalRef = *It;
		break;
	}
}

void ACrackedEggGameMode::OnPlayerDied()
{
	ShowDeathScreen();

	GetWorldTimerManager().SetTimer(TimerHandle_Respawn, this, &ACrackedEggGameMode::HideDeathScreenAndRestart, 3.0f, false);
}

void ACrackedEggGameMode::ShowDeathScreen()
{
	if (DeathScreenWidgetClass)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
		if (PC)
		{
			ActiveDeathWidget = CreateWidget<UUserWidget>(PC, DeathScreenWidgetClass);
			if (ActiveDeathWidget)
			{
				ActiveDeathWidget->AddToViewport(100);
			}
		}
	}
}

void ACrackedEggGameMode::HideDeathScreenAndRestart()
{
	if (ActiveDeathWidget)
	{
		ActiveDeathWidget->RemoveFromParent();
		ActiveDeathWidget = nullptr;
	}

	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), true);
}

void ACrackedEggGameMode::OnEnemyKilled(ABaseEnemy* KilledEnemy)
{
	if (!IsValid(KilledEnemy)) return;

	AliveEnemyCount = FMath::Max(0, AliveEnemyCount - 1);

	if (AliveEnemyCount <= 0)
	{
		ActivateVictoryPortal();
	}
}

void ACrackedEggGameMode::ActivateVictoryPortal()
{
	if (IsValid(VictoryPortalRef))
	{
		VictoryPortalRef->SetIsActive(true);
	}
}

void ACrackedEggGameMode::OnVictory()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		APawn* PlayerPawn = PC->GetPawn();
		if (PlayerPawn)
		{
			PlayerPawn->DisableInput(PC);
			if (ACharacter* Char = Cast<ACharacter>(PlayerPawn))
			{
				if (UCharacterMovementComponent* MoveComp = Char->GetCharacterMovement())
				{
					MoveComp->StopMovementImmediately();
					MoveComp->DisableMovement();
				}
			}
		}
	}

	ShowVictoryScreen();
}

void ACrackedEggGameMode::ShowVictoryScreen()
{
	if (VictoryScreenWidgetClass)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
		if (PC)
		{
			ActiveVictoryWidget = CreateWidget<UUserWidget>(PC, VictoryScreenWidgetClass);
			if (ActiveVictoryWidget)
			{
				ActiveVictoryWidget->AddToViewport(100);

				PC->SetShowMouseCursor(true);
				PC->SetInputMode(FInputModeUIOnly());
			}
		}
	}
}

