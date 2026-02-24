// Copyright - Amey Chavan


#include "AbilitySystem/Abilities/AuraBeamSpell.h"

void UAuraBeamSpell::StoreMouseDataInfo(const FHitResult& HitResult)
{
	if (HitResult.bBlockingHit)
	{
		MouseHitLocation = HitResult.ImpactPoint;
		MouseHitActor = HitResult.GetActor();
	}
	else
	{
		/**
		 * NOTE: Instead of calling the `CancelAbility()` method, we should call the `EndAbility()` method
		 * because that is intended to be called by the ability to end itself.
		 * Also, we pass the `bWasCancelled` parameter with a `false` value becasue we want to indicate that
		 * the ability was ended but not cancelled (instead, it is set to `true` when called from `CancelAbility()`).
		 * 
		 * On the other hand, `CancelAbility()` interrupts the ability (from an outside source).
		 */

		/*
		CancelAbility(
			CurrentSpecHandle,
			CurrentActorInfo,
			CurrentActivationInfo,
			true
		);
		*/

		EndAbility(
			CurrentSpecHandle,
			CurrentActorInfo,
			CurrentActivationInfo,
			true,
			false
		);
	}
}

void UAuraBeamSpell::StoreOwnerPlayerController()
{
	if (CurrentActorInfo)
	{
		OwnerPlayerController = CurrentActorInfo->PlayerController.Get();
	}
}
