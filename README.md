# Simple Event Plugin

**Simple Event Plugin** is a lightweight Unreal Engine plugin that provides a subsystem that can be used to send and listen for events.
- Events are indentified by `EventTags` and categorized by `DomainTags`.
- Events come with a payload that can be any UStruct you create in C++ or Blueprints (via [InstancedStruct](https://www.reddit.com/r/unrealengine/comments/1f7o1co/what_is_the_difference_between_a_struct_and_an/)).
- Anything that has access to the GameInstance (e.g Widgets, Pawns, PlayerControllers) can listen/send events.
- Sending events is not replicated but you can get around this using RPC's.

## API Summary

<details>
  <summary>Sending an event</summary>
    
  ![image](https://github.com/user-attachments/assets/064b7841-382d-4766-b058-0f3cc6c28abd)
</details>

<details>
  <summary>Listening for an event</summary>
    
  ![image](https://github.com/user-attachments/assets/956ba3e7-a2d3-4328-b6c1-faa8b8356d39)
</details>

<details>
  <summary>Receiving an event</summary>
    
  ![image](https://github.com/user-attachments/assets/d7c2fdd3-dd43-4d4c-aef1-417ded08cbc7)
</details>

<details>
  <summary>Stop listening for an event</summary>
    
  ![image](https://github.com/user-attachments/assets/055bb44b-28bc-4c42-a832-b8c6af02dbfd)
</details>

<details>
  <summary>A note on replication </summary>
    
  ![image](https://github.com/user-attachments/assets/1bce23c9-a01b-4b41-bb2f-71791f0c1c8d)
</details>

## Requirements

- Unreal Engine 5.2* or higher.   
*instanced structs were introduced in 5.0 with the StructUtils plugin and got [replication support around 5.2](https://github.com/EpicGames/UnrealEngine/pull/9280). Thus, this plugin will technically work with < 5.2 but expect things might not work

## Installation Steps

1. Download or clone the plugin into your Unreal Engine project, under the [Plugins folder](https://dev.epicgames.com/documentation/en-us/unreal-engine/plugins-in-unreal-engine#pluginfolders) (create it if it doesn't exist.)
2. Enable the plugin in your Unreal Engine project by navigating to **Edit > Plugins** and searching for "SimpleEventPlugin".
3. Rebuild your project.


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
        FInstancedStruct Payload = FInstancedStruct::Make(FVector::UpVector);

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
    // Test if an instanced struct is a vector
    if (const FVector* TestVector = Payload.GetPtr<FVector>())
    {
       // Do something with the payload vector
    }

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

