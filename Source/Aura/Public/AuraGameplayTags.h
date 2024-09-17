// Copyright - Amey Chavan

#pragma once

#include "CoreMinimal.h"

/**
 * AuraGameplayTags
 *
 * Singleton containing native Gameplay Tags
 */
struct FAuraGameplayTags
{
public:
 static const FAuraGameplayTags& Get() { return GameplayTags; }
 static void InitializeNativeGameplayTags();

protected:

private:
 static FAuraGameplayTags GameplayTags;
};
