// Copyright Metaxis Games 2026, All Rights Reserved

#include "MetaxisSightTargetComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Engine/Engine.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "CollisionQueryParams.h"
#include "AISystem.h"

UMetaxisSightTargetComponent::UMetaxisSightTargetComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bFallbackToActorLocation = true;
	PreferredMeshTag = NAME_None;
}

#if ENGINE_MAJOR_VERSION < 5 || (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION < 5)
void UMetaxisSightTargetComponent::BeginPlay()
{
	Super::BeginPlay();

#if !UE_BUILD_SHIPPING
	// Delay the check so any BeginPlay setup on the owner runs first.
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			EngineSupportCheckTimerHandle, this,
			&UMetaxisSightTargetComponent::CheckEngineSupport, 2.0f, false);
	}
#endif
}

void UMetaxisSightTargetComponent::CheckEngineSupport()
{
#if !UE_BUILD_SHIPPING
	if (bUnsupportedEngineWarningShown)
	{
		return;
	}
	bUnsupportedEngineWarningShown = true;

	const AActor* OwnerActor = GetOwner();
	if (!IsValid(OwnerActor))
	{
		return;
	}

	UE_LOG(LogTemp, Warning,
		TEXT("Metaxis Sight Target '%s' on '%s': this engine is UE %d.%d, which never queries components for the sight target interface, so this component has no effect and AI sight uses the actor location. It needs UE 5.5 or later. Every other Metaxis AI Perception Tools feature works on this engine."),
		*GetName(), *OwnerActor->GetName(), ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION);

	const UWorld* World = GetWorld();
	if (GEngine && World && World->WorldType == EWorldType::PIE)
	{
		GEngine->AddOnScreenDebugMessage(-1, 12.0f, FColor::Orange,
			FString::Printf(TEXT("Metaxis: Sight Target on '%s' has no effect on UE %d.%d. See the Output Log."),
				*OwnerActor->GetName(), ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION));
	}
#endif
}
#endif

FCollisionQueryParams UMetaxisSightTargetComponent::MakeSightQueryParams(const AActor* InIgnoreActor) const
{
	return FCollisionQueryParams(SCENE_QUERY_STAT(AILineOfSight), bTraceComplex, InIgnoreActor);
}

bool UMetaxisSightTargetComponent::IsTraceClear(const bool bHit, const FHitResult& HitResult, const AActor* InOwnerActor)
{
	if (!bHit)
	{
		return true;
	}

	// Matches the engine's unexported UE::AISense_Sight::IsTraceConsideredVisible
	// exactly, including the null hit actor counting as blocked.
	const AActor* HitActor = HitResult.GetActor();
	return HitActor ? HitActor->IsOwnedBy(InOwnerActor) : false;
}

UAISense_Sight::EVisibilityResult UMetaxisSightTargetComponent::CanBeSeenFrom(
	const FCanBeSeenFromContext& Context,
	FVector& OutSeenLocation,
	int32& OutNumberOfLoSChecksPerformed,
	int32& OutNumberOfAsyncLosCheckRequested,
	float& OutSightStrength,
	int32* UserData,
	const FOnPendingVisibilityQueryProcessedDelegate* Delegate)
{
	OutNumberOfLoSChecksPerformed = 0;
	OutNumberOfAsyncLosCheckRequested = 0;
	OutSightStrength = 0.0f;

	AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		return UAISense_Sight::EVisibilityResult::NotVisible;
	}

	UWorld* World = OwnerActor->GetWorld();
	if (!World)
	{
		return UAISense_Sight::EVisibilityResult::NotVisible;
	}

	FCollisionQueryParams QueryParams = MakeSightQueryParams(Context.IgnoreActor);

	// The sense snapshots the same setting when it is constructed, so an ini change
	// to the engine's sight channel needs an editor restart before either path picks
	// it up.
	const ECollisionChannel TraceChannel = bUseEngineSightCollisionChannel ? GetDefault<UAISystem>()->DefaultSightCollisionChannel.GetValue() : SightTraceChannel.GetValue();

	USkeletalMeshComponent* Mesh = ResolveSkeletalMesh();

#if !UE_BUILD_SHIPPING
	CheckSightTargetConfig(OwnerActor, Mesh);
