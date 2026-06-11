#include "AdvancedCharacter.h"
#include "AttributeComponent.h"
#include "GameplayInterface.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "CrackedEggGameMode.h"
#include "UI/DamageTextActor.h"

AAdvancedCharacter::AAdvancedCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 400.0f;
	SpringArmComponent->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;

	AttributeComponent = CreateDefaultSubobject<UAttributeComponent>(TEXT("AttributeComponent"));

	RollStrength = 1500.0f;
	RollCooldown = 1.5f;
	bCanRoll = true;
	bIsSprinting = false;
	bIsAttacking = false;
	bSaveAttack = false;
	bComboWindowOpen = false;
	bIsSweeping = false;
	ComboIndex = 0;
	SprintMultiplier = 1.5f;
	InteractionRange = 300.0f;
	LastAttackedEnemy = nullptr;
	HitReactChance = 0.5f;
	LightAttackHitboxSize = FVector(75.0f, 75.0f, 75.0f);
	HeavyAttackHitboxRadius = 150.0f;
	HeavyAttackDamageMultiplier = 1.5f;
	bIsDead = false;
}

void AAdvancedCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	if (AttributeComponent)
	{
		GetCharacterMovement()->MaxWalkSpeed = AttributeComponent->GetAttributeValue(EAttributeType::MovementSpeed);
		AttributeComponent->OnDeath.AddDynamic(this, &AAdvancedCharacter::HandleDeath);
	}
}

void AAdvancedCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsSweeping)
	{
		PerformWeaponSweep();
	}
}

void AAdvancedCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAdvancedCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AAdvancedCharacter::Look);
		
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AAdvancedCharacter::StartSprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AAdvancedCharacter::StopSprint);

		EnhancedInputComponent->BindAction(LightAttackAction, ETriggerEvent::Started, this, &AAdvancedCharacter::LightAttack);
		EnhancedInputComponent->BindAction(HeavyAttackAction, ETriggerEvent::Started, this, &AAdvancedCharacter::HeavyAttack);

		EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Started, this, &AAdvancedCharacter::ExecuteRoll);

		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AAdvancedCharacter::ExecuteInteractionLineTrace);
	}
}

void AAdvancedCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		if (bIsDead) return;

		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AAdvancedCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AAdvancedCharacter::LightAttack()
{
	if (bIsDead) return;

	if (bIsAttacking)
	{
		if (bComboWindowOpen)
		{
			bComboWindowOpen = false;
			ComboIndex++;
			if (ComboIndex > 4)
			{
				ComboIndex = 1;
			}
			PlayLightAttackMontage(ComboIndex);
		}
		else
		{
			bSaveAttack = true;
		}
	}
	else
	{
		bIsAttacking = true;
		ComboIndex = 1;
		PlayLightAttackMontage(ComboIndex);
	}
}

void AAdvancedCharacter::HeavyAttack()
{
	if (bIsDead) return;

	if (!bIsAttacking)
	{
		bIsAttacking = true;
		
		if (APlayerController* PC = Cast<APlayerController>(Controller))
		{
			PC->SetIgnoreMoveInput(true);
		}

		PlayHeavyAttackMontage();
	}
}

void AAdvancedCharacter::SaveCombo()
{
	if (bSaveAttack)
	{
		bSaveAttack = false;
		ComboIndex++;

		if (ComboIndex > 4)
		{
			ComboIndex = 1;
		}

		PlayLightAttackMontage(ComboIndex);
	}
	else
	{
		bComboWindowOpen = true;
	}
}

void AAdvancedCharacter::ResetCombo()
{
	bIsAttacking = false;
	bSaveAttack = false;
	bComboWindowOpen = false;
	ComboIndex = 0;
	StopWeaponSweep();

	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		PC->SetIgnoreMoveInput(false);
	}
}

void AAdvancedCharacter::PlayLightAttackMontage_Implementation(int32 ComboStep)
{
}

void AAdvancedCharacter::PlayHeavyAttackMontage_Implementation()
{
}

