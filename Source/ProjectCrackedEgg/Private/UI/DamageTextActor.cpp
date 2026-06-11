#include "UI/DamageTextActor.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"

ADamageTextActor::ADamageTextActor()
{
	PrimaryActorTick.bCanEverTick = true;

	TextRender = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRender"));
	RootComponent = TextRender;

	TextRender->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	TextRender->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);

	PlayerDamageColor = FColor::Red;
	EnemyDamageColor = FColor::White;
	LifeTime = 1.0f;
	Gravity = 500.0f;
	Age = 0.0f;
}

void ADamageTextActor::BeginPlay()
{
	Super::BeginPlay();

	LaunchVelocity = FVector(
		FMath::FRandRange(-80.0f, 80.0f),
		FMath::FRandRange(-80.0f, 80.0f),
		FMath::FRandRange(200.0f, 300.0f)
	);
}

void ADamageTextActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Age += DeltaTime;
	if (Age >= LifeTime)
	{
		Destroy();
		return;
	}

	LaunchVelocity.Z -= Gravity * DeltaTime;
	AddActorWorldOffset(LaunchVelocity * DeltaTime);

	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	if (CameraManager)
	{
		FRotator TargetRotation = CameraManager->GetCameraRotation();
		TargetRotation.Yaw += 180.0f;
		SetActorRotation(TargetRotation);
	}

	float Alpha = FMath::Clamp(1.0f - (Age / LifeTime), 0.0f, 1.0f);
	FColor NewColor = TextRender->TextRenderColor;
	NewColor.A = FMath::RoundToInt(Alpha * 255.0f);
	TextRender->SetTextRenderColor(NewColor);
}

void ADamageTextActor::InitializeDamageText(float DamageAmount, bool bIsDamageToPlayer)
{
	FString DamageStr = FString::FromInt(FMath::RoundToInt(DamageAmount));
	if (TextRender)
	{
		TextRender->SetText(FText::FromString(DamageStr));
		TextRender->SetTextRenderColor(bIsDamageToPlayer ? PlayerDamageColor : EnemyDamageColor);
	}
}
