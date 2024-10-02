# Simple Event Plugin

**Simple Event Plugin** is a lightweight Unreal Engine plugin that provides an event dispatching and subscription system. \
Events are categorized by `EventTags` and optionally by `DomainTags`, with an optional payload that can be any UStruct you create in C++ or Blueprints (via [InstancedStruct](https://www.reddit.com/r/unrealengine/comments/1f7o1co/what_is_the_difference_between_a_struct_and_an/)). \
Anything that has access to the GameInstance (e.g Widgets, Pawns, PlayerControllers) can subscribe an event through a subsystem.

![image](https://github.com/user-attachments/assets/45a95682-186d-4c5e-8ffd-388fe1613517)

## Use cases

This plugin allows you to:
- Send events using gameplay tags.
- Subscribe listeners to specific events using gameplay tags.
- Remove listeners from event subscriptions.

## Requirements

- Unreal Engine 5.2* or higher. 
- **GameplayTags** and **StructUtils** modules enabled in your project.
  
*(instanced structs were introduced in 5.0 with the StructUtils plugin and got [replication support in 5.2](https://github.com/EpicGames/UnrealEngine/pull/9280) I believe. Thus, this plugin will technically work with < 5.2 but expect things might not work)

## Installation Steps

1. Download or clone the plugin into your Unreal Engine project, under the [Plugins folder](https://dev.epicgames.com/documentation/en-us/unreal-engine/plugins-in-unreal-engine#pluginfolders) (create it if it doesn't exist.)
2. Enable the plugin in your Unreal Engine project by navigating to **Edit > Plugins** and searching for "SimpleEventPlugin".
3. Rebuild your project.

### Event Flow Overview

1. Use `SendEvent` to dispatch an event with an `EventTag`, optional `DomainTag`, and optional `Payload`.
2. Use `ListenForEvent` to register a listener that reacts when an event matching its `EventTag` and `DomainTag` is sent.
3. Use `StopListeningForEventDelegate` or `StopListeningForEventsFiltered` to unsubscribe listeners.

---

## Usage

### Blueprint Examples

#### Sending Events

In Blueprints, you can easily call the `SendEvent` function on the `SimpleEventSubsystem`.

1. Obtain the `SimpleEventSubsystem` reference by calling `Get Game Instance Subsystem`.
2. Use `Send Event` node.
3. Provide the required `EventTag`, optional `DomainTag`, and optional `Payload`.

![image](https://github.com/user-attachments/assets/33aad8a0-5990-432a-8d0f-02ebd6ce7e38)

#### Listening for Events

To listen for events in Blueprints:

1. Obtain the `SimpleEventSubsystem` reference by calling `Get Game Instance Subsystem`.
2. Use `Listen For Event` node.
3. Bind your custom event to the `EventReceivedDelegate` parameter.
4. Optionally, filter the event using `EventTag` and `DomainTag`.

![Blueprint - Listening for Events](./images/listen_event.png)

#### Unsubscribing from Events

To unsubscribe from events:

1. Obtain the `SimpleEventSubsystem` reference by calling `Get Game Instance Subsystem`.
2. Use either `Stop Listening For Event Delegate` or `Stop Listening For Events Filtered` nodes.

![Blueprint - Unsubscribing from Events](./images/unsubscribe_event.png)

---

### C++ Examples

#### Sending Events

To send an event from any part of your game:

```cpp
#include "SimpleEventSubsystem.h"
#include "GameplayTagContainer.h"
#include "InstancedStruct.h"

void YourFunctionToSendEvent(UWorld* World)
{
    if (USimpleEventSubsystem* EventSubsystem = World->GetGameInstance()->GetSubsystem<USimpleEventSubsystem>())
    {
        FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(TEXT("Game.PlayerDied"));
        FGameplayTag DomainTag = FGameplayTag::RequestGameplayTag(TEXT("Domains.Game"));
        
        // You can also add any custom data you want to send as a payload
        FInstancedStruct Payload;  // Optional payload (can be empty)

        EventSubsystem->SendEvent(EventTag, DomainTag, Payload);
    }
}
```

#### Listening for Events

To subscribe to an event in C++:

```cpp
#include "SimpleEventSubsystem.h"
#include "GameplayTagContainer.h"

void YourFunctionToListenForEvent(UObject* Listener, UWorld* World)
{
    if (USimpleEventSubsystem* EventSubsystem = World->GetGameInstance()->GetSubsystem<USimpleEventSubsystem>())
    {
        FGameplayTagContainer EventTags;
        EventTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Game.PlayerDied")));

        FGameplayTagContainer DomainTags;
        DomainTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Domains.Game")));

        EventSubsystem->ListenForEvent(Listener, EventTags, DomainTags, FSimpleEventDelegate::CreateUObject(Listener, &YourClass::YourCallbackFunction));
    }
}

void YourClass::YourCallbackFunction(FGameplayTag EventTag, FGameplayTag Domain, FInstancedStruct Payload)
{
    // Handle the event
    UE_LOG(LogTemp, Log, TEXT("Event received: %s in domain: %s"), *EventTag.ToString(), *Domain.ToString());
}
```

#### Unsubscribing from Events

To stop listening to specific events:

```cpp
#include "SimpleEventSubsystem.h"

void YourFunctionToUnsubscribe(UObject* Listener, UWorld* World)
{
    if (USimpleEventSubsystem* EventSubsystem = World->GetGameInstance()->GetSubsystem<USimpleEventSubsystem>())
    {
        // Unsubscribe a specific delegate
        EventSubsystem->StopListeningForEventDelegate(Listener, FSimpleEventDelegate::CreateUObject(Listener, &YourClass::YourCallbackFunction));
        
        // Unsubscribe filtered by tags
        FGameplayTagContainer EventTags;
        EventTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Game.PlayerDied")));

        FGameplayTagContainer DomainTags;
        DomainTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Domains.Game")));

        EventSubsystem->StopListeningForEventsFiltered(Listener, EventTags, DomainTags);
    }
}
```

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

---

## Contributions

Feel free to submit a pull request or file an issue on GitHub. Contributions are always welcome!

---

Let me know if you need additional features or help!