void AAdvancedCharacter::StartSprint()
{
	if (AttributeComponent)
	{
		GetCharacterMovement()->MaxWalkSpeed = AttributeComponent->GetAttributeValue(EAttributeType::MovementSpeed) * SprintMultiplier;
		bIsSprinting = true;
	}
}

void AAdvancedCharacter::StopSprint()
{
	if (AttributeComponent)
	{
		GetCharacterMovement()->MaxWalkSpeed = AttributeComponent->GetAttributeValue(EAttributeType::MovementSpeed);
		bIsSprinting = false;
	}
}

void AAdvancedCharacter::ExecuteRoll()
{
	if (!bCanRoll || !GetCharacterMovement()->IsMovingOnGround() || bIsDead)
	{
		return;
	}

	FVector LaunchDirection;

	if (GetCharacterMovement()->Velocity.SizeSquared() > KINDA_SMALL_NUMBER)
	{
		LaunchDirection = GetCharacterMovement()->Velocity.GetSafeNormal();
	}
	else
	{
		LaunchDirection = GetActorForwardVector();
	}

	FVector LaunchVelocity = LaunchDirection * RollStrength;
	LaunchVelocity.Z += 200.0f;

	LaunchCharacter(LaunchVelocity, true, true);

	bCanRoll = false;
	GetWorldTimerManager().SetTimer(TimerHandle_RollCooldown, this, &AAdvancedCharacter::ResetRoll, RollCooldown, false);
}

void AAdvancedCharacter::ResetRoll()
{
	bCanRoll = true;
}

void AAdvancedCharacter::TakeElementalDamage_Implementation(EDragonElement Element, float Damage, AActor* DamageInstigator)
{
	if (bIsDead) return;

	if (AttributeComponent && AttributeComponent->IsAlive())
	{
		AttributeComponent->ApplyHealthChange(-Damage);

		if (DamageTextClass)
		{
			FVector SpawnLoc = GetActorLocation() + FVector(0.0f, 0.0f, 100.0f);
			ADamageTextActor* TextActor = GetWorld()->SpawnActor<ADamageTextActor>(DamageTextClass, SpawnLoc, FRotator::ZeroRotator);
			if (TextActor)
			{
				TextActor->InitializeDamageText(Damage, true);
			}
		}

		if (HitVFX)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitVFX, GetActorLocation(), GetActorRotation());
		}

		if (AttributeComponent->IsAlive())
		{
			if (HitReactMontage && FMath::FRand() <= HitReactChance)
			{
				ResetCombo();
				PlayAnimMontage(HitReactMontage);
			}
		}
	}
}

void AAdvancedCharacter::ExecuteInteractionLineTrace()
{
	if (!Controller || bIsDead) return;

	FVector StartLocation;
	FRotator LookRotation;
	Controller->GetPlayerViewPoint(StartLocation, LookRotation);

	FVector LookDirection = LookRotation.Vector();
	FVector EndLocation = StartLocation + (LookDirection * InteractionRange);

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, CollisionParams);

	if (bHit && HitResult.GetActor())
	{
		AActor* HitActor = HitResult.GetActor();
		
		if (HitActor->Implements<UGameplayInterface>())
		{
			IGameplayInterface::Execute_Interact(HitActor, this);
		}
	}
}

void AAdvancedCharacter::StartWeaponSweep()
{
	bIsSweeping = true;
	HitActorsDuringAttack.Empty();
	HitActorsDuringAttack.Add(this);
}

void AAdvancedCharacter::StopWeaponSweep()
{
	bIsSweeping = false;
	HitActorsDuringAttack.Empty();
}

