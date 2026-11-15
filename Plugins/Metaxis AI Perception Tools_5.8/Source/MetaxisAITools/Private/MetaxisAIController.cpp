// Copyright Metaxis Games 2026, All Rights Reserved

#include "MetaxisAIController.h"
#include "MetaxisEyeSourceComponent.h"
#include "GameFramework/Pawn.h"
#include "Perception/AIPerceptionComponent.h"

AMetaxisAIController::AMetaxisAIController(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    EyeSourceComponent = CreateDefaultSubobject<UMetaxisEyeSourceComponent>(TEXT("EyeSourceComponent"));
}

void AMetaxisAIController::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
    APawn* ControlledPawn = GetPawn();
    if (!ControlledPawn)
    {
        Super::GetActorEyesViewPoint(OutLocation, OutRotation);
        return;
    }

    if (EyeSourceComponent && EyeSourceComponent->GetCustomEyesViewPoint(ControlledPawn, OutLocation, OutRotation))
    {
        return; // Controller's component handled the override.
    }

    Super::GetActorEyesViewPoint(OutLocation, OutRotation);
}

FGenericTeamId AMetaxisAIController::GetGenericTeamId() const
{
    // Derive the listener's team from the possessed pawn's team component (via the interface).
    if (const IGenericTeamAgentInterface* PawnTeamAgent = Cast<IGenericTeamAgentInterface>(GetPawn()))
    {
        const FGenericTeamId PawnTeam = PawnTeamAgent->GetGenericTeamId();
        if (PawnTeam != FGenericTeamId::NoTeam)
        {
            return PawnTeam;
        }
    }

    return Super::GetGenericTeamId();
}

void AMetaxisAIController::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
    // Forward to the possessed pawn so the team component remains the single source of truth.
    if (IGenericTeamAgentInterface* PawnTeamAgent = Cast<IGenericTeamAgentInterface>(GetPawn()))
    {
        PawnTeamAgent->SetGenericTeamId(NewTeamID);
        return;
    }

    Super::SetGenericTeamId(NewTeamID);
}

void AMetaxisAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    // The listener caches its team on registration; refresh it now that a pawn (and its
    // team) is possessed so affiliation filtering uses the correct team.
    if (IsValid(GetPerceptionComponent()))
    {
        GetPerceptionComponent()->RequestStimuliListenerUpdate();
    }
}