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
 
#include "platform.h"

// -- Utility functions

void* pd_calloc(size_t nb_of_items, size_t item_size)
{
    if (item_size && (nb_of_items > (SIZE_MAX / item_size))) {
        return NULL;
    }

    size_t size = nb_of_items * item_size;
    void* memory = PD_ALLOC(size);
    if(memory != NULL) {
        memset(memory, 0, size);
    }
    
    return memory;
}
