// Copyright Metaxis Games 2026, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GenericTeamAgentInterface.h"
#include "MetaxisTeamRelationship.h"
#include "MetaxisTeamSubsystem.generated.h"

class UDataTable;

/**
 * World subsystem that manages team relationships and installs a custom attitude solver into FGenericTeamId. This enables complex multi-team relationships (e.g. Team 1 ↔ Team 2: Hostile, Team 1 ↔ Team 3: Friendly) that the engine's default solver does not support.
 */
UCLASS()
class METAXISAITOOLS_API UMetaxisTeamSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	// USubsystem interface
	virtual void PostInitialize() override;
	virtual void Deinitialize() override;

	/**
	 * Set or update the relationship between two teams. The relationship is symmetric: setting TeamA→TeamB also applies to TeamB→TeamA. Refreshes perception listeners by default so the change takes effect at runtime.
	 */
	UFUNCTION(BlueprintCallable, Category = "Metaxis AI Perception Tools|Team Relationship", meta = (DisplayName = "Set Team Relationship"))
	void SetTeamRelationship(uint8 TeamA, uint8 TeamB, TEnumAsByte<ETeamAttitude::Type> NewAttitude, bool bRefreshListeners = true);

	/**
	 * Get the relationship between two teams. Returns Friendly if same team, Hostile if different with no explicit relationship set.
	 */
	UFUNCTION(BlueprintPure, Category = "Metaxis AI Perception Tools|Team Relationship", meta = (DisplayName = "Get Team Relationship"))
	TEnumAsByte<ETeamAttitude::Type> GetTeamRelationship(uint8 TeamA, uint8 TeamB) const;

	/**
	 * Remove an explicitly-defined relationship between two teams, reverting to default behaviour. Refreshes perception listeners by default (only when an entry was actually removed).
	 */
	UFUNCTION(BlueprintCallable, Category = "Metaxis AI Perception Tools|Team Relationship", meta = (DisplayName = "Remove Team Relationship"))
	bool RemoveTeamRelationship(uint8 TeamA, uint8 TeamB, bool bRefreshListeners = true);

	/**
	 * Bulk-load relationships from a DataTable whose row struct is FMetaxisTeamRelationship. Existing relationships are preserved; rows in the table overwrite conflicting entries. Refreshes perception listeners once after loading by default (only when at least one row applied).
	 */
	UFUNCTION(BlueprintCallable, Category = "Metaxis AI Perception Tools|Team Relationship", meta = (DisplayName = "Load Relationships From DataTable"))
	void LoadRelationshipsFromDataTable(UDataTable* DataTable, bool bRefreshListeners = true);

	/**
	 * Remove all explicitly-defined relationships, reverting entirely to default behaviour. Refreshes perception listeners by default (only when the table was non-empty).
	 */
	UFUNCTION(BlueprintCallable, Category = "Metaxis AI Perception Tools|Team Relationship", meta = (DisplayName = "Clear All Relationships"))
	void ClearAllRelationships(bool bRefreshListeners = true);

	/**
	 * Force every AI Perception listener in this world to re-evaluate its affiliation filter. Call after changing team relationships at runtime so already-registered listeners pick up the new attitudes. The relationship setters above call this automatically by default.
	 */
	UFUNCTION(BlueprintCallable, Category = "Metaxis AI Perception Tools|Team Relationship", meta = (DisplayName = "Refresh All Perception Listeners"))
	void RefreshAllPerceptionListeners();

protected:

	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

private:

	UPROPERTY()
	TArray<FMetaxisTeamRelationship> TeamRelationships;

	ETeamAttitude::Type ResolveAttitude(FGenericTeamId A, FGenericTeamId B) const;
};
