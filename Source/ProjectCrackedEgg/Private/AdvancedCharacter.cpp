#include "AdvancedCharacter.h"
#include "AttributeComponent.h"
#include "GameplayInterface.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

AAdvancedCharacter::AAdvancedCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

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
	SprintMultiplier = 1.5f;
	InteractionRange = 300.0f;
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
	}
}

void AAdvancedCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
}

void AAdvancedCharacter::HeavyAttack()
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
	if (!bCanRoll || !GetCharacterMovement()->IsMovingOnGround())
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

void AAdvancedCharacter::ExecuteInteractionLineTrace()
{
	if (!Controller) return;

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