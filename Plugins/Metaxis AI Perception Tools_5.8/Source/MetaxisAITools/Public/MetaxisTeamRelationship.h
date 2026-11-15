// Copyright Metaxis Games 2026, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Engine/DataTable.h"
#include "MetaxisTeamRelationship.generated.h"

/**
 * Row struct for a DataTable of team pairs, for bulk loading through Load Relationships From DataTable on the Metaxis Team Subsystem. Each row names two teams and the attitude between them. Relationships are symmetric, so one row defines both directions.
 */
USTRUCT(BlueprintType)
struct METAXISAITOOLS_API FMetaxisTeamRelationship : public FTableRowBase
{
	GENERATED_BODY()

	/**
	 * First team in the pair, as a numeric team ID. Relationships are symmetric, so which team goes in Team A and which in Team B does not matter.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Metaxis AI Perception Tools|Team Relationship")
	FGenericTeamId TeamA;

	/**
	 * Second team in the pair, as a numeric team ID. Relationships are symmetric, so which team goes in Team A and which in Team B does not matter.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Metaxis AI Perception Tools|Team Relationship")
	FGenericTeamId TeamB;

	/**
	 * How the two teams regard each other: Friendly, Neutral or Hostile. Defaults to Hostile. With no row for a pair, the subsystem treats the same team as Friendly and different teams as Hostile.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Metaxis AI Perception Tools|Team Relationship")
	TEnumAsByte<ETeamAttitude::Type> Attitude = ETeamAttitude::Hostile;

	/**
	 * Authoring notes for this row. Not read by the plugin and has no runtime effect.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Metaxis AI Perception Tools|Team Relationship")
	FString DeveloperComments;
};
