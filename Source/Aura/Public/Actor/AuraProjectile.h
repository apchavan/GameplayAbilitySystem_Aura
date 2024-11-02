// Copyright - Amey Chavan

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AuraProjectile.generated.h"

class UNiagaraSystem;
class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class AURA_API AAuraProjectile : public AActor
{
	GENERATED_BODY()

public:

	AAuraProjectile();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

protected:

	virtual void BeginPlay() override;
	virtual void Destroyed() override;

	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	/**
	 * On the client, either `OnSphereOverlap()` function will be called first or the act of destruction (with `Destroyed()`)
	 * will replicate down to client will happen first.
	 *
	 * NOTE: Once `Destroyed()` function gets called after that `OnSphereOverlap()` function will never get called.
	 *
	 * This boolean is used to handle that odd case when the act of destruction (i.e. call to `Destroyed()`)
	 * could replicate down the client before that client had a chance to have its `OnSphereOverlap()` function called;
	 * in that case the projectile would be destroyed before it could play the cosmetic effects like playing the sound
	 * & spawning the Niagara system.
	 *
	 * To avoid such case on the client, within `OnSphereOverlap()` function if we overlap before being `Destroyed()`
	 * we set this boolean to `true` indicating the cosmetic effects were already played.
	 *
	 * So once the act of destruction is replicated, we can check whether these cosmetic effects were already played &
	 * if not we can play them.
	 */
	bool bHit = false;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> ImpactEffect;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> ImpactSound;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> LoopingSound;

	UPROPERTY()
	TObjectPtr<UAudioComponent> LoopingSoundComponent;

	UPROPERTY(EditDefaultsOnly)
	float LifeSpan = 15.0f;
};
