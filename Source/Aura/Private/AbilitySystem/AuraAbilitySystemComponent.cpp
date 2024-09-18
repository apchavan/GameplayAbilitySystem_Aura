// Copyright - Amey Chavan


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AuraGameplayTags.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::EffectApplied);

	const FAuraGameplayTags& AuraGameplayTags = FAuraGameplayTags::Get();
	GEngine->AddOnScreenDebugMessage(
		-1,
		10.0f,
		FColor::Orange,
		FString::Printf(TEXT("Tag: %s"), *AuraGameplayTags.Attributes_Secondary_Armor.ToString())
	);
}

void UAuraAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent,
                                                const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);

	EffectAssetTags.Broadcast(TagContainer);
}
