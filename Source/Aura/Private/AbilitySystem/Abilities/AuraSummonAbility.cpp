// Copyright - Amey Chavan


#include "AbilitySystem/Abilities/AuraSummonAbility.h"

#include "Kismet/KismetSystemLibrary.h"

TArray<FVector> UAuraSummonAbility::GetSpawnLocations()
{
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	const float DeltaSpread = SpawnSpread / NumMinions;

	const FVector LeftOfSpread = Forward.RotateAngleAxis(-SpawnSpread / 2.0f, FVector::UpVector);
	TArray<FVector> SpawnLocations;

	for (int32 i = 0; i < NumMinions; i++)
	{
		const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
		const FVector ChosenSpawnLocation = Location + Direction * FMath::FRandRange(MinSpawnDistance, MaxSpawnDistance);
		SpawnLocations.Add(ChosenSpawnLocation);

		DrawDebugSphere(
			GetWorld(),
			ChosenSpawnLocation,
			18.0f,
			12,
			FColor::Cyan,
			false,
			3.0f
		);
		UKismetSystemLibrary::DrawDebugArrow(
			GetAvatarActorFromActorInfo(),
			Location,
			Location + Direction * MaxSpawnDistance,
			4.0f,
			FLinearColor::Green,
			3.0f
		);
		DrawDebugSphere(
			GetWorld(),
			Location + Direction * MinSpawnDistance,
			5.0f,
			12,
			FColor::Red,
			false,
			3.0f
		);
		DrawDebugSphere(
			GetWorld(),
			Location + Direction * MaxSpawnDistance,
			5.0f,
			12,
			FColor::Red,
			false,
			3.0f
		);
	}

	return SpawnLocations;
}
