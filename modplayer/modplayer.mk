#
#  modplayer - Amiga SoundTracker/ProTracker Module Player for Playdate.
#
#  Based on littlemodplayer by Matt Evans.
#
#  MIT License
#  Copyright (c) 2022 Didier Malenfant.
#
#  Permission is hereby granted, free of charge, to any person obtaining a copy
#  of this software and associated documentation files (the "Software"), to deal
#  in the Software without restriction, including without limitation the rights
#  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#  copies of the Software, and to permit persons to whom the Software is
#  furnished to do so, subject to the following conditions:
#
#  The above copyright notice and this permission notice shall be included in all
#  copies or substantial portions of the Software.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#  SOFTWARE.
#

ifndef NET_MALENFANT_MODPLAYER_PD	
	# -- We need to make sure this library won't be included twice in a project
	# -- This could happen if the project uses tow libraries which use this one
	NET_MALENFANT_MODPLAYER_PD := 1

	# -- Find out more about where this file is relative to the Makefile including it
	RELATIVE_FILE_PATH := $(lastword $(MAKEFILE_LIST))
	RELATIVE_DIR := $(subst /$(notdir $(RELATIVE_FILE_PATH)),,$(RELATIVE_FILE_PATH))
	RELATIVE_PARENT_DIR := $(subst /$(notdir $(RELATIVE_DIR)),,$(RELATIVE_DIR))
	
	# -- Add us as an include search folder only if it's not already there
	uniq = $(if $1,$(firstword $1) $(call uniq,$(filter-out $(firstword $1),$1)))
	UINCDIR := $(call uniq, $(UINCDIR) $(RELATIVE_PARENT_DIR))
	
	# -- Add our source files
	SRC := $(SRC) \
	   	   $(RELATIVE_DIR)/modplayer.c \
	   	   $(RELATIVE_DIR)/lmp/littlemodplayer.c
	
	# -- Add the utility library
	include $(RELATIVE_DIR)/pdutility/pdutility.mk
endif
