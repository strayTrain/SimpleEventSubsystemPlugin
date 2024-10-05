# Simple Event Plugin

**Simple Event Plugin** is a lightweight Unreal Engine plugin that provides a subsystem to send and listen for events. </br>
Events can come with a payload that can be any UStruct you create in C++ or Blueprints (via [InstancedStruct](https://github.com/mattyman174/GenericItemization?tab=readme-ov-file#intro)). </br>
Anything that has access to the GameInstance (e.g Widgets, Pawns, PlayerControllers) can listen for/send events.

## API Summary

<details>
  <summary>Sending an event</summary>

  </br>
  
  Events are identified using two [gameplay tags](https://www.tomlooman.com/unreal-engine-gameplaytags-data-driven-design/): </br>
  - `EventTag` e.g `Events.UI.ButtonClicked`
  - `DomainTag` e.g `EventDomains.UI.PauseMenu`

  </br>

  <details>
  <summary>Why use two tags instead of one?</summary>
  
  </br>
    
  Using a domain tag can help cut down on repetetive tag names. </br>
  
  For example, imagine we have a manager object that wants to know when any UI button gets clicked. </br>
  
  Using a `DomainTag` we can use the following to identify the events of interest </br>
  EventTag = `UI.ButtonClicked` </br>
  DomainTags = `[EventDomains.MainMenu, EventDomains.OptionsMenu, EventDomains.PauseMenu]` </br>
  
  Without a `DomainTag` we would need to listen for: </br>
  EventTags = `[UI.MainMenu.ButtonClicked, UI.OptionsMenu.ButtonClicked, UI.PauseMenu.ButtonClicked]` </br>
  
  Without a `DomainTag` we have 3 "ButtonClicked" tags and this looks messy in my opinion as we start to add more tags during development. </br>
  </details>
  
  </br>
  
  You can use any struct as a payload to provide extra context to the event with the `MakeInstancedStruct` node.

  </br>
    
  ![image](https://github.com/user-attachments/assets/10307cca-f18b-47cc-9b99-f55f111e488a)
</details>

<details>
  <summary>Listening for an event</summary>

  </br>

  You can listen for multiple events and domains at the same time. </br>
  If you leave `EventFilter` or `DomainFilter` empty then the delegate will be triggered for all events/domains.
  
  There are some additional arguments that are hidden by default in the `ListenForEvent` node: </br>
  - `RequiredPayloadType`: The event won't trigger unless a payload exists and it is of this specific struct type. If left empty the event will accept any payload. Default behaviour is empty.
  - `OnlyMatchExactEvent` & `OnlyMatchExactDomain`: if set to true, "A.B" will only match "A.B" and won't match "A.B.C" tags. By default they are set to only match tags exactly.
    
  ![image](https://github.com/user-attachments/assets/a1eb6709-05f9-457b-a056-68417795f040)
</details>

<details>
  <summary>Receiving an event</summary>

  </br>

  Use the `GetInstancedStructValue` node to cast to the type you expect. </br>
  The output is initially a wildcard and you break your expected struct to cast the output type. 

  </br>
  
  ![image](https://github.com/user-attachments/assets/82c3bbdb-f08a-4939-9d06-6fcffed21657)
</details>

<details>
  <summary>Stop listening for an event</summary>

  </br>
    
  ![image](https://github.com/user-attachments/assets/34f561ef-443b-4b6a-8899-bcd2f422903f)
</details>

<details>
  <summary>A note on replication </summary>

  </br>

  The `SendEvent` function is not replicated i.e calling `SendEvent` on the client won't trigger a listener on the server and vice versa.
  `InstancedStruct`, which is the type of `Payload` in `SendEvent`, **can** be replicated though! So you can pass the payload though an RPC and the underlying wrapped struct will also replicate. e.g The server calls a multicast event which calls `SendEvent` on all connected clients

  </br>
  
  ![image](https://github.com/user-attachments/assets/1d04328e-a038-4ea2-8035-c4115eda914e)
</details>

## Requirements

- Unreal Engine 5.2* or higher.   
*instanced structs were introduced in 5.0 with the StructUtils plugin and got [replication support around 5.2](https://github.com/EpicGames/UnrealEngine/pull/9280). Thus, this plugin may work with UE < 5.2 but expect issues with replication.

## Installation Steps

1. Download or clone the SimpleEventPlugin folder from this repo into your Unreal Engine project under your project's [Plugins folder](https://dev.epicgames.com/documentation/en-us/unreal-engine/plugins-in-unreal-engine#pluginfolders), create the Plugins folder if it doesn't exist. (e.g. If your project folder is `C:\Projects\SimpleEventTest` then place SimpleEventPlugin in  `C:\Projects\SimpleEventTest\Plugins`)
2. Rebuild your project.
3. Enable the plugin in your Unreal Engine project by navigating to **Edit > Plugins** and searching for "SimpleEventPlugin". (it should be enabled by default)


---

### C++ Examples

#### Sending Events

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
