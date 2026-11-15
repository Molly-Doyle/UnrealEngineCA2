// Copyright Metaxis Games 2026, All Rights Reserved

#pragma once

#include "Runtime/Launch/Resources/Version.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Perception/AISightTargetInterface.h"
#include "Engine/TimerHandle.h"
#include "MetaxisSightTargetComponent.generated.h"

/**
 * Defines which points on an actor can satisfy AI sight line-of-sight checks. Attach to the actor being perceived (for example the player character).
 *
 * On UE 5.5 and later, UAISense_Sight discovers this component through FindComponentByInterface and delegates line-of-sight checks to CanBeSeenFrom. UE 5.4 and earlier never query components for the sight target interface, so there the component compiles but is never consulted; a one-time warning at play start says so.
 *
 * Trace points are tried in this order: scene components matched by SightTargetComponentTags, then sockets in SightTargetSockets on the resolved skeletal mesh, then the actor origin if bFallbackToActorLocation is true. The first unobstructed point wins and becomes the seen location.
 *
 * This refines which points can satisfy line of sight. The engine still tests the sight cone and range against the actor location; this component does not change that.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class METAXISAITOOLS_API UMetaxisSightTargetComponent : public UActorComponent, public IAISightTargetInterface
{
	GENERATED_BODY()

public:
	UMetaxisSightTargetComponent();

	/**
	 * Optional component tags that mark extra line-of-sight target points.
	 *
	 * Any scene component on this actor whose Component Tags contain one of these names becomes a trace point: cameras, static or skeletal meshes, arrows, or plain scene components all qualify. Tags are set on the component itself, in the Component Tags array of its Details panel, not in the actor's Tags.
	 *
	 * Tagged components are traced before Sight Target Sockets. Priority follows this array's order. Components sharing one tag have no guaranteed order between them, so use separate tags when the order matters. A component matched by more than one tag is traced once.
	 *
	 * Leave empty (the default) to use sockets only. Non-skeletal pawns such as VR pawns, vehicles and drones should tag their camera or a scene component placed at head height, since they have no skeleton to take sockets from.
	 *
	 * Multiplayer: perception runs on the server, so a tagged component's transform must be correct on the server for remote players to be perceived correctly.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Metaxis AI Perception Tools|Sight Target Component")
	TArray<FName> SightTargetComponentTags;

	/**
	 * Bone or socket names to use as sight target points, taken from the single skeletal mesh chosen by Preferred Mesh Tag, then the Character mesh, then the first skeletal mesh found on the actor. Traced in array order, after any components matched by Sight Target Component Tags. The first unobstructed point makes the actor visible at that location. Names that do not exist on the resolved mesh are skipped, and reported once in the Output Log in non-shipping builds.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Metaxis AI Perception Tools|Sight Target Component")
	TArray<FName> SightTargetSockets;

	/**
	 * Optional component tag used to choose which skeletal mesh supplies the sockets. The first skeletal mesh carrying this tag wins outright, even if it lacks the listed sockets. Without a match, the Character mesh is used, then the first skeletal mesh found on the actor. Only one mesh is ever used for sockets.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Metaxis AI Perception Tools|Sight Target Component")
	FName PreferredMeshTag;

	/**
	 * If true (the default), the component traces to the actor's origin whenever no tagged component and no socket returned a clear line of sight, including when the points resolved correctly but were all obstructed. Set to false for strict point-only line of sight, where the actor is not visible unless one of the listed points is.
	 *
	 * The fallback point is the actor origin (GetActorLocation), which on VR pawns and vehicles is usually not where the body is.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Metaxis AI Perception Tools|Sight Target Component")
	bool bFallbackToActorLocation;

	/**
	 * When true, line of sight traces use the engine's own sight channel from Project Settings (AI System, Default Sight Collision Channel), matching every actor that does not carry this component. When false, Sight Trace Channel below is used instead. The engine's shipped default is Visibility.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Metaxis AI Perception Tools|Sight Target Component")
	bool bUseEngineSightCollisionChannel = true;

	/**
	 * Trace channel for line of sight when Use Engine Sight Collision Channel is false. Visibility reproduces the behaviour of earlier plugin versions.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Metaxis AI Perception Tools|Sight Target Component", meta = (EditCondition = "!bUseEngineSightCollisionChannel"))
	TEnumAsByte<ECollisionChannel> SightTraceChannel = ECC_Visibility;

	/**
	 * Trace against complex, per triangle collision, matching the engine's own sight traces. Set false to trace simple collision hulls, the behaviour of earlier plugin versions.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Metaxis AI Perception Tools|Sight Target Component")
	bool bTraceComplex = true;

	// IAISightTargetInterface
	virtual UAISense_Sight::EVisibilityResult CanBeSeenFrom(
		const FCanBeSeenFromContext& Context,
		FVector& OutSeenLocation,
		int32& OutNumberOfLoSChecksPerformed,
		int32& OutNumberOfAsyncLosCheckRequested,
		float& OutSightStrength,
		int32* UserData = nullptr,
		const FOnPendingVisibilityQueryProcessedDelegate* Delegate = nullptr) override;

private:

	USkeletalMeshComponent* ResolveSkeletalMesh() const;

	// Builds the query params shared by all three line of sight traces: the
	// engine's own AILineOfSight trace tag and stat, the complex flag from
	// bTraceComplex, and the single ignored actor from the sight query context.
	FCollisionQueryParams MakeSightQueryParams(const AActor* InIgnoreActor) const;

	// Reimplements the engine's unexported UE::AISense_Sight::IsTraceConsideredVisible
	// exactly. A trace that hit nothing is clear; a trace that hit something is clear
	// only when the hit actor is owned by the target, which AActor::IsOwnedBy reports
	// true for the target itself. A null hit actor counts as blocked.
	static bool IsTraceClear(const bool bHit, const FHitResult& HitResult, const AActor* InOwnerActor);

	// One-shot configuration check run on the first CanBeSeenFrom call, by which
	// point all BeginPlay setup has run (component BeginPlay precedes the owner's
	// Blueprint BeginPlay, so BeginPlay itself would be too early). The members
	// are unguarded so the class layout is the same in every build configuration;
	// only the body is compiled out in Shipping.
	bool bSightTargetConfigChecked = false;
	void CheckSightTargetConfig(const AActor* OwnerActor, const USkeletalMeshComponent* ResolvedMesh);

#if ENGINE_MAJOR_VERSION < 5 || (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION < 5)
	// UE 5.4 and earlier resolve IAISightTargetInterface from the perceived actor
	// only, so CanBeSeenFrom is never called and the check above never runs. This
	// warning therefore fires from a timer armed in BeginPlay instead. The members are unguarded
	// against build configuration so the class layout is the same in every
	// configuration; only the body is compiled out in Shipping.
	FTimerHandle EngineSupportCheckTimerHandle;
	bool bUnsupportedEngineWarningShown = false;
	void CheckEngineSupport();

	virtual void BeginPlay() override;
#endif
};
