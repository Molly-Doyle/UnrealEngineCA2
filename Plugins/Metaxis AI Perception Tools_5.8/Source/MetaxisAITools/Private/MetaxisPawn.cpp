// Copyright Metaxis Games 2026, All Rights Reserved


#include "MetaxisPawn.h"
#include "MetaxisEyeSourceComponent.h"
#include "MetaxisTeamComponent.h"


AMetaxisPawn::AMetaxisPawn()
{

	PrimaryActorTick.bCanEverTick = true;

	EyeSourceComponent = CreateDefaultSubobject<UMetaxisEyeSourceComponent>(TEXT("EyeSourceComponent"));

	TeamComponent = CreateDefaultSubobject<UMetaxisTeamComponent>(TEXT("TeamComponent"));

}

void AMetaxisPawn::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{

	if (IsValid(EyeSourceComponent) && EyeSourceComponent->GetCustomEyesViewPoint(this, OutLocation, OutRotation))
	{
		return;
	}

	Super::GetActorEyesViewPoint(OutLocation, OutRotation);
}

FGenericTeamId AMetaxisPawn::GetGenericTeamId() const
{
	if (IsValid(TeamComponent))
	{
		return FGenericTeamId(TeamComponent->TeamID);
	}

	return FGenericTeamId::NoTeam;
}

void AMetaxisPawn::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	if (IsValid(TeamComponent))
	{
		// Route through SwitchTeam so the team-change log line is shared.
		TeamComponent->SwitchTeam(NewTeamID.GetId());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AMetaxisPawn '%s': SetGenericTeamId called but TeamComponent is invalid."), *GetName());
	}
}

// Called when the game starts or when spawned
void AMetaxisPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMetaxisPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMetaxisPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

