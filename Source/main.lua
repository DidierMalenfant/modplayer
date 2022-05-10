--
--  modplayer - Amiga SoundTracker/ProTracker Module Player for Playdate.
--
--  Based on littlemodplayer by Matt Evans.
--
--  MIT License
--  Copyright (c) 2022 Didier Malenfant.
--
--  Permission is hereby granted, free of charge, to any person obtaining a copy
--  of this software and associated documentation files (the "Software"), to deal
--  in the Software without restriction, including without limitation the rights
--  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
--  copies of the Software, and to permit persons to whom the Software is
--  furnished to do so, subject to the following conditions:
--
--  The above copyright notice and this permission notice shall be included in all
--  copies or substantial portions of the Software.
--
--  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
--  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
--  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
--  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
--  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
--  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
--  SOFTWARE.
--

import "CoreLibs/utilities/where"
import "CoreLibs/timer"

local gfx = playdate.graphics

local setup_is_done = false
local module = nil
local player = nil

function setup()
    print('Setting up...')
    gfx.setColor(gfx.kColorBlack)

    module = modplayer.module.new('Sounds/Crystal_Hammer.mod')
    assert(module)

    player = modplayer.player.new()
    assert(player)

    player:load(module)
    player:play()

    setup_is_done = true
end

function playdate.update()
    if not setup_is_done then
        setup()
    end

    gfx.fillRect(0, 0, 400, 240)
    playdate.drawFPS(0,0)

    player:update()
end
