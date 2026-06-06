#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_Combat.generated.h"

// ---------------------------------------------------------
// Start Weapon Sweep
// ---------------------------------------------------------
UCLASS()
class PROJECTCRACKEDEGG_API UAnimNotify_StartSweep : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};

// ---------------------------------------------------------
// Stop Weapon Sweep
// ---------------------------------------------------------
UCLASS()
class PROJECTCRACKEDEGG_API UAnimNotify_StopSweep : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};

// ---------------------------------------------------------
// Save Combo
// ---------------------------------------------------------
UCLASS()
class PROJECTCRACKEDEGG_API UAnimNotify_SaveCombo : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};

// ---------------------------------------------------------
// Reset Combo
// ---------------------------------------------------------
UCLASS()
class PROJECTCRACKEDEGG_API UAnimNotify_ResetCombo : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};