#endif

	// 1) Scene components matched by tag, in tag order. Skipped entirely when no
	//    tags are set, so content that predates this property pays nothing here.
	if (SightTargetComponentTags.Num() > 0)
	{
		TInlineComponentArray<USceneComponent*> SceneComponents(OwnerActor);

		// A component carrying more than one listed tag is traced once. Positional
		// flags over the gathered array avoid a set and any heap allocation, and
		// are only needed when a second tag could revisit a component.
		const bool bNeedDedupe = SightTargetComponentTags.Num() > 1;
		TArray<bool, TInlineAllocator<NumInlinedActorComponents>> AlreadyTracedFlags;
		if (bNeedDedupe)
		{
			AlreadyTracedFlags.Init(false, SceneComponents.Num());
		}

		for (const FName& Tag : SightTargetComponentTags)
		{
			if (Tag.IsNone())
			{
				continue;
			}

			for (int32 Index = 0; Index < SceneComponents.Num(); ++Index)
			{
				USceneComponent* Candidate = SceneComponents[Index];
				if (!IsValid(Candidate) || !Candidate->IsRegistered() || !Candidate->ComponentHasTag(Tag))
				{
					continue;
				}

				if (bNeedDedupe)
				{
					if (AlreadyTracedFlags[Index])
					{
						continue;
					}
					AlreadyTracedFlags[Index] = true;
				}

				const FVector ComponentLocation = Candidate->GetComponentLocation();
				FHitResult HitResult;
				++OutNumberOfLoSChecksPerformed;

				const bool bHit = World->LineTraceSingleByChannel(
					HitResult,
					Context.ObserverLocation,
					ComponentLocation,
					TraceChannel,
					QueryParams);

				if (IsTraceClear(bHit, HitResult, OwnerActor))
				{
					OutSeenLocation = ComponentLocation;
					OutSightStrength = 1.0f;
					return UAISense_Sight::EVisibilityResult::Visible;
				}
			}
		}
	}

	// 2) Sockets on the resolved skeletal mesh.
	if (Mesh && SightTargetSockets.Num() > 0)
	{
		for (const FName& SocketName : SightTargetSockets)
		{
			if (!Mesh->DoesSocketExist(SocketName))
			{
				continue;
			}

			const FVector SocketLocation = Mesh->GetSocketLocation(SocketName);
			FHitResult HitResult;
			++OutNumberOfLoSChecksPerformed;

			const bool bHit = World->LineTraceSingleByChannel(
				HitResult,
				Context.ObserverLocation,
				SocketLocation,
				TraceChannel,
				QueryParams);

			if (IsTraceClear(bHit, HitResult, OwnerActor))
			{
				OutSeenLocation = SocketLocation;
				OutSightStrength = 1.0f;
				return UAISense_Sight::EVisibilityResult::Visible;
			}
		}
	}

	// 3) Actor origin fallback.
	if (bFallbackToActorLocation)
	{
		const FVector ActorLocation = OwnerActor->GetActorLocation();
		FHitResult HitResult;
		++OutNumberOfLoSChecksPerformed;

		const bool bHit = World->LineTraceSingleByChannel(
			HitResult,
			Context.ObserverLocation,
			ActorLocation,
			TraceChannel,
			QueryParams);

		if (IsTraceClear(bHit, HitResult, OwnerActor))
		{
			OutSeenLocation = ActorLocation;
			OutSightStrength = 1.0f;
			return UAISense_Sight::EVisibilityResult::Visible;
		}
	}

	// Defined value for any caller that reads it regardless of result; the engine
	// reads it only when visible.
	OutSeenLocation = OwnerActor->GetActorLocation();
	return UAISense_Sight::EVisibilityResult::NotVisible;
}

USkeletalMeshComponent* UMetaxisSightTargetComponent::ResolveSkeletalMesh() const
{
	const AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		return nullptr;
	}

	TArray<USkeletalMeshComponent*> SkeletalMeshes;
	OwnerActor->GetComponents<USkeletalMeshComponent>(SkeletalMeshes);

	// 1) The first mesh carrying PreferredMeshTag wins outright. Unlike
	//    UMetaxisEyeSourceComponent, this does not check that the tagged mesh has
	//    the requested sockets; CheckSightTargetConfig reports any it lacks.
	if (!PreferredMeshTag.IsNone())
	{
		for (USkeletalMeshComponent* MeshComp : SkeletalMeshes)
		{
			if (MeshComp && MeshComp->ComponentHasTag(PreferredMeshTag))
			{
				return MeshComp;
			}
		}
	}

	// 2) Character mesh fallback.
	if (const ACharacter* OwnerCharacter = Cast<ACharacter>(OwnerActor))
	{
		USkeletalMeshComponent* CharMesh = OwnerCharacter->GetMesh();
		if (CharMesh)
		{
			return CharMesh;
		}
	}

	// 3) First available skeletal mesh.
	for (USkeletalMeshComponent* MeshComp : SkeletalMeshes)
	{
		if (MeshComp)
		{
			return MeshComp;
		}
	}

	return nullptr;
}

