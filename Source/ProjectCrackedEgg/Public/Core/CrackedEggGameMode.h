#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CrackedEggGameMode.generated.h"

class ABaseEnemy;
class AVictoryPortal;

UCLASS()
class PROJECTCRACKEDEGG_API ACrackedEggGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACrackedEggGameMode();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> DeathScreenWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> VictoryScreenWidgetClass;

	UPROPERTY()
	UUserWidget* ActiveDeathWidget;

	UPROPERTY()
	UUserWidget* ActiveVictoryWidget;

	UPROPERTY(BlueprintReadOnly, Category = "Game Flow")
	TArray<ABaseEnemy*> RegisteredEnemies;

	int32 AliveEnemyCount;

	UPROPERTY()
	AVictoryPortal* VictoryPortalRef;

	FTimerHandle TimerHandle_DeathScreen;
	FTimerHandle TimerHandle_Respawn;

public:
	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void OnPlayerDied();

	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void OnEnemyKilled(ABaseEnemy* KilledEnemy);

	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void OnVictory();

	UFUNCTION(BlueprintPure, Category = "Game Flow")
	int32 GetAliveEnemyCount() const { return AliveEnemyCount; }

protected:
	void RegisterAllEnemies();
	void FindVictoryPortal();
	void ActivateVictoryPortal();

	void ShowDeathScreen();
	void HideDeathScreenAndRestart();

	void ShowVictoryScreen();
};

