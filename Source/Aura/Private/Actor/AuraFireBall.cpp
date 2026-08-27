// Copyright - Amey Chavan


#include "Actor/AuraFireBall.h"

#include "GameFramework/ProjectileMovementComponent.h"

AAuraFireBall::AAuraFireBall()
{
	ProjectileMovement->InitialSpeed = 1600.0f;
	ProjectileMovement->MaxSpeed = 1600.0f;
}

void AAuraFireBall::BeginPlay()
{
	Super::BeginPlay();
}

void AAuraFireBall::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}
