// Copyright Metaxis Games 2026, All Rights Reserved

#pragma once

#include "Runtime/Launch/Resources/Version.h"
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "MetaxisBlueprintFunctionLibrary.generated.h"

class UAIPerceptionComponent;
class AActor;

/**
 * Blueprint Function Library for Metaxis AI Perception Tools.
 */
UCLASS()
class METAXISAITOOLS_API UMetaxisBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	// Forget Functions
	
	/**
	 * Clears the perception component's memory of a single actor, then requests a stimuli listener update. On UE 5.4 and 5.5 it also broadcasts OnTargetPerceptionForgotten once, working around an engine delegate issue. Logs a warning and makes no change if the perception component or the actor is invalid.
	 */
	UFUNCTION(BlueprintCallable, Category = "Metaxis AI Perception Tools", meta=(DisplayName = "AI Forget Actor"))
	static void AIForgetActor(UAIPerceptionComponent* AIPerceptionComponent, AActor* ActorToForget);
	
	/**
	 * Clears the perception component's memory of every valid actor in the array, then requests a stimuli listener update. The array is copied internally so delegate side effects cannot corrupt the loop. Logs a warning and makes no change if the perception component is invalid.
	 */
	UFUNCTION(BlueprintCallable, Category = "Metaxis AI Perception Tools", meta=(DisplayName = "AI Forget Actors"))
	static void AIForgetActors(UAIPerceptionComponent* AIPerceptionComponent, const TArray<AActor*>& ActorsToForget);

	// Set Sight Config Functions
	
	/**
	 * Sets the sight sense's peripheral half angle in degrees on the first sight config found on the component, clamped to 0 to 180. Reconfigures the sense and requests a stimuli listener update on success. Logs a warning and makes no change if the component is invalid or has no sight config.
	 */
	UFUNCTION(BlueprintCallable, Category = "Metaxis AI Perception Tools|Sight Config", meta=(DisplayName = "Set Vision Cone Half Angle"))
	static void SetVisionConeHalfAngle(UAIPerceptionComponent* AIPerceptionComponent, float NewHalfAngle);
	
	/**
	 * Sets how far the AI can see within the cone, on the first sight config found on the component, with negative values raised to 0. Reconfigures the sense and requests a stimuli listener update on success. Logs a warning and makes no change if the component is invalid or has no sight config.
	 */
	UFUNCTION(BlueprintCallable, Category = "Metaxis AI Perception Tools|Sight Config", meta=(DisplayName = "Set Vision Cone Sight Radius"))
	static void SetVisionConeSightRadius(UAIPerceptionComponent* AIPerceptionComponent, float NewRadius);
	
	/**
	 * Sets the radius beyond the sight radius where a target is still tracked before being lost, on the first sight config found on the component, with negative values raised to 0. Reconfigures the sense and requests a stimuli listener update on success. Logs a warning and makes no change if the component is invalid or has no sight config.
	 */
	UFUNCTION(BlueprintCallable, Category = "Metaxis AI Perception Tools|Sight Config", meta=(DisplayName = "Set Vision Cone Lose Sight Radius"))
	static void SetVisionConeLoseSightRadius(UAIPerceptionComponent* AIPerceptionComponent, float NewLoseSightRadius);
	
	/**
	 * Moves the sight origin backward along the forward view axis on the first sight config found on the component, with negative values raised to 0. Reconfigures the sense and requests a stimuli listener update on success. Logs a warning and makes no change if the component is invalid or has no sight config.
	 */
	UFUNCTION(BlueprintCallable, Category = "Metaxis AI Perception Tools|Sight Config", meta=(DisplayName = "Set Point Of View Backward Offset"))
	static void SetPointOfViewBackwardOffset(UAIPerceptionComponent* AIPerceptionComponent, float NewOffset);
	
	/**
	 * Sets the inner radius where sight stimuli are ignored, on the first sight config found on the component, with negative values raised to 0. Reconfigures the sense and requests a stimuli listener update on success. Logs a warning and makes no change if the component is invalid or has no sight config.
	 */
	UFUNCTION(BlueprintCallable, Category = "Metaxis AI Perception Tools|Sight Config", meta=(DisplayName = "Set Near Clipping Radius"))
	static void SetNearClippingRadius(UAIPerceptionComponent* AIPerceptionComponent, float NewRadius);

	/**
	 * Sets how long sight stimuli remain valid before expiring, on the first sight config found on the component, with negative values raised to 0. Reconfigures the sense and requests a stimuli listener update on success. Logs a warning and makes no change if the component is invalid or has no sight config.
	 */
	UFUNCTION(BlueprintCallable, Category = "Metaxis AI Perception Tools|Sight Config", meta=(DisplayName = "Set Sight Sense Max Age"))
	static void SetSightSenseMaxAge(UAIPerceptionComponent* AIPerceptionComponent, float NewMaxAge);

	// Get Sight Config Functions

	/**
	 * Returns the sight sense's peripheral half angle in degrees from the first sight config found on the component. Returns 0 and logs a warning if the component is invalid or has no sight config.
	 */
	UFUNCTION(BlueprintPure, Category = "Metaxis AI Perception Tools|Sight Config", meta=(DisplayName = "Get Vision Cone Half Angle"))
	static float GetVisionConeHalfAngle(UAIPerceptionComponent* AIPerceptionComponent);
	
	/**
	 * Returns the sight radius from the first sight config found on the component. Returns 0 and logs a warning if the component is invalid or has no sight config.
	 */
	UFUNCTION(BlueprintPure, Category = "Metaxis AI Perception Tools|Sight Config", meta=(DisplayName = "Get Vision Cone Sight Radius"))
	static float GetVisionConeSightRadius(UAIPerceptionComponent* AIPerceptionComponent);
	
	/**
	 * Returns the lose sight radius from the first sight config found on the component. Returns 0 and logs a warning if the component is invalid or has no sight config.
	 */
	UFUNCTION(BlueprintPure, Category = "Metaxis AI Perception Tools|Sight Config", meta=(DisplayName = "Get Vision Cone Lose Sight Radius"))
	static float GetVisionConeLoseSightRadius(UAIPerceptionComponent* AIPerceptionComponent);

	/**
	 * Returns the sight origin's backward offset along the forward view axis from the first sight config found on the component. Returns 0 and logs a warning if the component is invalid or has no sight config.
	 */
	UFUNCTION(BlueprintPure, Category = "Metaxis AI Perception Tools|Sight Config", meta=(DisplayName = "Get Point Of View Backward Offset"))
	static float GetPointOfViewBackwardOffset(UAIPerceptionComponent* AIPerceptionComponent);

	/**
	 * Returns the near clipping radius from the first sight config found on the component. Returns 0 and logs a warning if the component is invalid or has no sight config.
	 */
	UFUNCTION(BlueprintPure, Category = "Metaxis AI Perception Tools|Sight Config", meta=(DisplayName = "Get Near Clipping Radius"))
	static float GetNearClippingRadius(UAIPerceptionComponent* AIPerceptionComponent);

	/**
	 * Returns the sight sense max age from the first sight config found on the component. A configured Max Age of 0 is reported as the largest float, which is the engine's never-expires value. Returns 0 and logs a warning if the component is invalid or has no sight config.
	 */
	UFUNCTION(BlueprintPure, Category = "Metaxis AI Perception Tools|Sight Config", meta=(DisplayName = "Get Sight Sense Max Age"))
	static float GetSightSenseMaxAge(UAIPerceptionComponent* AIPerceptionComponent);

	// Hearing Config Functions
	
	/**
	 * Returns the hearing sense's max distance from the first hearing config found on the component. Returns 0 and logs a warning if the component is invalid or has no hearing config.
	 */
	UFUNCTION(BlueprintPure, Category = "Metaxis AI Perception Tools|Hearing Config", meta=(DisplayName = "Get Hearing Range"))
	static float GetHearingRange(UAIPerceptionComponent* AIPerceptionComponent);

	/**
	 * Sets the hearing range on the first hearing config found on the component, with negative values raised to 0. Reconfigures the sense and requests a stimuli listener update on success. Logs a warning and makes no change if the component is invalid or has no hearing config.
	 */
	UFUNCTION(BlueprintCallable, Category = "Metaxis AI Perception Tools|Hearing Config", meta=(DisplayName = "Set Hearing Range"))
	static void SetHearingRange(UAIPerceptionComponent* AIPerceptionComponent, float NewHearingRange);

	/**
	 * Returns the hearing sense max age from the first hearing config found on the component. A configured Max Age of 0 is reported as the largest float, which is the engine's never-expires value. Returns 0 and logs a warning if the component is invalid or has no hearing config.
	 */
	UFUNCTION(BlueprintPure, Category = "Metaxis AI Perception Tools|Hearing Config", meta=(DisplayName = "Get Hearing Sense Max Age"))
	static float GetHearingSenseMaxAge(UAIPerceptionComponent* AIPerceptionComponent);

	/**
	 * Sets how long hearing stimuli remain valid, on the first hearing config found on the component, with negative values raised to 0. Reconfigures the sense and requests a stimuli listener update on success. Logs a warning and makes no change if the component is invalid or has no hearing config.
	 */
	UFUNCTION(BlueprintCallable, Category = "Metaxis AI Perception Tools|Hearing Config", meta=(DisplayName = "Set Hearing Sense Max Age"))
	static void SetHearingSenseMaxAge(UAIPerceptionComponent* AIPerceptionComponent, float NewMaxAge);
	
	// Damage Config Functions

	/**
	 * Returns the damage sense max age from the first damage config found on the component. A configured Max Age of 0 is reported as the largest float, which is the engine's never-expires value. Returns 0 and logs a warning if the component is invalid or has no damage config.
	 */
	UFUNCTION(BlueprintPure, Category = "Metaxis AI Perception Tools|Damage Config", meta=(DisplayName = "Get Damage Sense Max Age"))
	static float GetDamageSenseMaxAge(UAIPerceptionComponent* AIPerceptionComponent);

	/**
	 * Sets how long damage stimuli remain valid, on the first damage config found on the component, with negative values raised to 0. Reconfigures the sense and requests a stimuli listener update on success. Logs a warning and makes no change if the component is invalid or has no damage config.
	 */
	UFUNCTION(BlueprintCallable, Category = "Metaxis AI Perception Tools|Damage Config", meta=(DisplayName = "Set Damage Sense Max Age"))
	static void SetDamageSenseMaxAge(UAIPerceptionComponent* AIPerceptionComponent, float NewMaxAge);

	// Touch Config Functions

	/**
	 * Returns the touch sense max age from the first touch config found on the component. A configured Max Age of 0 is reported as the largest float, which is the engine's never-expires value. Returns 0 and logs a warning if the component is invalid or has no touch config.
	 */
	UFUNCTION(BlueprintPure, Category = "Metaxis AI Perception Tools|Touch Config", meta=(DisplayName = "Get Touch Sense Max Age"))
	static float GetTouchSenseMaxAge(UAIPerceptionComponent* AIPerceptionComponent);

	/**
	 * Sets how long touch stimuli remain valid, on the first touch config found on the component, with negative values raised to 0. Reconfigures the sense and requests a stimuli listener update on success. Logs a warning and makes no change if the component is invalid or has no touch config.
	 */
	UFUNCTION(BlueprintCallable, Category = "Metaxis AI Perception Tools|Touch Config", meta=(DisplayName = "Set Touch Sense Max Age"))
	static void SetTouchSenseMaxAge(UAIPerceptionComponent* AIPerceptionComponent, float NewMaxAge);

	// Team Affiliation Functions
	
	/**
	 * Outputs whether enemies, neutrals and friendlies are detected for the given sense class on this component. Only sight, hearing and touch configs expose affiliation. All three outputs are false when the component or sense class is invalid, or the config does not expose affiliation.
	 */
	UFUNCTION(BlueprintPure, Category = "Metaxis AI Perception Tools|Team Affiliation", meta = (DisplayName = "Get Detection By Affiliation"))
	static void GetDetectionByAffiliation(UAIPerceptionComponent* AIPerceptionComponent, TSubclassOf<UAISense> SenseClass, bool& bDetectEnemies, bool& bDetectNeutrals, bool& bDetectFriendlies);

	/**
	 * Writes the enemies, neutrals and friendlies detection flags for the given sense class on this component, then reconfigures the sense and requests a stimuli listener update. Only sight, hearing and touch configs expose affiliation. Makes no change if the component or sense class is invalid, or the config does not expose affiliation.
	 */
	UFUNCTION(BlueprintCallable, Category = "Metaxis AI Perception Tools|Team Affiliation", meta = (DisplayName = "Set Detection By Affiliation"))
	static void SetDetectionByAffiliation(UAIPerceptionComponent* AIPerceptionComponent, TSubclassOf<UAISense> SenseClass, bool bDetectEnemies, bool bDetectNeutrals, bool bDetectFriendlies);

	/**
	 * Returns the team ID from IGenericTeamAgentInterface on the actor or, for a pawn, on its controller. Falls back to the Team ID on a Metaxis Team Component on the actor or its controller when the interface is absent or reports no team. Returns 255 (NoTeam) and logs a warning when neither is present.
	 */
	UFUNCTION(BlueprintPure, Category = "Metaxis AI Perception Tools|Team Affiliation", meta=(DisplayName = "Get Actor Team Affiliation"))
	static uint8 GetActorTeamAffiliation(AActor* TargetActor);

	/**
	 * Writes the team ID to a Metaxis Team Component on the actor or, for a pawn, on its controller, and also sets it through IGenericTeamAgentInterface when the actor or its controller implements that interface. Both are written when both are present. Logs a warning and makes no change when neither is present.
	 */
	UFUNCTION(BlueprintCallable, Category = "Metaxis AI Perception Tools|Team Affiliation", meta=(DisplayName = "Set Actor Team Affiliation"))
	static void SetActorTeamAffiliation(AActor* TargetActor, uint8 NewTeamID);
	
	// General Perception Functions

	/**
	 * Returns the perception component's dominant sense class. Returns an unset class and logs a warning if the component is invalid.
	 */
	UFUNCTION(BlueprintPure, Category = "Metaxis AI Perception Tools|General", meta=(DisplayName = "Get Dominant Sense"))
	static TSubclassOf<UAISense> GetDominantSense(UAIPerceptionComponent* AIPerceptionComponent);

	/**
	 * Assigns the dominant sense class on the component, or clears it when the class is unset. Logs a warning and makes no change if the component is invalid. Unlike the sense config setters, this does not reconfigure senses or request a stimuli listener update.
	 */
	UFUNCTION(BlueprintCallable, Category = "Metaxis AI Perception Tools|General", meta=(DisplayName = "Set Dominant Sense"))
	static void SetDominantSense(UAIPerceptionComponent* AIPerceptionComponent, TSubclassOf<UAISense> NewDominantSense);
};