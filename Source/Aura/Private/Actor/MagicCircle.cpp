// Copyright - Amey Chavan


#include "Actor/MagicCircle.h"

#include "Components/DecalComponent.h"
#include "Components/SphereComponent.h"
#include "Interaction/EnemyInterface.h"


AMagicCircle::AMagicCircle()
{
	PrimaryActorTick.bCanEverTick = true;

	/**
	 * To avoid the following kind of warnings,
	 * 
	 * Warning      LogActor                  BP_MagicCircle_C /Game/Maps/Dungeon.Dungeon:PersistentLevel.BP_MagicCircle_C_3 has natively added scene component(s), but none of them were set as the actor's RootComponent - picking one arbitrarily
	 * Warning      LogActor                  BP_MagicCircle_C /Game/Maps/UEDPIE_0_Dungeon.Dungeon:PersistentLevel.BP_MagicCircle_C_0 has natively added scene component(s), but none of them were set as the actor's RootComponent - picking one arbitrarily
	 * 
	 * and to retain the possibility to actors transform we do not set `MagicCircleDecal` directly as root.
	 * Instead, we create the default scene component as root and then attach the `MagicCircleDecal` to it.
	 */
	SetRootComponent(CreateDefaultSubobject<USceneComponent>("RootSceneComponent"));

	TargetsHighlightSphere = CreateDefaultSubobject<USphereComponent>("TargetsHighlightSphere");
	TargetsHighlightSphere->SetupAttachment(GetRootComponent());
	TargetsHighlightSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	TargetsHighlightSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	MagicCircleDecal = CreateDefaultSubobject<UDecalComponent>("MagicCircleDecal");
	MagicCircleDecal->SetupAttachment(GetRootComponent());
}

void AMagicCircle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMagicCircle::SetHighlightSphereRadiusAndDecalSize(const float InRadiusSize) const
{
	if (InRadiusSize > 0.0f)
	{
		TargetsHighlightSphere->SetSphereRadius(InRadiusSize);
		MagicCircleDecal->DecalSize = FVector(InRadiusSize);
	}
}

void AMagicCircle::BeginPlay()
{
	Super::BeginPlay();

	TargetsHighlightSphere->OnComponentBeginOverlap.AddDynamic(this, &AMagicCircle::OnTargetsHighlightSphereBeginOverlap);
	TargetsHighlightSphere->OnComponentEndOverlap.AddDynamic(this, &AMagicCircle::OnTargetsHighlightSphereEndOverlap);
}

void AMagicCircle::OnTargetsHighlightSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(OtherActor); IEnemyInterface* EnemyInterface = Cast<IEnemyInterface>(OtherActor))
	{
		EnemyInterface->HighlightActor();
	}
}

void AMagicCircle::OnTargetsHighlightSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (IsValid(OtherActor); IEnemyInterface* EnemyInterface = Cast<IEnemyInterface>(OtherActor))
	{
		EnemyInterface->UnHighlightActor();
	}
}
