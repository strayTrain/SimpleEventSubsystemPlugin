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
	 * @param EventTag The gameplay tag identifying the event (mandatory).
	 * @param DomainTag The domain tag categorizing the event (optional).
	 * @param Payload The payload of the event as an instanced struct (optional).
	 */
	UFUNCTION(BlueprintCallable, Category = "SimpleEventSubsystem")
	void SendEvent(FGameplayTag EventTag, FGameplayTag DomainTag, const FInstancedStruct& Payload);

	/**
	 * Register a listener to receive events. The listener will be notified when an event is sent that matches the provided filters.
	 *
	 * @param Listener The object listening for the event (mandatory).
	 * @param EventFilter Only listen for events with tags matching this filter (mandatory).
	 * @param DomainFilter Only listen for events with domains matching this filter (optional). If not set, the listener will receive events from all domains.
	 * @param EventReceivedDelegate The FGlobalEventDelegate that gets called when the event is received and passes the filters (mandatory).
	 */
	UFUNCTION(BlueprintCallable, Category = "SimpleEventSubsystem")
	void ListenForEvent(UObject* Listener, FGameplayTagContainer EventFilter, FGameplayTagContainer DomainFilter, FSimpleEventDelegate EventReceivedDelegate);

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
