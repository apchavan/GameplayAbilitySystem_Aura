// Copyright - Amey Chavan


#include "Actor/AuraFireBall.h"

#include "GameFramework/ProjectileMovementComponent.h"

AAuraFireBall::AAuraFireBall()
{
	ProjectileMovement->PrimaryComponentTick.bCanEverTick = false;
	ProjectileMovement->PrimaryComponentTick.bStartWithTickEnabled = false;
	ProjectileMovement->SetAutoActivate(false);
	ProjectileMovement->InitialSpeed = 0.0f;
	ProjectileMovement->MaxSpeed = 0.0f;
}

void AAuraFireBall::BeginPlay()
{
	Super::BeginPlay();

	StartOutgoingTimeline();
}

void AAuraFireBall::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}
