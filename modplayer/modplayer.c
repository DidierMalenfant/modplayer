/*
 *  modplayer - Amiga SoundTracker/ProTracker Module Player for Playdate.
 *
 *  Based on littlemodplayer by Matt Evans.
 *
 *  MIT License
 *  Copyright (c) 2022 Didier Malenfant.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

#include "modplayer.h"
#include "platform.h"

#include "lmp/littlemodplayer.h"

#include <stdbool.h> 

// -- Module class

#define CLASSNAME_MODULE                "modplayer.module"
#define NEW_CHUNK_SIZE_IN_SAMPLES       4096
#define MODULE_BUFFER_SIZE_IN_SAMPLES   NEW_CHUNK_SIZE_IN_SAMPLES*16

typedef struct {
    uint8_t* module_data;
    
    int16_t sample_buffer[MODULE_BUFFER_SIZE_IN_SAMPLES * 2];
    int current_offset;
    int current_end_of_frame;
    
    AudioSample* sample;

    mps_t lmp_state;
} Module;

static Module* getModuleArg(int index)
{
    return pd->lua->getArgObject(index, CLASSNAME_MODULE, NULL);
}

int moduleNew(lua_State* state)
{
    const char* filepath = pd->lua->getArgString(1);
    PD_LOG("Loading file at '%s'.", filepath);
    
    SDFile* file = pd->file->open(filepath, kFileRead);
    if (file == NULL) {
        PD_LOG("Error loading file at '%s' (%s).", filepath, pd->file->geterr());
        return 0;
    }
      
    if (pd->file->seek(file, 0, SEEK_END) != 0) {
        PD_LOG("Error seeking to the end of file at '%s' (%s).", filepath, pd->file->geterr());
        pd->file->close(file);
        return 0;
    }
    
    int total_size = pd->file->tell(file);
    if (total_size == 0) {
        PD_LOG("Error getting file size '%s' (%s).", filepath, pd->file->geterr());
        pd->file->close(file);
        return 0;
    }
    
    uint8_t* mod_data = PD_ALLOC(total_size);
    if (mod_data == NULL) {
        PD_LOG("Error allocating memory.");
        pd->file->close(file);
        return 0;
    }
    
    if (pd->file->seek(file, 0, SEEK_SET) != 0) {
        PD_LOG("Error seeking to the beginning of file at '%s' (%s).", filepath, pd->file->geterr());
        pd->file->close(file);
        PD_FREE(mod_data);
        return 0;
    }
    
    int result = pd->file->read(file, mod_data, total_size);
    if (result != total_size) {
        PD_LOG("Error reading file at '%s' %d (%s).", filepath, result, pd->file->geterr());
        pd->file->close(file);
        PD_FREE(mod_data);
        return 0;
    }
    
    if(pd->file->close(file) != 0) {
        PD_LOG("Error closing module file at '%s' (%s).", filepath, pd->file->geterr());
        PD_FREE(mod_data);
        return 0;
    }
    
    Module* module = pd_calloc(1, sizeof(Module));
    if (module == NULL) {
        PD_LOG("Error allocating memory.");
        PD_FREE(mod_data);
        return 0;
    }
    
    module->module_data = mod_data;    
    module->current_offset = MODULE_BUFFER_SIZE_IN_SAMPLES;
    module->current_end_of_frame = 2 * MODULE_BUFFER_SIZE_IN_SAMPLES;

    PD_DBG_LOG("initial chunk at %d-%d", module->current_offset, module->current_end_of_frame);    
    
    memset(module->sample_buffer, 0 , sizeof(module->sample_buffer));        
    
    lmp_init(&module->lmp_state, mod_data);
    lmp_fill_buffer_stereo_soft(&module->lmp_state, module->sample_buffer, MODULE_BUFFER_SIZE_IN_SAMPLES);

    module->sample = pd->sound->sample->newSampleFromData((uint8_t *)module->sample_buffer,
                                                          kSound16bitStereo,
                                                          44100,
                                                          MODULE_BUFFER_SIZE_IN_SAMPLES * sizeof(int16_t) * 2);
    if (module->sample == NULL) {
        PD_LOG("Error creating sample.");
        PD_FREE(module);
        PD_FREE(mod_data);
        return 0;
    }

    pd->lua->pushObject(module, CLASSNAME_MODULE, 0);
    
    return 1;
}

int moduleDelete(lua_State* state)
{
    Module* module = getModuleArg(1);
    if(module == NULL) {
        return -1;
    }

    if(module->sample != NULL) {
        pd->sound->sample->freeSample(module->sample);
    }
    
    if(module->module_data != NULL) {
        PD_FREE(module->module_data);
    }
    
    memset(module, 0, sizeof(Module));
    
    PD_FREE(module);
    
    return 0;
}

static const lua_reg module_class[] = {
    { "new", moduleNew },
    { "__gc", moduleDelete },
    { NULL, NULL }
};

// -- Player class

#define CLASSNAME_PLAYER "modplayer.player"

typedef struct {
    SamplePlayer* sample_player;
    Module* module;
    bool is_playing;
} Player;

static Player* getPlayerArg(int index)
{
    return pd->lua->getArgObject(index, CLASSNAME_PLAYER, NULL);
}

int playerNew(lua_State* state)
{
    Player* player = pd_calloc(1, sizeof(Player));        
    if(player == NULL) {
        return -1;
    }

    pd->lua->pushObject(player, CLASSNAME_PLAYER, 0);

    return 1;
}

int playerDelete(lua_State* state)
{
    Player* player = getPlayerArg(1);
    if(player == NULL) {
        return -1;
    }
    
    if(player->sample_player != NULL) {
        pd->sound->sampleplayer->stop(player->sample_player);
        pd->sound->sampleplayer->freePlayer(player->sample_player);
    }
    
    memset(player , 0, sizeof(Player));
    
    PD_FREE(player);
    
    return 0;
}

int playerLoad(lua_State* state)
{
    Player* player = getPlayerArg(1);
    if(player == NULL) {
        return -1;
    }

    Module* module = getModuleArg(2);
    if(module == NULL) {
        return -1;
    }
    
    if (module->sample == NULL) {
        return -1;
    }
    
    player->module = module;
    
    if (player->sample_player == NULL) {
        player->sample_player = pd->sound->sampleplayer->newPlayer();
        if (!player->sample_player) {
            PD_LOG("Error creating player!");
            return -1;
        }
    }
    else {
        pd->sound->sampleplayer->stop(player->sample_player);
    }
    
    pd->sound->sampleplayer->setSample(player->sample_player, player->module->sample);

    return 1;
}

int playerPlay(lua_State* state)
{
    Player* player = getPlayerArg(1);
    if(player == NULL) {
        return -1;
    }
    
    int result = pd->sound->sampleplayer->play(player->sample_player, 0, 1.0);
    if (!result) {
        PD_LOG("Error playing sample! %d.", result);
        return -1;
    }
    
    player->is_playing = true;
    
    return 0;
}

int playerStop(lua_State* state)
{   
    Player* player = getPlayerArg(1);
    if(player == NULL) {
        return -1;
    }
    
    if(player->sample_player == NULL) {
        return -1;
    }

    pd->sound->sampleplayer->stop(player->sample_player);
    
    player->is_playing = false;
    
    return 0;
}

int playerUpdate(lua_State* state)
{    
    Player* player = getPlayerArg(1);
    if(player == NULL) {
        return -1;
    }

    if (player->module == NULL) {
        return -1;
    }

    if (!player->is_playing) {
        return 0;
    }
    
    pd->system->resetElapsedTime();
    
    if (player->module->current_offset >= player->module->current_end_of_frame) {
        float half_sample_length = pd->sound->sampleplayer->getLength(player->sample_player) / 2.0f;
        float current_playing_position = pd->sound->sampleplayer->getOffset(player->sample_player);
    
        PD_DBG_LOG("waiting %f %f", current_playing_position, half_sample_length);        

        if (player->module->current_end_of_frame == 2 * MODULE_BUFFER_SIZE_IN_SAMPLES) {
            if (current_playing_position <= half_sample_length) {
                PD_DBG_LOG("Took %.2f%% of a frame.", 100 * pd->system->getElapsedTime() / (1.0f / 30));
                return 0;
            }
            
            player->module->current_offset = 0;
            player->module->current_end_of_frame = MODULE_BUFFER_SIZE_IN_SAMPLES;
        }
        else {
            if (current_playing_position >= half_sample_length) {
                PD_DBG_LOG("Took %.2f%% of a frame.", 100 * pd->system->getElapsedTime() / (1.0f / 30));
                return 0;
            }
            
            player->module->current_offset = MODULE_BUFFER_SIZE_IN_SAMPLES;
            player->module->current_end_of_frame = 2 * MODULE_BUFFER_SIZE_IN_SAMPLES;
        }

        PD_DBG_LOG("new chunk at %d-%d", player->module->current_offset, player->module->current_end_of_frame);        
    }
    
    PD_DBG_LOG("chunk at %d", player->module->current_offset);

    int16_t* current_position = player->module->sample_buffer + player->module->current_offset;
    lmp_fill_buffer_stereo_soft(&player->module->lmp_state, current_position, NEW_CHUNK_SIZE_IN_SAMPLES);
    
    player->module->current_offset += NEW_CHUNK_SIZE_IN_SAMPLES;

    PD_DBG_LOG("Took %.2f%% of a frame.", 100 * pd->system->getElapsedTime() / (1.0f / 30));

    return 0;
}

static const lua_reg player_class[] = {
    { "new", playerNew },
    { "__gc", playerDelete },
    { "load", playerLoad },
    { "play", playerPlay },
    { "stop", playerStop },
    { "update", playerUpdate },
    { NULL, NULL }
};

// -- Register our classes with Lua

void registerModPlayer(void)
{
    const char* err = NULL;

    // -- Register Module class
    if(!pd->lua->registerClass(CLASSNAME_MODULE, module_class, NULL, 0, &err)) {
        PD_LOG("modplayer: Failed to register module class. (%s)", err);
        return;
    }

    // -- Register Player class
    if(!pd->lua->registerClass(CLASSNAME_PLAYER, player_class, NULL, 0, &err)) {
        PD_LOG("modplayer: Failed to register module class. (%s)", err);
        return;
    }
}
