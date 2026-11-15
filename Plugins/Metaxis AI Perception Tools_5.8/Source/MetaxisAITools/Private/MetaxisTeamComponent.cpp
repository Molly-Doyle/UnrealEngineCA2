// Copyright Metaxis Games 2026, All Rights Reserved

#include "MetaxisTeamComponent.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Pawn.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"

UMetaxisTeamComponent::UMetaxisTeamComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	TeamID = FGenericTeamId::NoTeam;
}

void UMetaxisTeamComponent::SwitchTeam(uint8 NewTeamID)
{
	TeamID = NewTeamID;
	UE_LOG(LogTemp, Log, TEXT("UMetaxisTeamComponent '%s': Switched to Team %d on actor %s."),
		*GetName(), NewTeamID, *GetOwner()->GetName());

	// If this component lives on a possessed pawn, refresh the controller's perception
	// listener so the team change re-filters affiliation at runtime. Every cast is guarded.
	if (const APawn* OwnerPawn = Cast<APawn>(GetOwner()))
	{
		if (AAIController* AIController = Cast<AAIController>(OwnerPawn->GetController()))
		{
			if (UAIPerceptionComponent* PerceptionComponent = AIController->GetPerceptionComponent())
			{
				PerceptionComponent->RequestStimuliListenerUpdate();
			}
		}
	}
}
