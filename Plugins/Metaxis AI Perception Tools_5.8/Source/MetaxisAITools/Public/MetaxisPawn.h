// Copyright Metaxis Games 2026, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GenericTeamAgentInterface.h"
#include "MetaxisPawn.generated.h"

class UMetaxisEyeSourceComponent;
class UMetaxisTeamComponent;

/**
 * Pawn base class with a built-in Metaxis Eye Source Component that drives the eye viewpoint from a skeletal mesh socket, and a built-in Metaxis Team Component that backs its IGenericTeamAgentInterface, so native AI Perception Detect by Affiliation reads the team set in Blueprint. A standard AI Controller is enough for the eye viewpoint; pair it with the Metaxis AI Controller so the listener side reports the same team. Do not add a second Metaxis Team Component, which shadows the built-in one and makes the team read as NoTeam.
 */
UCLASS()
class METAXISAITOOLS_API AMetaxisPawn : public APawn, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:

	AMetaxisPawn();

	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;

	// IGenericTeamAgentInterface: sources the team ID from the built-in TeamComponent so
	// native AI Perception "Detect by Affiliation" works with the team set in Blueprint.
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/**
	 * The built-in Metaxis Eye Source Component that supplies this pawn's eye viewpoint. Configure the socket name and the override toggles on the component itself.
	 */
	UPROPERTY(Category = "AI", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMetaxisEyeSourceComponent> EyeSourceComponent;

	// Single source of truth for this actor's team. Read by GetGenericTeamId so native perception affiliation resolves through the interface.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMetaxisTeamComponent> TeamComponent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
