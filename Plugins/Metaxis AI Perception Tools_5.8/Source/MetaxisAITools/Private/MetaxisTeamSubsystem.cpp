// Copyright Metaxis Games 2026, All Rights Reserved

#include "MetaxisTeamSubsystem.h"
#include "Engine/DataTable.h"
#include "Engine/World.h"
#include "Perception/AIPerceptionComponent.h"
#include "UObject/UObjectIterator.h"

void UMetaxisTeamSubsystem::PostInitialize()
{
	Super::PostInitialize();

	FGenericTeamId::SetAttitudeSolver([WeakThis = MakeWeakObjectPtr(this)](FGenericTeamId A, FGenericTeamId B)
	{
		if (WeakThis.IsValid())
		{
			return WeakThis->ResolveAttitude(A, B);
		}
		// Subsystem already gone, so mirror ResolveAttitude's NoTeam-as-Neutral fallback.
		if (A == FGenericTeamId::NoTeam || B == FGenericTeamId::NoTeam)
		{
			return ETeamAttitude::Neutral;
		}
		return A != B ? ETeamAttitude::Hostile : ETeamAttitude::Friendly;
	});
}
void UMetaxisTeamSubsystem::Deinitialize()
{
	FGenericTeamId::ResetAttitudeSolver();
	Super::Deinitialize();
}

bool UMetaxisTeamSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

void UMetaxisTeamSubsystem::SetTeamRelationship(uint8 TeamA, uint8 TeamB, TEnumAsByte<ETeamAttitude::Type> NewAttitude, bool bRefreshListeners)
{
	for (FMetaxisTeamRelationship& Existing : TeamRelationships)
	{
		if ((Existing.TeamA == TeamA && Existing.TeamB == TeamB)
			|| (Existing.TeamA == TeamB && Existing.TeamB == TeamA))
		{
			Existing.Attitude = NewAttitude;
			UE_LOG(LogTemp, Log, TEXT("SetTeamRelationship: Updated relationship between Team %d and Team %d to %s."),
				TeamA, TeamB, *UEnum::GetValueAsString(NewAttitude.GetValue()));

			if (bRefreshListeners)
			{
				RefreshAllPerceptionListeners();
			}
			return;
		}
	}

	FMetaxisTeamRelationship NewEntry;
	NewEntry.TeamA = TeamA;
	NewEntry.TeamB = TeamB;
	NewEntry.Attitude = NewAttitude;
	TeamRelationships.Add(NewEntry);

	UE_LOG(LogTemp, Log, TEXT("SetTeamRelationship: Added relationship between Team %d and Team %d as %s."),
		TeamA, TeamB, *UEnum::GetValueAsString(NewAttitude.GetValue()));

	if (bRefreshListeners)
	{
		RefreshAllPerceptionListeners();
	}
}

TEnumAsByte<ETeamAttitude::Type> UMetaxisTeamSubsystem::GetTeamRelationship(uint8 TeamA, uint8 TeamB) const
{
	return ResolveAttitude(FGenericTeamId(TeamA), FGenericTeamId(TeamB));
}

bool UMetaxisTeamSubsystem::RemoveTeamRelationship(uint8 TeamA, uint8 TeamB, bool bRefreshListeners)
{
	for (int32 i = TeamRelationships.Num() - 1; i >= 0; --i)
	{
		const FMetaxisTeamRelationship& Entry = TeamRelationships[i];
		if ((Entry.TeamA == TeamA && Entry.TeamB == TeamB)
			|| (Entry.TeamA == TeamB && Entry.TeamB == TeamA))
		{
			TeamRelationships.RemoveAt(i);
			UE_LOG(LogTemp, Log, TEXT("RemoveTeamRelationship: Removed relationship between Team %d and Team %d."), TeamA, TeamB);

			if (bRefreshListeners)
			{
				RefreshAllPerceptionListeners();
			}
			return true;
		}
	}
	return false;
}

void UMetaxisTeamSubsystem::LoadRelationshipsFromDataTable(UDataTable* DataTable, bool bRefreshListeners)
{
	if (!DataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("LoadRelationshipsFromDataTable: Invalid DataTable provided."));
		return;
	}

	TArray<FMetaxisTeamRelationship*> Rows;
	DataTable->GetAllRows<FMetaxisTeamRelationship>(TEXT("UMetaxisTeamSubsystem::LoadRelationshipsFromDataTable"), Rows);

	int32 AppliedCount = 0;
	for (const FMetaxisTeamRelationship* Row : Rows)
	{
		if (Row)
		{
			// Defer the refresh until after the whole table is applied to avoid a refresh-per-row storm.
			SetTeamRelationship(Row->TeamA.GetId(), Row->TeamB.GetId(), Row->Attitude, /*bRefreshListeners=*/false);
			++AppliedCount;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("LoadRelationshipsFromDataTable: Loaded %d rows from DataTable %s."), Rows.Num(), *DataTable->GetName());

	if (bRefreshListeners && AppliedCount > 0)
	{
		RefreshAllPerceptionListeners();
	}
}

void UMetaxisTeamSubsystem::ClearAllRelationships(bool bRefreshListeners)
{
	const bool bHadRelationships = TeamRelationships.Num() > 0;

	TeamRelationships.Empty();
	UE_LOG(LogTemp, Log, TEXT("ClearAllRelationships: All team relationships cleared."));

	if (bRefreshListeners && bHadRelationships)
	{
		RefreshAllPerceptionListeners();
	}
}

void UMetaxisTeamSubsystem::RefreshAllPerceptionListeners()
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	int32 RefreshedCount = 0;
	for (TObjectIterator<UAIPerceptionComponent> It; It; ++It)
	{
		UAIPerceptionComponent* PerceptionComp = *It;
		if (IsValid(PerceptionComp)
			&& !PerceptionComp->IsTemplate()
			&& PerceptionComp->GetWorld() == World)
		{
			PerceptionComp->RequestStimuliListenerUpdate();
			++RefreshedCount;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("RefreshAllPerceptionListeners: Requested a listener update on %d perception component(s)."), RefreshedCount);
}

ETeamAttitude::Type UMetaxisTeamSubsystem::ResolveAttitude(FGenericTeamId A, FGenericTeamId B) const
{
	// An actor with no team assigned (NoTeam / 255) has no allegiance, so treat it as Neutral
	// toward everything, matching the engine's default convention. Without this guard the
	// same-ID rule below would resolve two unassigned actors (both 255) as Friendly.
	if (A == FGenericTeamId::NoTeam || B == FGenericTeamId::NoTeam)
	{
		return ETeamAttitude::Neutral;
	}

	for (const FMetaxisTeamRelationship& Relationship : TeamRelationships)
	{
		if ((Relationship.TeamA == A && Relationship.TeamB == B)
			|| (Relationship.TeamA == B && Relationship.TeamB == A))
		{
			return Relationship.Attitude;
		}
	}

	return A != B ? ETeamAttitude::Hostile : ETeamAttitude::Friendly;
}
