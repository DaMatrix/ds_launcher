.SUFFIXES:
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

export TARGET	:=	$(shell basename $(CURDIR))
export TOPDIR	:=	$(CURDIR)

# specify a directory which contains the nitro filesystem
# this is relative to the Makefile
NITRO_FILES	:= nitrofs

# These set the information text in the nds file
GAME_TITLE     := PorkStore
GAME_SUBTITLE1 := made by DaPorkchop_
GAME_SUBTITLE2 := https://daporkchop.net

include $(DEVKITARM)/ds_rules

.PHONY: checkarm7 checkarm9 clean

# main targets
all: $(TARGET).nds

checkarm7:
	$(MAKE) -C arm7

checkarm9:
	$(MAKE) -C arm9

$(TARGET).nds	: $(NITRO_FILES) checkarm7 checkarm9
	ndstool	-c $(TARGET).nds -7 arm7/arm7.elf -9 arm9/arm9.elf \
	-b $(GAME_ICON) "$(GAME_TITLE);$(GAME_SUBTITLE1);$(GAME_SUBTITLE2)" \
	$(_ADDFILES)

clean:
	$(MAKE) -C arm9 clean
	$(MAKE) -C arm7 clean
	rm -f $(TARGET).nds arm7/arm7.elf arm9/arm9.elf
