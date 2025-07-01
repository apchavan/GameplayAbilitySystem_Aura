// Copyright - Amey Chavan


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Aura/AuraLogChannels.h"


void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::ClientEffectApplied);
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		if (const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.DynamicAbilityTags.AddTag(AuraAbility->StartupInputTag);
			GiveAbility(AbilitySpec);
		}
	}

	/**
	 * Set the boolean status to `true` since the startup abilities are now given to this ASC and then do a broadcast
	 * of its `AbilitiesGivenDelegate`.
	 */
	bStartupAbilitiesGiven = true;
	AbilitiesGivenDelegate.Broadcast(this);
}

void UAuraAbilitySystemComponent::AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupPassiveAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		GiveAbilityAndActivateOnce(AbilitySpec);
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputReleased(AbilitySpec);
		}
	}
}

void UAuraAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{
	/**
	 * We do the following operation here in Ability System Component (ASC) and not in the widget controller is because
	 * as we loop over the abilities, we have to be careful because abilities can change status like some of them can
	 * no longer become activatable or maybe they're blocked by some Gameplay Tag.
	 *
	 * So, a good practice when looping over this container is to lock that activatable abilities container until that
	 * loop is done and we do that using `FScopedAbilityListLock`, it requires ASC by reference.
	 * This ensures that the activatable abilities list will be locked and it will keep track of any abilities
	 * that are attempted to be removed or added, and wait until this scope has finished before mutating that list.
	 * This gives us a safe way to do it.
	 */

	FScopedAbilityListLock ActiveScopeLock(*this);

	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!Delegate.ExecuteIfBound(AbilitySpec))
		{
			UE_LOG(
				LogAura,
				Error,
				TEXT("Failed to execute delegate in %hs."),
				__FUNCTION__
			);
		}
	}
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (IsValid(AbilitySpec.Ability))
	{
		for (FGameplayTag Tag : AbilitySpec.Ability.Get()->AbilityTags)
		{
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities"))))
			{
				return Tag;
			}
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag Tag : AbilitySpec.DynamicAbilityTags)
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag"))))
		{
			return Tag;
		}
	}
	return FGameplayTag();
}

void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	/**
	 * Once the abilities are given, then the Ability System Component's (ASC) `ActivatableAbilities` container
	 * (i.e. returned by `GetActivatableAbilities()` function) replicates as it is a replicated variable.
	 * The `OnRep_ActivateAbilities()` is a virtual function that gets called in response to `ActivatableAbilities`
	 * replicating.
	 * So after giving abilities, this is going to replicate and also it contains the ability specs of newly given
	 * abilities. Hence we override `OnRep_ActivateAbilities()` here in our ASC.
	 *
	 * Also, we don't want to broadcast `AbilitiesGivenDelegate` everytime this replicates, but we only want to do this
	 * for the first time we've given abilities. For this, we use `bStartupAbilitiesGiven` and since it's set on server
	 * and also not replicated, down on the client it will be `false` the first time. So we set it to `true` to avoid
	 * broadcasting `AbilitiesGivenDelegate` everytime, except for the first time, as the `ActivatableAbilities`
	 * replicates.
	 */

	if (!bStartupAbilitiesGiven)
	{
		bStartupAbilitiesGiven = true;
		AbilitiesGivenDelegate.Broadcast(this);
	}
}

void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(
	UAbilitySystemComponent* AbilitySystemComponent,
	const FGameplayEffectSpec& EffectSpec,
	FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);

	EffectAssetTags.Broadcast(TagContainer);
}
