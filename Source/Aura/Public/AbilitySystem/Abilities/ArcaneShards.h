// Copyright - Amey Chavan

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "ArcaneShards.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UArcaneShards : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()

public:

	virtual FString GetDescription(int32 Level) override;
	virtual FString GetNextLevelDescription(int32 Level) override;

	/** Total number of maximum Arcane Shards that can be spawned. Max number can be 11. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 1, ClampMax = 11, UIMin = 1, UIMax = 11))
	int32 MaxNumShards = 11;
};
