# modplayer for Playdate

An Amiga Soundtracker/Protracker player, based off [lmp](https://github.com/evansm7/lmp), for the [Playdate SDK](https://play.date/dev/).

v0.2-alpha May 11th 2022


## Usage

If you're already using C extensions in your project and you're also using the rules in `$(SDK_DIR)/C_API/buildsupport/common.mk` to build it, then adding this library is very easy. Just include `include modplayer/modplayer.mk` in your makefile and then call `registerModPlayer()` (declared in `modplayer/modplayer.h`) during Lua's initialisation (responding to a `kEventInitLua` event for example).

If you currently only have a pure Lua project, you will need to add a `Makefile` to it and some code to add the extensions. The `Makefile` used to build the demo project and the `extension` folder provided are both a very good places to start.

`modplayer` should then be available in your Lua project.

The overall paradigm is:

 * Load a mod file
 * Create a player and add the module to it
 * Press play and update the player every frame

Rough example:

~~~
	module = modplayer.module.new('Sounds/Crystal_Hammer.mod')
	assert(module)

	player = modplayer.player.new()
	assert(player)
	
	player:load(module)
	player:play()
~~~

Then make sure that you call `player:update()` in your `playdate.update()` method.


## Where can I get modules from?

### The internet

[ModArchive](https://modarchive.org/) is a good place to start.  Look for `.MOD` 4-channel SoundTracker/ProTracker files.  This won't play XM etc.

### Write your own!

[MilkyTracker](https://milkytracker.org) is a great multi-platform tracker, complete with the old school tracker look and feel!


## What's the plan moving forward?

This is more or less just a proof of concept at this point. I don't have an actual Playdate console so I have no idea how fast this runs IRL. There are quite a few things that we can improve on or add:

 * There seems to be some glitches during the replay. Maybe at given buffer boundaries.
 * This renders the audio at 44100Hz, it's probably overkill since Soundtracker samples had a much lower sample rate IIRC.
 * There may be a possibility to use Playdate's sound SDK instead of rendering the audio on the fly. Not sure how effects would be supported.
 * Some effects are currently not supported because lmp does not support them (portamento for example).
 * The API could use some documenting, even if it's fairly simple right now.
 

* * *

Copyright (c) 2022 Didier Malenfant
