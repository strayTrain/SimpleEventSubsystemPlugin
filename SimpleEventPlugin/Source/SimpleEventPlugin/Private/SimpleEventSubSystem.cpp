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
		
		if (Subscription.EventTags.IsValid())
		{
			if (Subscription.OnlyMatchExactEvent && !Subscription.EventTags.HasTagExact(EventTag))
			{
				continue;
			}

			if (!Subscription.OnlyMatchExactEvent && !Subscription.EventTags.HasTag(EventTag))
			{
				continue;
			}
		}

		if (Subscription.DomainTags.IsValid())
		{
			if (Subscription.OnlyMatchExactDomain && !Subscription.DomainTags.HasTagExact(DomainTag))
			{
				continue;
			}

			if (!Subscription.OnlyMatchExactDomain && !Subscription.DomainTags.HasTag(DomainTag))
			{
				continue;
			}
		}

		if (Subscription.RequiredPayloadType)
		{
			if (!Payload.IsValid())
			{
				UE_LOG(
					LogTemp, Warning,
					TEXT("No payload passed for Listener %s but the listener is expecting %s"),
					*Listener->GetName(), *Subscription.RequiredPayloadType->GetName());
				continue;	
			}
			
			if (Payload.GetScriptStruct() != Subscription.RequiredPayloadType)
			{
				UE_LOG(
					LogTemp, Warning,
					TEXT("Payload type %s does not match Listener expected type %s"),
					*Payload.GetScriptStruct()->GetName(), *Subscription.RequiredPayloadType->GetName());
				continue;
			}
		}

		bool WasCalled = Subscription.CallbackDelegate.ExecuteIfBound(EventTag, DomainTag, Payload);

		if (!WasCalled)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to call delegate for Listener %s"), *Listener->GetName());
		}
	}

	for (const int32 i : InvalidListenerIndexes)
	{
		Subscriptions.RemoveAt(i);
	}
}

void USimpleEventSubsystem::ListenForEvent(UObject* Listener, FGameplayTagContainer EventFilter,
	FGameplayTagContainer DomainFilter, FSimpleEventDelegate EventReceivedDelegate, UScriptStruct* RequiredPayloadType,
	bool OnlyMatchExactEvent, bool OnlyMatchExactDomain)
{
	if (!Listener)
	{
		UE_LOG(LogTemp, Warning, TEXT("Null Listener passed to ListenForEvent. Can't listen for event."));
		return;
	}

	if (!EventReceivedDelegate.IsBound())
	{
		UE_LOG(LogTemp, Warning, TEXT("No delegate bound to ListenForEvent. Can't listen for event."));
		return;
	}

	FEventSubscription Subscription;
	Subscription.ListenerObject = Listener;
	Subscription.CallbackDelegate = EventReceivedDelegate;
	Subscription.EventTags.AppendTags(EventFilter);
	Subscription.OnlyMatchExactEvent = OnlyMatchExactEvent;
	Subscription.DomainTags.AppendTags(DomainFilter);
	Subscription.OnlyMatchExactDomain = OnlyMatchExactDomain;
	Subscription.RequiredPayloadType = RequiredPayloadType;

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