void AAdvancedCharacter::PerformWeaponSweep()
{
	FVector StartLocation = GetActorLocation();
	FVector ForwardVector = GetActorForwardVector();
	FVector EndLocation = StartLocation + (ForwardVector * 150.0f);

	FCollisionShape BoxShape = FCollisionShape::MakeBox(LightAttackHitboxSize);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActors(HitActorsDuringAttack);

	FHitResult HitResult;
	bool bHit = GetWorld()->SweepSingleByChannel(HitResult, StartLocation, EndLocation, GetActorQuat(), ECC_Pawn, BoxShape, QueryParams);

	if (bHit)
	{
		DrawDebugBox(GetWorld(), StartLocation + (ForwardVector * 75.0f), LightAttackHitboxSize, GetActorQuat(), FColor::Green, false, 2.0f);
	}
	else if (HitActorsDuringAttack.Num() <= 1)
	{
		DrawDebugBox(GetWorld(), StartLocation + (ForwardVector * 75.0f), LightAttackHitboxSize, GetActorQuat(), FColor::Red, false, 2.0f);
	}

	if (bHit && HitResult.GetActor())
	{
		AActor* HitActor = HitResult.GetActor();
		HitActorsDuringAttack.Add(HitActor);
		LastAttackedEnemy = HitActor;
		
		if (HitActor->Implements<UGameplayInterface>())
		{
			float Damage = AttributeComponent ? AttributeComponent->GetAttributeValue(EAttributeType::BaseDamage) : 10.0f;
			IGameplayInterface::Execute_TakeElementalDamage(HitActor, EDragonElement::None, Damage, this);
		}
	}
}

void AAdvancedCharacter::PerformHeavySweep()
{
	FVector SweepLocation = GetActorLocation();
	
	FCollisionShape SphereShape = FCollisionShape::MakeSphere(HeavyAttackHitboxRadius);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActors(HitActorsDuringAttack);

	TArray<FHitResult> HitResults;
	bool bHit = GetWorld()->SweepMultiByChannel(HitResults, SweepLocation, SweepLocation, FQuat::Identity, ECC_Pawn, SphereShape, QueryParams);

	DrawDebugSphere(GetWorld(), SweepLocation, HeavyAttackHitboxRadius, 12, bHit ? FColor::Green : FColor::Red, false, 2.0f);

	if (bHit)
	{
		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();
			if (HitActor && !HitActorsDuringAttack.Contains(HitActor) && HitActor->Implements<UGameplayInterface>())
			{
				HitActorsDuringAttack.Add(HitActor);
				LastAttackedEnemy = HitActor;
				
				float Damage = AttributeComponent ? (AttributeComponent->GetAttributeValue(EAttributeType::BaseDamage) * HeavyAttackDamageMultiplier) : 15.0f;
				IGameplayInterface::Execute_TakeElementalDamage(HitActor, EDragonElement::None, Damage, this);
			}
		}
	}
}

void AAdvancedCharacter::HandleDeath(UAttributeComponent* AttributeComp)
{
	if (bIsDead) return;
	bIsDead = true;

	ResetCombo();
	StopAnimMontage();

	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		DisableInput(PC);
	}

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (DeathMontage)
	{
		float MontageDuration = PlayAnimMontage(DeathMontage);
		if (MontageDuration > 0.0f)
		{
			float RagdollDelay = FMath::Max(0.1f, MontageDuration - 0.25f);
			GetWorldTimerManager().SetTimer(TimerHandle_Ragdoll, this, &AAdvancedCharacter::TriggerRagdoll, RagdollDelay, false);
		}
		else
		{
			TriggerRagdoll();
		}
	}
	else
	{
		TriggerRagdoll();
	}
}

void AAdvancedCharacter::TriggerRagdoll()
{
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->bPauseAnims = true;
		MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		MeshComp->SetCollisionProfileName(TEXT("Ragdoll"));
		MeshComp->SetSimulatePhysics(true);
	}

	GetWorldTimerManager().SetTimer(TimerHandle_DeathSequence, this, &AAdvancedCharacter::OnDeathSequenceFinished, 1.5f, false);
}

void AAdvancedCharacter::OnDeathSequenceFinished()
{
	OnDeath();

	if (ACrackedEggGameMode* GM = Cast<ACrackedEggGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GM->OnPlayerDied();
	}
}
