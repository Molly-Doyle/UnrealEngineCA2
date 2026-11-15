// Copyright Metaxis Games 2026, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MetaxisAIController.generated.h"

class UMetaxisEyeSourceComponent;

/**
 * AI Controller that drives the agent's eye viewpoint from a socket on the possessed pawn's skeletal mesh, through its built-in Metaxis Eye Source Component, so sight follows the mesh animation instead of the collision capsule. It also mirrors the possessed pawn's team through IGenericTeamAgentInterface, so the perception listener reports the same team the pawn carries, and refreshes that listener on possession. For the eye viewpoint, this controller with a standard pawn is enough, as is a standard AI controller with a Metaxis Pawn or Character; for team affiliation, pair this controller with a Metaxis Pawn or Character, whose team it mirrors.
 */
UCLASS()
class METAXISAITOOLS_API AMetaxisAIController : public AAIController
{
    GENERATED_BODY()

public:
    AMetaxisAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;

    // Mirror the possessed pawn's team so the perception listener (owned by this controller)
    // reports the same team the pawn carries through IGenericTeamAgentInterface.
    virtual FGenericTeamId GetGenericTeamId() const override;
    virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;

    // Refresh the perception listener after possession so its cached team picks up the pawn's.
    virtual void OnPossess(APawn* InPawn) override;

protected:

    /**
     * The built-in Metaxis Eye Source Component that supplies this controller's eye viewpoint. Configure the socket name and the override toggles on the component itself.
     */
    UPROPERTY(Category = "AI", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UMetaxisEyeSourceComponent> EyeSourceComponent;
};