void UMetaxisSightTargetComponent::CheckSightTargetConfig(const AActor* OwnerActor, const USkeletalMeshComponent* ResolvedMesh)
{
#if !UE_BUILD_SHIPPING
	if (bSightTargetConfigChecked)
	{
		return;
	}
	bSightTargetConfigChecked = true;

	if (!IsValid(OwnerActor))
	{
		return;
	}

	TArray<FString> Problems;

	// A tag nobody carries is the common mistake: it went on the actor, on a
	// component that is not a scene component, or was mistyped.
	if (SightTargetComponentTags.Num() > 0)
	{
		TInlineComponentArray<USceneComponent*> SceneComponents(OwnerActor);
		for (const FName& Tag : SightTargetComponentTags)
		{
			if (Tag.IsNone())
			{
				Problems.Add(TEXT("Sight Target Component Tags contains a blank entry, which matches nothing."));
				continue;
			}

			bool bMatched = false;
			for (const USceneComponent* Candidate : SceneComponents)
			{
				if (IsValid(Candidate) && Candidate->IsRegistered() && Candidate->ComponentHasTag(Tag))
				{
					bMatched = true;
					break;
				}
			}

			if (!bMatched)
			{
				Problems.Add(FString::Printf(
					TEXT("No registered scene component on this actor carries the tag '%s'. Tags go in the component's Component Tags array, not in the actor's Tags."),
					*Tag.ToString()));
			}
		}
	}

	// Sockets are only ever read from one mesh. Naming the mesh that won makes
	// the wrong-mesh case diagnosable from the log.
	if (SightTargetSockets.Num() > 0)
	{
		if (!ResolvedMesh)
		{
			Problems.Add(TEXT("Sight Target Sockets are set but no skeletal mesh was found on this actor, so no socket can be traced."));
		}
		else
		{
			for (const FName& SocketName : SightTargetSockets)
			{
				if (!ResolvedMesh->DoesSocketExist(SocketName))
				{
					Problems.Add(FString::Printf(
						TEXT("Socket or bone '%s' does not exist on the resolved mesh '%s' (asset '%s'). Only one mesh supplies sockets; if they live on another mesh, set Preferred Mesh Tag to that mesh."),
						*SocketName.ToString(), *ResolvedMesh->GetName(), *GetNameSafe(ResolvedMesh->GetSkeletalMeshAsset())));
				}
			}
		}
	}

	if (SightTargetComponentTags.Num() == 0 && SightTargetSockets.Num() == 0 && !bFallbackToActorLocation)
	{
		Problems.Add(TEXT("No Sight Target Component Tags, no Sight Target Sockets, and Fallback To Actor Location is off, so this actor can never be seen. Add a trace point or turn the fallback on."));
	}

	// The engine binds one IAISightTargetInterface component per actor and
	// ignores the rest, and which one it binds is not under the purchaser's control.
	{
		const TArray<UActorComponent*> InterfaceComponents = OwnerActor->GetComponentsByInterface(UAISightTargetInterface::StaticClass());
		if (InterfaceComponents.Num() > 1)
		{
			TArray<FString> Names;
			for (const UActorComponent* Component : InterfaceComponents)
			{
				Names.Add(GetNameSafe(Component));
			}
			Problems.Add(FString::Printf(
				TEXT("This actor has %d components implementing IAISightTargetInterface (%s). AI sight consults only one of them, and which one is not configurable. Keep one."),
				InterfaceComponents.Num(), *FString::Join(Names, TEXT(", "))));
		}
	}

	if (Problems.Num() == 0)
	{
		return;
	}

	for (const FString& Problem : Problems)
	{
		UE_LOG(LogTemp, Warning, TEXT("Metaxis Sight Target '%s' on '%s': %s"), *GetName(), *OwnerActor->GetName(), *Problem);
	}

	const UWorld* World = GetWorld();
	if (GEngine && World && World->WorldType == EWorldType::PIE)
	{
		GEngine->AddOnScreenDebugMessage(-1, 12.0f, FColor::Orange,
			FString::Printf(TEXT("Metaxis: Sight Target on '%s' has %d configuration problem(s). See the Output Log."),
				*OwnerActor->GetName(), Problems.Num()));
	}
#endif
}
