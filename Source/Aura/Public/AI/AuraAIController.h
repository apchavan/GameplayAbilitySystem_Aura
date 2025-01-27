// Copyright - Amey Chavan

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AuraAIController.generated.h"

class UBehaviorTreeComponent;

/**
 * 
 */
UCLASS()
class AURA_API AAuraAIController : public AAIController
{
	GENERATED_BODY()

public:

	AAuraAIController();

protected:

	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;
};
