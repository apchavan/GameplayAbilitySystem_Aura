// Copyright - Amey Chavan


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Player/AuraPlayerState.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

	OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	AuraPlayerState->OnXPChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnXPChanged);
	AuraPlayerState->OnLevelChangedDelegate.AddLambda(
		[this](int32 NewLevel)
		{
			OnPlayerLevelChangedDelegate.Broadcast(NewLevel);
		}
	);

	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AuraAttributeSet->GetHealthAttribute()
	).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnHealthChanged.Broadcast(Data.NewValue);
		}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AuraAttributeSet->GetMaxHealthAttribute()
	).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnMaxHealthChanged.Broadcast(Data.NewValue);
		}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AuraAttributeSet->GetManaAttribute()
	).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnManaChanged.Broadcast(Data.NewValue);
		}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AuraAttributeSet->GetMaxManaAttribute()
	).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnMaxManaChanged.Broadcast(Data.NewValue);
		}
	);

	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		if (AuraASC->bStartupAbilitiesGiven)
		{
			/**
			 * Case [1]:
			 * Since the startup abilities were already given and `AbilitiesGivenDelegate` was already broadcasted,
			 * we just manually call the callback function directly, instead of binding to its
			 * `AbilitiesGivenDelegate` delegate.
			 */
			OnInitializeStartupAbilities(AuraASC);
		}
		else
		{
			/**
			 * Case [2]:
			 * The startup abilities are NOT given yet, so we bind the callback function to its
			 * `AbilitiesGivenDelegate` delegate.
			 */
			AuraASC->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::OnInitializeStartupAbilities);
		}

		AuraASC->EffectAssetTags.AddLambda(
			[this](const FGameplayTagContainer& AssetTags)
			{
				for (const FGameplayTag& Tag : AssetTags)
				{
					/**
					 * For example, say that Tag = Message.HealthPotion
					 * "Message.HealthPotion".MatchesTag("Message") will return True,
					 * "Message".MatchesTag("Message.HealthPotion") will return False.
					 */

					FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
					if (Tag.MatchesTag(MessageTag))
					{
						const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
						MessageWidgetRowDelegate.Broadcast(*Row);
					}
				}
			}
		);
	}
}

void UOverlayWidgetController::OnInitializeStartupAbilities(UAuraAbilitySystemComponent* AuraAbilitySystemComponent)
{
	// Return early if the startup abilities are not given.
	if (!AuraAbilitySystemComponent->bStartupAbilitiesGiven) return;

	// TODO: Get information about all given abilities, look up their Ability Info, and broadcast it to widgets.
	FForEachAbility BroadcastDelegate;

	BroadcastDelegate.BindLambda(
		[this, AuraAbilitySystemComponent](const FGameplayAbilitySpec& AbilitySpec)
		{
			// TODO: Need a way to figure out the ability tag for a given ability spec.
			FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(
				AuraAbilitySystemComponent->GetAbilityTagFromSpec(AbilitySpec)
			);
			Info.InputTag = AuraAbilitySystemComponent->GetInputTagFromSpec(AbilitySpec);

			AbilityInfoDelegate.Broadcast(Info);
		}
	);

	AuraAbilitySystemComponent->ForEachAbility(BroadcastDelegate);
}

void UOverlayWidgetController::OnXPChanged(int32 NewXP) const
{
	const AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	const ULevelUpInfo* LevelUpInfo = AuraPlayerState->LevelUpInfo;
	checkf(LevelUpInfo, TEXT("Unable to find LevelUpInfo. Please fill out AuraPlayerState Blueprint."));

	const int32 Level = LevelUpInfo->FindLevelForXP(NewXP);
	const int32 MaxLevel = LevelUpInfo->LevelUpInformation.Num();

	if (Level <= MaxLevel && Level > 0)
	{
		const int32 LevelUpRequirement = LevelUpInfo->LevelUpInformation[Level].LevelUpRequirement;
		const int32 PreviousLevelUpRequirement = LevelUpInfo->LevelUpInformation[Level - 1].LevelUpRequirement;

		// Delta is required to know what to divide our current XP value by.
		const int32 DeltaLevelRequirement = LevelUpRequirement - PreviousLevelUpRequirement;

		// Calculate the XP we have for the current level because `NewXP` i.e. received in parameter is a cumulative XP,
		// and we just need to know how much XP we have w.r.t. this current level.
		// So we take the difference between `NewXP` & `PreviousLevelUpRequirement`.
		const int32 XPForThisLevel = NewXP - PreviousLevelUpRequirement;

		// Percentage is the division between `XPForThisLevel` and `DeltaLevelRequirement`.
		const float XPBarPercent = static_cast<float>(XPForThisLevel) / static_cast<float>(DeltaLevelRequirement);

		OnXPPercentChangedDelegate.Broadcast(XPBarPercent);
	}
}
