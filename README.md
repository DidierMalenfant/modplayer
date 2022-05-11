# modplayer for Playdate

An Amiga Soundtracker/Protracker player, based off [lmp](https://github.com/evansm7/lmp), for the [Playdate SDK](https://play.date/dev/).

v0.2-alpha May 11th 2022


## Usage

You will need to add C extensions to your Playdate project. You can take a look at the Makefile provided in the sample code for inspiration if your project doesn't already use a mixture of C and Lua. The file `$(SDK_DIR)/C_API/buildsupport/common.mk` pretty much does all the heavy lifting there.

You then need to add `modplayer/modplayer.c`, `modplayer/platform.c` and `modplayer/lmp/littlemodplayer.c` to your list of source Files (`SRC` variable if you're using `common.mk`). If you already have C extensions in your project, you will need to add a call to `registerModPlayer()` during Lua's initialisation (responding to a `kEventInitLua` event for example). Finally you'll also need to add the `modplayer` folder to your VPATH and include dir paths.

If you don't already have C extensions, you will also need to add `extension/main.c` to your source files and the `extension` folder to your VPATH and include dir paths.

modplayer should then be available to you in your Lua project.

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
