// Copyright - Amey Chavan


#include "Actor/MagicCircle.h"

#include "Components/DecalComponent.h"


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

	MagicCircleDecal = CreateDefaultSubobject<UDecalComponent>("MagicCircleDecal");
	MagicCircleDecal->SetupAttachment(GetRootComponent());
}

void AMagicCircle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMagicCircle::BeginPlay()
{
	Super::BeginPlay();
}
