// Copyright Metaxis Games 2026, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MetaxisEyeSourceComponent.generated.h"

class APawn;
class AAIController;

/**
 * Drives the AI sight viewpoint from a bone or socket on a skeletal mesh instead of the actor's default eye point, so sight follows the mesh animation. It is built into the Metaxis AI Controller, Pawn and Character and takes effect only through them; on any other class it does nothing unless your own C++ GetActorEyesViewPoint override calls GetCustomEyesViewPoint. The override applies only while Enable Override Eyes Viewpoint is true.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class METAXISAITOOLS_API UMetaxisEyeSourceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMetaxisEyeSourceComponent();

	/**
	 * Bone or socket the eye viewpoint is taken from. Defaults to Eyes. It is looked up on the mesh chosen by Preferred Eye Mesh Tag, then the Character mesh, then the remaining skeletal meshes on the pawn in no guaranteed order.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Metaxis AI Perception Tools|Eye Source Component")
	FName EyeSocketName;

	// If set, meshes with this component tag are checked first when resolving the eye socket.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Metaxis AI Perception Tools|Eye Source Component")
	FName PreferredEyeMeshTag;

	/**
	 * When true (the default), the owner's Get Actor Eyes View Point returns the resolved socket transform instead of the actor's default eye point. When false the component does nothing and the engine's default viewpoint is used.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Metaxis AI Perception Tools|Eye Source Component")
	bool bEnableOverrideEyesViewpoint;

	/**
	 * When true, the eye forward vector is projected onto the XY plane so sight aim stays level regardless of head pitch. Defaults to false. Set Flatten Sight Forward Axis changes it at runtime.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Metaxis AI Perception Tools|Eye Source Component")
	bool bFlattenSightForwardAxis;

	bool GetCustomEyesViewPoint(const APawn* TargetPawn, FVector& OutLocation, FRotator& OutRotation) const;

	/**
	 * Updates Flatten Sight Forward Axis at runtime, switching the eye forward direction between the full socket rotation and a level XY projection. Pass a valid AI Controller to also request a stimuli listener update on its perception component, so active sight listeners refresh promptly.
	 */
	UFUNCTION(BlueprintCallable, Category = "Metaxis AI Perception Tools|Eye Source Component")
	void SetFlattenSightForwardAxis(AAIController* OwningController, bool bNewEnabled);
};