#pragma once

#include "CoreMinimal.h"
#include "SimpleEventTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SimpleEventSubsystem.generated.h"

UCLASS()
class SIMPLEEVENTPLUGIN_API USimpleEventSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/**
	 * Sends an event to all listeners. The event is not replicated.
	 *
	 * @param EventTag The gameplay tag identifying the event. (mandatory)
	 * @param DomainTag The domain tag categorizing the event. (optional)
	 * @param Payload The payload of the event as an instanced struct (optional).
	 */
	UFUNCTION(BlueprintCallable, Category = "SimpleEventSubsystem")
	void SendEvent(FGameplayTag EventTag, FGameplayTag DomainTag, const FInstancedStruct& Payload);

	/**
	 * Register a listener to receive events. The listener will be notified when an event is sent that matches the provided filters.
	 *
	 * @param Listener The object listening for the event (mandatory).
	 * @param EventFilter Only listen for events with tags matching this filter. If not set, the listener will accept all events.
	 * @param DomainFilter Only listen for events with domains matching this filter. If not set, the listener will accept events from all domains.
	 * @param EventReceivedDelegate The FGlobalEventDelegate that gets called when the event is received and passes the filters (mandatory).
  	 * @param RequiredPayloadType Only respond to the event if there is a payload present and it is of this type. If not set, the listener will accept events with any (or no) payload.
	 * @param OnlyMatchExactEvent If true, only listen for events that match the filter tags exactly. i.e "A.B" will only match "A.B" and not "A.B.C".
	 * @param OnlyMatchExactDomain If true, only listen for events that match the domain tags exactly. i.e "A.B" will only match "A.B" and not "A.B.C".
	 */
	UFUNCTION(BlueprintCallable, Category = "SimpleEventSubsystem", meta=(AdvancedDisplay=4))
	void ListenForEvent(
		UObject* Listener,
		FGameplayTagContainer EventFilter,
		FGameplayTagContainer DomainFilter,
		FSimpleEventDelegate EventReceivedDelegate,
		UScriptStruct* RequiredPayloadType,
		bool OnlyMatchExactEvent = true,
		bool OnlyMatchExactDomain = true);

	/**
	 * Stop listening for an event on a listener.
	 *
	 * @param Listener The object listening for the event (mandatory).
	 * @param EventReceivedDelegate Stop listening for events that call this delegate (mandatory).
	 */
	UFUNCTION(BlueprintCallable, Category = "SimpleEventSubsystem")
	void StopListeningForEventDelegate(UObject* Listener, FSimpleEventDelegate EventReceivedDelegate);

	/**
	 * Stop listening for events on a listener that match the provided filters.
	 *
	 * @param Listener The object listening for the event (mandatory).
	 * @param EventTagFilter Stop listening for these events (mandatory).
	 * @param DomainTagFilter Only stop listening if the domain also matches (optional). If not set, the listener will stop listening for events in EventTagFilter across all domains.
	 */
	UFUNCTION(BlueprintCallable, Category = "SimpleEventSubsystem")
	void StopListeningForEventsFiltered(UObject* Listener, FGameplayTagContainer EventTagFilter, FGameplayTagContainer DomainTagFilter);

private:
	// List of event subscriptions
	TArray<FEventSubscription> Subscriptions;
};
