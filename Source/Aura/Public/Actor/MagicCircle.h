// Copyright - Amey Chavan

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MagicCircle.generated.h"


class USphereComponent;

UCLASS()
class AURA_API AMagicCircle : public AActor
{
	GENERATED_BODY()

public:

	AMagicCircle();
	virtual void Tick(float DeltaTime) override;

	void SetHighlightSphereRadiusAndDecalSize(const float InRadiusSize = 0.0f) const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> TargetsHighlightSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UDecalComponent> MagicCircleDecal;

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnTargetsHighlightSphereBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnTargetsHighlightSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);
};
