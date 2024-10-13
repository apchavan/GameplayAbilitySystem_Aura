// Copyright - Amey Chavan


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/SplineComponent.h"
#include "Input/AuraInputComponent.h"
#include "Interaction/EnemyInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
	AutoRun();
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(AuraContext);

	// Add InputMappingContext to controller.
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	// Instead of using assertion `check()`, here we need to just check whether `Subsystem` is valid to make it work in multi-player.
	if (Subsystem)
	{
		Subsystem->AddMappingContext(AuraContext, 0);
	}

	// Show mouse cursor.
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	/*
	 * Configure the cursor settings.
	 * 
	 * Set InputMode to enable using inputs from keyboard and mouse.
	 * Also be able to affect UI such as widgets.
	 */
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);

	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);

	AuraInputComponent->BindAbilityActions(
		InputConfig,
		this,
		&ThisClass::AbilityInputTagPressed,
		&ThisClass::AbilityInputTagReleased,
		&ThisClass::AbilityInputTagHeld
	);
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardVector, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightVector, InputAxisVector.X);
	}
}

void AAuraPlayerController::CursorTrace()
{
	// Detect the actor under cursor.
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);

	// Return early if there's no blocking hit detected.
	if (!CursorHit.bBlockingHit) return;

	// Store previous actor and current actor under cursor.
	LastActor = ThisActor;
	ThisActor = CursorHit.GetActor();

	if (LastActor != ThisActor)
	{
		if (LastActor) LastActor->UnHighlightActor();
		if (ThisActor) ThisActor->HighlightActor();
	}
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	// Check whether the `InputTag` pressed is LMB, because it can either move character or activate the ability.
	if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		// If `ThisActor` is valid, it means the user is trying to target some actor, like an enemy.
		bTargeting = ThisActor ? true : false;

		// We should not be auto-running because it's not clear yet whether this is a short press.
		// If it is short press, then we can auto-run but here we don't know yet until we release this LMB input.
		bAutoRunning = false;
	}
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (GetASC()) GetASC()->AbilityInputTagReleased(InputTag);
		return;
	}

	if (bTargeting)
	{
		if (GetASC()) GetASC()->AbilityInputTagReleased(InputTag);
	}
	else
	{
		const APawn* ControlledPawn = GetPawn<APawn>();

		// Check whether this was a short press or not, with a valid controlled Pawn.
		if (FollowTime <= ShortPressThreshold && ControlledPawn)
		{
			// Create a navigation path, i.e. a set of points to follow.
			if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, ControlledPawn->GetActorLocation(), CachedDestination))
			{
				Spline->ClearSplinePoints();
				for (const FVector& PointLoc : NavPath->PathPoints)
				{
					Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
				}

				// Override the destination to last point of `NavPath`.
				// This will avoid the issue of un-reachable locations where "NavMeshBoundsVolume" is not covered on the game map.
				CachedDestination = NavPath->PathPoints[NavPath->PathPoints.Num() - 1];

				// Set the auto-running since we have navigation points ready.
				bAutoRunning = true;
			}
		}

		// Reset total pressed time for the LMB input.
		FollowTime = 0.0f;

		// Reset the targeting status.
		bTargeting = false;
	}
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	// If the `InputTag` is NOT the LMB input, we can try to activate the ability.
	// Because here we don't auto-run or move character.
	// Except the LMB input, all other held inputs will be managed here to activate their related abilities.
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (GetASC()) GetASC()->AbilityInputTagHeld(InputTag);
		return;
	}

	// If targeting an actor with LMB input, then still we want to activate the related ability.
	if (bTargeting)
	{
		if (GetASC()) GetASC()->AbilityInputTagHeld(InputTag);
	}
	else
	{
		// Manage click-to-move behavior using LMB input.

		// Add up total time the LMB input is being pressed.
		FollowTime += GetWorld()->GetDeltaSeconds();

		// If this `CursorHit` was a result of blocking collision.
		// We can either use `CursorHit.Location` or `CursorHit.ImpactPoint` because for line trace, they are the same.
		// But for other trace shapes like box/sphere these will be different.
		if (CursorHit.bBlockingHit) CachedDestination = CursorHit.ImpactPoint;

		// Move towards the destination.
		if (APawn* ControlledPawn = GetPawn<APawn>())
		{
			// The normalized vector from `ControlledPawn` to `CachedDestination` indicates world direction.
			const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(WorldDirection);
		}
	}
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
	if (AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}
	return AuraAbilitySystemComponent;
}

void AAuraPlayerController::AutoRun()
{
	if (!bAutoRunning) return;

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		// Find the location on the spline i.e. closest to the Pawn.
		// Because our Pawn may not be exactly on the spline.
		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);

		// Find the direction on the spline that corresponds to this location.
		// Instead of `LocationOnSpline` we can also pass `ControlledPawn->GetActorLocation()`.
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);

		// Move towards the destination.
		ControlledPawn->AddMovementInput(Direction);

		const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();

		// If the current destination distance is within `AutoRunAcceptanceRadius`, stop auto-running.
		if (DistanceToDestination <= AutoRunAcceptanceRadius)
		{
			bAutoRunning = false;
		}
	}
}
