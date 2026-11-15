// Copyright Metaxis Games 2026, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MetaxisTeamComponent.generated.h"

/**
 * Stores a team ID on any actor without requiring it to implement IGenericTeamAgentInterface. Native AI Perception reads teams through that interface only, so the stored value reaches affiliation filtering when the owning class implements it, as the Metaxis Character and Pawn do; on any other class the component is storage, read by the Get and Set Actor Team Affiliation nodes.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class METAXISAITOOLS_API UMetaxisTeamComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMetaxisTeamComponent();

	/**
	 * Numeric team ID stored on this actor, defaulting to 255 (NoTeam). Native AI Perception reads teams only through IGenericTeamAgentInterface, so this value reaches affiliation filtering only when the owning class implements that interface, as the Metaxis Character and Pawn do; on its own the component is storage. Use Switch Team to change it at runtime.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Metaxis AI Perception Tools|Team Component")
	uint8 TeamID;

	/**
	 * Sets Team ID at runtime and logs the change. When the owner is a possessed pawn, it also requests a stimuli listener update on the controller's perception component, so affiliation re-filters immediately.
	 */
	UFUNCTION(BlueprintCallable, Category = "Metaxis AI Perception Tools|Team Component", meta = (DisplayName = "Switch Team"))
	void SwitchTeam(uint8 NewTeamID);
};
