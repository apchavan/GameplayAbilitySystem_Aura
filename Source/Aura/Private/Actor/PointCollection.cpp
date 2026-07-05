// Copyright - Amey Chavan


#include "Actor/PointCollection.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

APointCollection::APointCollection()
{
	PrimaryActorTick.bCanEverTick = false;

	Pt_0 = CreateDefaultSubobject<USceneComponent>(TEXT("Pt_0"));
	ImmutablePts.Add(Pt_0);
	SetRootComponent(Pt_0);

	Pt_1 = CreateDefaultSubobject<USceneComponent>(TEXT("Pt_1"));
	ImmutablePts.Add(Pt_1);
	Pt_1->SetupAttachment(GetRootComponent());

	Pt_2 = CreateDefaultSubobject<USceneComponent>(TEXT("Pt_2"));
	ImmutablePts.Add(Pt_2);
	Pt_2->SetupAttachment(GetRootComponent());

	Pt_3 = CreateDefaultSubobject<USceneComponent>(TEXT("Pt_3"));
	ImmutablePts.Add(Pt_3);
	Pt_3->SetupAttachment(GetRootComponent());

	Pt_4 = CreateDefaultSubobject<USceneComponent>(TEXT("Pt_4"));
	ImmutablePts.Add(Pt_4);
	Pt_4->SetupAttachment(GetRootComponent());

	Pt_5 = CreateDefaultSubobject<USceneComponent>(TEXT("Pt_5"));
	ImmutablePts.Add(Pt_5);
	Pt_5->SetupAttachment(GetRootComponent());

	Pt_6 = CreateDefaultSubobject<USceneComponent>(TEXT("Pt_6"));
	ImmutablePts.Add(Pt_6);
	Pt_6->SetupAttachment(GetRootComponent());

	Pt_7 = CreateDefaultSubobject<USceneComponent>(TEXT("Pt_7"));
	ImmutablePts.Add(Pt_7);
	Pt_7->SetupAttachment(GetRootComponent());

	Pt_8 = CreateDefaultSubobject<USceneComponent>(TEXT("Pt_8"));
	ImmutablePts.Add(Pt_8);
	Pt_8->SetupAttachment(GetRootComponent());

	Pt_9 = CreateDefaultSubobject<USceneComponent>(TEXT("Pt_9"));
	ImmutablePts.Add(Pt_9);
	Pt_9->SetupAttachment(GetRootComponent());

	Pt_10 = CreateDefaultSubobject<USceneComponent>(TEXT("Pt_10"));
	ImmutablePts.Add(Pt_10);
	Pt_10->SetupAttachment(GetRootComponent());
}

TArray<USceneComponent*> APointCollection::GetGroundPoints(
	const FVector& GroundLocation, int32 NumPoints, float YawOverride
)
{
	checkf(ImmutablePts.Num() >= NumPoints, TEXT("Attempted to access ImmutablePts out of bounds."));

	TArray<USceneComponent*> ArrayCopy;

	for (USceneComponent* Pt : ImmutablePts)
	{
		if (ArrayCopy.Num() >= NumPoints) return ArrayCopy;

		// Rotate the point using `YawOverride` except for the `Pt_0` (or root component at center).
		if (Pt != Pt_0)
		{
			// The vector from `Pt_0` (or root component at center) to point `Pt` being processed.
			FVector ToPoint = Pt->GetComponentLocation() - Pt_0->GetComponentLocation();

			// Rotate the point using `YawOverride`.
			ToPoint = ToPoint.RotateAngleAxis(YawOverride, FVector::UpVector);

			// Set the new location for `Pt` as per the computed rotation.
			Pt->SetWorldLocation(
				Pt_0->GetComponentLocation() + ToPoint
				/**
				 * Or we can also write this parameter as, `GetActorLocation() + ToPoint`
				 * because it returns the location of the root component of this actor which is `Pt_0`.
				 */
			);
		}

		// The start of the line trace.
		const FVector RaisedLocation = FVector(
			Pt->GetComponentLocation().X,
			Pt->GetComponentLocation().Y,
			Pt->GetComponentLocation().Z + 500.0f
		);
		// The end of the line trace.
		const FVector LoweredLocation = FVector(
			Pt->GetComponentLocation().X,
			Pt->GetComponentLocation().Y,
			Pt->GetComponentLocation().Z - 500.0f
		);

		TArray<AActor*> IgnoreActors;

		// Get all the living enemies/players within the radius & use it to ignore them when doing the line trace later.
		UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(
			this,
			IgnoreActors,
			TArray<AActor*>(),
			1500.0f,
			GetActorLocation()
		);

		FHitResult HitResult;

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActors(IgnoreActors);

		// Perform line trace.
		GetWorld()->LineTraceSingleByProfile(
			HitResult,
			RaisedLocation,
			LoweredLocation,
			FName("BlockAll"),
			QueryParams
		);

		// The location for the point `Pt` by adjusting its "Z" coordinate.
		const FVector AdjustedLocation = FVector(
			Pt->GetComponentLocation().X,
			Pt->GetComponentLocation().Y,
			HitResult.ImpactPoint.Z
		);

		Pt->SetWorldLocation(AdjustedLocation);
		Pt->SetWorldRotation(UKismetMathLibrary::MakeRotFromZ(HitResult.ImpactNormal));

		ArrayCopy.Add(Pt);
	}
	return ArrayCopy;
}

void APointCollection::BeginPlay()
{
	Super::BeginPlay();
}
