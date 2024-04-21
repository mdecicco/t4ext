## t4ext
This project builds a file, `t4ext.dll`, which embeds the [v8 engine](https://github.com/v8/v8) into the game and exposes several APIs to it for modding purposes.

I probably won't be putting too much work into documenting all of this, but as of right now you can check out
[this file](https://github.com/mdecicco/t4scripts/blob/main/mods/DebugMenu.ts) to see an example of what's possible
at the moment.

## Contents
- [Requirements](#requirements)
- [Features](#features)
- [Status](#status)
- [The Script API](#the-script-api)
    - [Namespaces](#namespaces)
    - [Global Mods](#global-mods)
    - [Actor Mods](#actor-mods)
    - [Actor Type Mods](#actor-type-mods)
    - [Events](#events)

## Requirements
In order to use `t4ext.dll` you will need to have a modified version of `Turok4.exe` which has the IAT modified to replace the import of `Direct3DCreate9` from `d3d9.dll` with an import of the same function, but from `t4ext.dll`. There's some name mangling going on with `t4ext.dll`, so the exported function from that DLL is actually called `_Direct3DCreate9Hook@4`.

In addition to this, you will also need to clone the [t4scripts repo](https://github.com/mdecicco/t4scripts) to `<game directory>/scripts`.

If you downloaded a zip file with all this stuff in it and extracted it to your game folder, you can ignore this


## Features
- Automatically loads mod scripts files from `<game directory>/scripts/mods` on startup, so long as they're written in TypeScript and have the `.ts` extension
- Automatically reloads mod scripts if they are modified during gameplay
- Mods have access to a caching mechanism so that they can have data persisted between game sessions / reloads
- Automatically scans the `<game directory>/inject` directory for `.dll` files to load into the game at runtime
- Exposes game engine functionality to scripts for the purpose of manipulating the behavior of the game
- Exposes the [ImGui](https://github.com/ocornut/imgui) API to scripts for writing scripted UI elements
- Exposes additional APIs for the purpose of facilitating more interesting mod development

## Status
Good for testing and tinkering. Maybe not so much for writing production-grade mods yet.

There are still many game engine features to expose to the scripts, and many things will need to be understood better in order to decrease the likelihood of causing the game to crash. As it stands, when the game loads with this modding framework it is more or less stable but occasionally it may crash either because it's using the game engine in ways it wasn't meant to be used or because there is still work to do on the interface between the scripts and the game.

## The script API
The first thing to know about writing mod scripts for the game is that t4ext has a few different options as far as how you structure your mod. Right now there's really only two "types" of mods, but in the future there will be more.

These types are:
- "Global" mods
    - This type is useful if you want easy access to changing the
    behavior of the game in general, or rendering some UI that is available at all times
- "Plain" mods
    - Plain mod scripts don't have to follow any structure and can be set up however you want, but currently
    will not have access to caching, and will not be automatically reloaded because they will not be tracked by the mod manager. This type makes sense if you want to run a simple script on startup or something like that

Planned types:
- "Actor" mods
    - Mods of this type will be tied to individual "Actor"s in the game. `Actor` is the term the game uses to refer
    to pretty much every object in the game world, including UI elements, foliage, static and dynamic models, etc...
    - This type will be useful for writing scripts to control specific actors in the game. For example, you could
    write an actor mod for a specific bush in a specific level so that it always tries to follow the player.
- "Actor Type" mods
    - Mods of this type will be tied to actor _types_. There are several different types of actors in the game.
    Including but not limited to: weapons, pickups, foliage, ui elements, ...
    - You could use this mod type to affect the behavior of all actors of a specific type. For example, you could
    write a actor type mod so that ALL bushes in the game will try to follow the player
- This might be all that's necessary but I'm not sure

For any of the specific game types, like `CGame`, `CLevel`, `CActor`, you can use VS Code's autocompletions or view
the auto-generated `globals.d.ts` file for more info about specific fields you have access to for each game type. Many things are unknown at this point, so if you notice any interesting behavior with anything you change, let me know. In general, use VS Code opened to the `<game directory>/scripts` path for best experiences with writing mods.
VS Code should utilize the `tsconfig.json`, `globals.d.ts`, `default.d.ts` files to provide useful autocompletions
and in some cases documentation about the API.

### Namespaces
Most of the script API you'll have available to you when writing mods will be available from a few different namespaces:
- `t4` - This is where all of the turok-specific types, functions, interfaces will be
- `fs` - This is where anything related to file IO will be
- `sys` - Anything low level will be here. Currently the only thing here is `sys.loadDLL` though
- `ImGui` - This is where all things `ImGui` will be

If you're new to typescript, and using VS Code (opened to `<game directory>/scripts`), you can type `t4.` for example in order to see what things are available to you.

### Global Mods
Global mods have to be in the form of a TypeScript `class`, and your class has to specify that it implements the `GlobalMod` interface. Additionally, in order for your mod to activate you need to call `ModMan.registerGlobalMod`
with your mod's class as the only parameter.

```
class YourMod implements GlobalMod {
    // your interesting stuff
};

ModMan.registerGlobalMod(YourMod);
```

There are a few methods your mod class can inherit from the `GlobalMod` interface:
- `onInitialize()`
- `onShutdown()`
- `onActorCreated(actor: t4.CActor)`
- `onActorDestroy(actor: t4.CActor)`
- `onLevelCreate(level: t4.CLevel)`
- `onLevelDestroy(level: t4.CLevel)`
- `onKeyboardInput(event: t4.KeyboardEvent)`
- `onUpdate(deltaTime: f32)`
- `onRender(deltaTime: f32)`

All of these methods will be called automatically by the mod manager, you should not call them directly from your own code

#### onInitialize()
This method will be called when your mod is loaded into the game, and whenever it's reloaded after being modified.
You should use this method to set up any event listeners you might care about or acquire any data from the game
engine that's relevant to your mod

#### onShutdown()
This method will be called when your mod is unloaded from the game, and before it's reloaded after being modified.
You should use this method to remove any event listeners your mod set up in `onInitialize`. Basically undo anything
you did in `onInitialize` that needs to be undone so that the API doesn't continue to try to talk to your mod.

#### onActorCreated(actor: t4.CActor)
This method will be called whenever the game spawns a new actor into the level. `actor` is the actor that was spawned.

#### onActorDestroy(actor: t4.CActor)
This method will be called whenever the game destroys an actor. `actor` is the actor that is about to be destroyed.

#### onLevelCreate(level: t4.CLevel)
This method will be called whenever the game loads a new level. `level` is the level that was created. Note: This method may be called before the level is actually loaded.

#### onLevelDestroy(level: t4.CLevel)
This method will be called whenever the game destroys an level. `level` is the level that is about to be destroyed.

#### onKeyboardInput(event: t4.KeyboardEvent)
This method will be called whenever there's keyboard input. The keyboard event `event` has a few different properties you can access.
```
enum KeyState {
    Pressed = 0, // Key was just pressed, this will only be called once per key press
    Held = 1,    // Key is currently being held down, this will be called once per frame as long as the key is down
    Released = 2 // Key was released, this will only be called once per key press
};

class KeyboardEvent {
    readonly key: t4.Key; // see autocompletions for `t4.Key` for more info on options
    readonly state: t4.KeyState;
};
```

#### onUpdate(deltaTime: f32)
This method will be called once per frame, immediately before the game engine updates itself. `deltaTime` is the
amount of time in seconds that has passed since the last frame.

#### onRender(deltaTime: f32)
This method will be called after the game has put in all the work to render everything, but before presenting the
framebuffer to the screen. You can render your UI or anything you care about here. `deltaTime` is the
amount of time in seconds that has passed since the last frame.

### Actor Mods
todo

### Actor Type Mods
todo

### Events
t4ext uses an event driven architecture for scripts. The v8 engine is constantly running in parallel with the game. The game has been modified to notify t4ext when certain key things occur, and t4ext will in turn notify the script engine of these events. Unless I've made a grave mistake, any code you write should be executed while the game is in a paused state, waiting on the scripts to process events. This is necessary because if you change something with the game engine while it's actively running it will very likely cause a crash. It is set up such that the script engine will try to limit the amount of time it spends processing events in an effort to not disrupt the game's normal function. So be careful about how much work your scripts are doing, in particular with any `onUpdate` and `onRender` methods.

All `add*` event listener creators will return a number which is used to identify the listener. You should pass
this number to the corresponding `remove*` function in order to destroy the listener.

#### Engine Creation
- `t4.addEngineCreateListener(callback: (engine: t4.CGame) => void): u32`
    - This function will add the specified `callback` as a listener for when the engine is created
    - When the game engine is created, the callback will be called and the engine object will be passed to it
    - If add a listener of this type _after_ the engine is created, it will never be called.
- `t4.removeEngineCreateListener(listenerId: u32)`
    - Removes an engine creation listener

#### Engine Updates
- `engine.addUpdateListener(callback: () => void): u32`
    - This function will add the specified `callback` as a listener for when the game engine updates, once per frame.
    - When the engine updates, the callback will be called with no parameters
- `engine.removeUpdateListener(listenerId: u32)`
    - Removes an engine update listener

#### Engine Renders
- `engine.addRenderListener(callback: () => void): u32`
    - This function will add the specified `callback` as a listener for when the game engine renders, once per frame.
    - When the engine renders, the callback will be called with no parameters
- `engine.removeRenderListener(listenerId: u32)`
    - Removes an engine render listener

#### Actor Creation
- `engine.addActorCreateListener(callback: (actor: t4.CActor) => void): u32`
    - This function will add the specified `callback` as a listener for when the game spawns an actor into the
    current level.
    - `actor` is the actor that was spawned
- `engine.removeActorCreateListener(listenerId: u32)`
    - Removes an actor creation listener

#### Actor Destruction
- `engine.addActorDestroyListener(callback: (actor: t4.CActor) => void): u32`
    - This function will add the specified `callback` as a listener for when the game destroys an actor.
    - `actor` is the actor that is about to be destroyed
- `engine.removeActorDestroyListener(listenerId: u32)`
    - Removes an actor destruction listener

#### Level Creation
- `engine.addLevelCreateListener(callback: (level: t4.CLevel) => void): u32`
    - This function will add the specified `callback` as a listener for when the game creates a new level.
    - `callback` may be executed before the level actually loads
    - `level` is the level that was created
- `engine.removeLevelCreateListener(listenerId: u32)`
    - Removes a level creation listener

#### Level Destruction
- `engine.addLevelDestroyListener(callback: (level: t4.CLevel) => void): u32`
    - This function will add the specified `callback` as a listener for when the game destroys a level.
    - `level` is the level that is about to be destroyed
- `engine.removeLevelDestroyListener(listenerId: u32)`
    - Removes an level destruction listener

#### Timeouts & Intervals
- `setTimeout(callback: () => void, delayInMS: number): TimeoutID`
    - Sets up a timer to call `callback` after `delayInMS` milliseconds
    - This function returns a `TimeoutID` which can be used to remove the timeout before it's completed
- `clearTimeout(timeout: TimeoutID)`
    - Removes a timeout created by `setTimeout` if it exists and hasn't already fired
    - If you remove a timeout before the callback function is called, the callback function will not be called
- `setInterval(callback: () => void, intervalInMS: number): IntervalID`
    - Sets up a timer to call `callback` every `intervalInMS` milliseconds
    - This function returns an `IntervalID` which can be used to remove the interval, which will stop it from
    calling `callback`
- `clearInterval(interval: IntervalID)`
    - Removes an interval created by `setInterval` if it exists
    - If you remove an interval, the callback function will stop being called