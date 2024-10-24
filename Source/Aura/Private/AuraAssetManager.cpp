// Copyright - Amey Chavan


#include "AuraAssetManager.h"
#include "AbilitySystemGlobals.h"
#include "AuraGameplayTags.h"

UAuraAssetManager& UAuraAssetManager::Get()
{
	check(GEngine);

	UAuraAssetManager* AuraAssetManager = Cast<UAuraAssetManager>(GEngine->AssetManager);
	return *AuraAssetManager;
}

void UAuraAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	FAuraGameplayTags::InitializeNativeGameplayTags();

	/**
	 * This is required to use Target Data!
	 *
	 * Starting from Unreal Engine 5.3, this gets called automatically & may not be required to call manually like this.
	 * Reference: https://github.com/tranek/GASDocumentation?tab=readme-ov-file#491-initglobaldata
	 */
	UAbilitySystemGlobals::Get().InitGlobalData();
}
