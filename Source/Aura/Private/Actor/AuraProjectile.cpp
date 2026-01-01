// Copyright - Amey Chavan


#include "Actor/AuraProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Aura/Aura.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AAuraProjectile::AAuraProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere);
	Sphere->SetCollisionObjectType(ECC_Projectile);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->InitialSpeed = 550.0f;
	ProjectileMovement->MaxSpeed = 550.0f;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
}

void AAuraProjectile::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(LifeSpan);

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectile::OnSphereOverlap);

	/**
	 * Make a looping sound that'll be played once projectile is spawned.
	 * The returned `UAudioComponent` is useful to start/stop playing the sound whenever required.
	 */
	LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent());

	/** Automatically set to stop the sound when this projectile actor is destroyed. */
	if (IsValid(LoopingSoundComponent))
	{
		LoopingSoundComponent->bStopWhenOwnerDestroyed = true;
	}
}

void AAuraProjectile::Destroyed()
{
	/** Handle cosmetic effects only if this is running on the client & already NOT hit. */
	if (!bHit && !HasAuthority()) OnHit();
	Super::Destroyed();
}

void AAuraProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();

	/**
	 * If the `SourceAvatarActor` and the `OtherActor` are the same, then we don't consider it and return.
	 */
	if (SourceAvatarActor == OtherActor) return;

	/**
	 * Do not apply damage if the `SourceAvatarActor` and `OtherActor` are from the same team, or they are friends.
	 * This will avoid damaging between enemy versus enemy and player versus player.
	 */
	if (!UAuraAbilitySystemLibrary::IsNotFriend(SourceAvatarActor, OtherActor)) return;

	/** Handle cosmetic effects only if already NOT hit. */
	if (!bHit) OnHit();

	if (HasAuthority())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			const FVector DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
			DamageEffectParams.DeathImpulse = DeathImpulse;

			/**
			 * It is important to set the `TargetAbilitySystemComponent` before applying the damage
			 * because it may not be set before when creating the `DamageEffectParams`.
			 */
			DamageEffectParams.TargetAbilitySystemComponent = TargetASC;

			UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
		}

		/** Destroy the object since we're on the server. */
		Destroy();
	}
	else bHit = true; /** Set this on the client. */
}

void AAuraProjectile::OnHit()
{
	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());

	if (IsValid(LoopingSoundComponent) && LoopingSoundComponent->IsPlaying())
	{
		LoopingSoundComponent->Stop();
	}

	bHit = true;
}
