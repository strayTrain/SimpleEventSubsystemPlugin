#include "SimpleEventSubSystem.h"

void USimpleEventSubsystem::SendEvent(FGameplayTag EventTag, FGameplayTag DomainTag, const FInstancedStruct& Payload)
{
	// If we come across an invalid listener on a subscription (e.g the listener was garbage collected and forgot to unsubscribe)
	// we'll remove that subscription. We store the indexes of the invalid listeners here.
	TArray<int32> InvalidListenerIndexes;

	// We check subscriptions from the most recently added to the oldest
	for (int32 i = Subscriptions.Num() - 1; i >= 0; --i)
	{
		FEventSubscription& Subscription = Subscriptions[i];

		const UObject* Listener = Subscription.ListenerObject.Get();
		
		if (!Listener)
		{
			InvalidListenerIndexes.Add(i);
			continue;
		}

		if (Subscription.EventTags.IsValid() && !Subscription.EventTags.HasTag(EventTag))
		{
			continue;
		}

		if (Subscription.DomainTags.IsValid() && !Subscription.DomainTags.HasTag(DomainTag))
		{
			continue;
		}

		bool wasCalled = Subscription.CallbackDelegate.ExecuteIfBound(EventTag, DomainTag, Payload);

		if (!wasCalled)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to call delegate for Listener %s"), *Listener->GetName());
		}
	}

	for (const int32 i : InvalidListenerIndexes)
	{
		Subscriptions.RemoveAt(i);
	}
}

void USimpleEventSubsystem::ListenForEvent(UObject* Listener, FGameplayTagContainer EventFilter, FGameplayTagContainer DomainFilter, FSimpleEventDelegate EventReceivedDelegate)
{
	if (!Listener)
	{
		UE_LOG(LogTemp, Warning, TEXT("Null Listener passed to ListenForEvent. Aborting."));
		return;
	}

	FEventSubscription Subscription;
	Subscription.ListenerObject = Listener;
	Subscription.CallbackDelegate = EventReceivedDelegate;
	Subscription.EventTags.AppendTags(EventFilter);
	Subscription.DomainTags.AppendTags(DomainFilter);

	Subscriptions.Add(Subscription);
}

void USimpleEventSubsystem::StopListeningForEventDelegate(UObject* Listener, FSimpleEventDelegate EventReceivedDelegate)
{
	Subscriptions.RemoveAll([Listener, EventReceivedDelegate](const FEventSubscription& Subscription)
	{
		return Subscription.ListenerObject == Listener && Subscription.CallbackDelegate == EventReceivedDelegate;
	});
}

void USimpleEventSubsystem::StopListeningForEventsFiltered(UObject* Listener, FGameplayTagContainer EventTagFilter, FGameplayTagContainer DomainTagFilter)
{
	Subscriptions.RemoveAll([Listener, EventTagFilter, DomainTagFilter](const FEventSubscription& Subscription)
	{
		return Subscription.ListenerObject == Listener &&
			(!EventTagFilter.Num() || EventTagFilter.HasAny(Subscription.EventTags)) &&
			(!DomainTagFilter.Num() || DomainTagFilter.HasAny(Subscription.DomainTags));
	});
}
