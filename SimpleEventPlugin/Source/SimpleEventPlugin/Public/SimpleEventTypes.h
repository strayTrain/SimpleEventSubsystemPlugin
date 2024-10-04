#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InstancedStruct.h"
#include "SimpleEventTypes.generated.h"

DECLARE_DYNAMIC_DELEGATE_ThreeParams(
	FSimpleEventDelegate,
	FGameplayTag, EventTag,
	FGameplayTag, Domain,
	FInstancedStruct, Payload);

USTRUCT()
struct FEventSubscription
{
	GENERATED_BODY()

	// The tags this subscription is listening for (e.g "UI.ButtonClicked" or "Game.PlayerDied") 
	UPROPERTY()
	FGameplayTagContainer EventTags;

	UPROPERTY()
	bool OnlyMatchExactEvent = true;

	// The domains that the event belongs to (e.g "Domains.UI" or "Domains.Game")
	UPROPERTY()
	FGameplayTagContainer DomainTags;

	UPROPERTY()
	bool OnlyMatchExactDomain = true;
	
	// The object to call the delegate on
	UPROPERTY()
	TWeakObjectPtr<UObject> ListenerObject;

	UPROPERTY()
	bool OnlyMatchExactStructType = true;

	// Only call the CallbackDelegate if the payload is one of these types. If empty, any payload is accepted.
	UPROPERTY()
	UScriptStruct* RequiredPayloadType;

	// The actual delegate that gets called when the event is received
	UPROPERTY()
	FSimpleEventDelegate CallbackDelegate;
};