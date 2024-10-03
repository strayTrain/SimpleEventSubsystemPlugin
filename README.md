# Simple Event Plugin

**Simple Event Plugin** is a lightweight Unreal Engine plugin that provides a subsystem that can be used to send and listen for events.
- Events are indentified by `EventTags` and categorized by `DomainTags`.
- Events come with a payload that can be any UStruct you create in C++ or Blueprints (via [InstancedStruct](https://www.reddit.com/r/unrealengine/comments/1f7o1co/what_is_the_difference_between_a_struct_and_an/)).
- Anything that has access to the GameInstance (e.g Widgets, Pawns, PlayerControllers) can listen/send events.
- Sending events is not replicated but you can get around this using RPC's.

## API Summary

<details>
  <summary>Sending an event</summary>

  You can use any struct as a payload with the `MakeInstancedStruct` node.
    
  ![image](https://github.com/user-attachments/assets/10307cca-f18b-47cc-9b99-f55f111e488a)
</details>

<details>
  <summary>Listening for an event</summary>

  You can listen for multiple events and domains at the same time. 
  
  ![image](https://github.com/user-attachments/assets/a1e6ddf3-4448-45e1-b215-14bf9a1be7f5)
</details>

<details>
  <summary>Receiving an event</summary>

  Use the `GetInstancedStructValue` node to cast to the type you expect (the output is initially a wildcard and you break your expected struct to set the output type)   
  ![image](https://github.com/user-attachments/assets/82c3bbdb-f08a-4939-9d06-6fcffed21657)
</details>

<details>
  <summary>Stop listening for an event</summary>
    
  ![image](https://github.com/user-attachments/assets/34f561ef-443b-4b6a-8899-bcd2f422903f)
</details>

<details>
  <summary>A note on replication </summary>

  The `SendEvent` function is not replicated i.e calling `SendEven`t on the client won't trigger a listener on the server and vice versa.
  `InstancedStruct` can be replicated though so can use it as an argument for an RPC. E.g The server calls a multicast event which calls send event on all connected clients
  
  ![image](https://github.com/user-attachments/assets/1d04328e-a038-4ea2-8035-c4115eda914e)
</details>

## Requirements

- Unreal Engine 5.2* or higher.   
*instanced structs were introduced in 5.0 with the StructUtils plugin and got [replication support around 5.2](https://github.com/EpicGames/UnrealEngine/pull/9280). Thus, this plugin will technically work with < 5.2 but expect things might not work

## Installation Steps

1. Download or clone the plugin into your Unreal Engine project, under the [Plugins folder](https://dev.epicgames.com/documentation/en-us/unreal-engine/plugins-in-unreal-engine#pluginfolders) (create it if it doesn't exist.)
2. Rebuild your project.
3. Enable the plugin in your Unreal Engine project by navigating to **Edit > Plugins** and searching for "SimpleEventPlugin". (it should be enabled by default)


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
