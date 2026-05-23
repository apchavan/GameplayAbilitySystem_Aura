// Copyright - Amey Chavan

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NiagaraComponent.h"
#include "DebuffNiagaraComponent.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UDebuffNiagaraComponent : public UNiagaraComponent
{
	GENERATED_BODY()

public:

	UDebuffNiagaraComponent();

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	FGameplayTag DebuffTag;

protected:

	void DebuffTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	UFUNCTION()
	void OnOwnerDeath(AActor* DeadActor);
};
