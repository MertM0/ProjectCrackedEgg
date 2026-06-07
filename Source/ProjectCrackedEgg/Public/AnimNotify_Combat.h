#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_Combat.generated.h"

// ---------------------------------------------------------
// Start Weapon Sweep (Light Attack)
// ---------------------------------------------------------
UCLASS()
class PROJECTCRACKEDEGG_API UAnimNotify_StartSweep : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};

// ---------------------------------------------------------
// Start Heavy Sweep (360 Attack)
// ---------------------------------------------------------
UCLASS()
class PROJECTCRACKEDEGG_API UAnimNotify_StartHeavySweep : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};

// ---------------------------------------------------------
// Stop Weapon Sweep (For both)
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