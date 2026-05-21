// Copyright - Amey Chavan


#include "AbilitySystem/Abilities/AuraPassiveAbility.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"


void UAuraPassiveAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                          const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(
			GetAbilitySystemComponentFromActorInfo()
		)
	)
	{
		/** Bind the callback only if it is already NOT bound. */
		if (!AuraASC->DeactivatePassiveAbility.IsBoundToObject(this))
		{
			AuraASC->DeactivatePassiveAbility.AddUObject(this, &UAuraPassiveAbility::ReceiveDeactivate);
		}
	}
}

void UAuraPassiveAbility::ReceiveDeactivate(const FGameplayTag& AbilityTag)
{
	if (AbilityTags.HasTagExact(AbilityTag))
	{
		/**
		 * Unbind/remove the callback in case if the ability is NOT the type of "Instanced Per Actor".
		 * That's because with "Instanced Per Actor" abilities, the binding happens once with its first activation.
		 * But for "Instanced Per Execution" abilities, it's necessary to unbind to avoid having same callback triggered multiple times.
		 */
		if (InstancingPolicy != EGameplayAbilityInstancingPolicy::InstancedPerActor)
		{
			if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(
					GetAbilitySystemComponentFromActorInfo()
				)
			)
			{
				AuraASC->DeactivatePassiveAbility.RemoveAll(this);
			}
		}

		EndAbility(
			CurrentSpecHandle,
			CurrentActorInfo,
			CurrentActivationInfo,
			true,
			false
		);
	}
}
