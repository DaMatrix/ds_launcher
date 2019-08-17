.SUFFIXES:
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

include $(DEVKITARM)/ds_rules

# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
# DATA is a list of directories containing binary files
# all directories are relative to this makefile
BUILD		:=	build
SOURCES		:=	source ../lib/porklib/source
INCLUDES	:=	include ../lib/porklib/source
DATA		:=
 
# options for code generation
ARCH	:=	-mthumb-interwork

CFLAGS	:=	-g -Wall -O2 \
		-mcpu=arm7tdmi -mtune=arm7tdmi -fomit-frame-pointer\
		-ffast-math \
		$(ARCH)

CFLAGS	+=	$(INCLUDE) -DARM7 -DNDS -DDSI
CXXFLAGS	:=	$(CFLAGS) -fno-rtti -fno-rtti


ASFLAGS	:=	-g $(ARCH)
LDFLAGS	=	-specs=ds_arm7.specs -g $(ARCH) -Wl,--nmagic -Wl,-Map,$(notdir $*).map

LIBS	:=	-ldswifi7 -lmm7 -lnds7

# list of directories containing libraries, this must be the top level containing
# include and lib
LIBDIRS	:=	$(LIBNDS)
 
  
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
ifneq ($(BUILD),$(notdir $(CURDIR)))
 
export ARM7ELF	:=	$(CURDIR)/arm7.elf
export DEPSDIR	:=	$(CURDIR)/$(BUILD)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir))
 
CFILES			:=	$(foreach dir,$(SOURCES),$(shell find -L $(dir) -type f -name '*.c'))
CFILES_COMP		:=  $(subst /,_, $(subst ..,_,$(CFILES)))
CPPFILES		:=	$(foreach dir,$(SOURCES),$(shell find -L $(dir) -type f -name '*.cpp'))
CPPFILES_COMP	:=  $(subst /,_, $(subst ..,_,$(CPPFILES)))
SFILES			:=	$(foreach dir,$(SOURCES),$(shell find -L $(dir) -type f -name '*.s'))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))
 
export OFILES	:=	$(addsuffix .o,$(BINFILES)) \
					$(CPPFILES_COMP:.cpp=.cpp.o) $(CFILES_COMP:.c=.c.o) $(SFILES_COMP:.s=.s.o)
 
export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			-I$(CURDIR)/$(BUILD)
 
export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

# use CXX for linking C++ projects, CC for standard C
ifeq ($(strip $(CPPFILES)),)
	export LD	:=	$(CC)
else
	export LD	:=	$(CXX)
endif

.PHONY: $(BUILD) clean
 
$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile
 
clean:
	@echo clean ...
	@rm -fr $(BUILD) *.elf
 
 
else

all: $(ARM7ELF)
 
# main targets
$(ARM7ELF)	:	$(OFILES)
	@echo linking $(notdir $@)
	@$(LD)  $(LDFLAGS) $(OFILES) $(LIBPATHS) $(LIBS) -o $@


# you need a rule like this for each extension you use as binary data
%.bin.o	:	%.bin
	@echo $(notdir $<)
	@$(bin2o)

%.c.o: $(CFILES)
	@echo "$(notdir $<) -> $(notdir $@)"
	$(CC) -MMD -MP -MF $(DEPSDIR)/$*.d $(CFLAGS) -c ../$(subst .c.o,.c,$(subst _,/,$(subst __,../,$@))) -o $@ $(ERROR_FILTER)

%.cpp.o: $(CPPFILES)
	@echo "$(notdir $<) -> $(notdir $@)"
	$(CXX) -MMD -MP -MF $(DEPSDIR)/$*.d $(CXXFLAGS) -c ../$(subst .cpp.o,.cpp,$(subst _,/,$(subst __,../,$@))) -o $@ $(ERROR_FILTER)

%.s.o: $(SFILES)
	@echo "$(notdir $<) -> $(notdir $@)"
	$(CC) -MMD -MP -MF $(DEPSDIR)/$*.d -x assembler-with-cpp $(ASFLAGS) -c ../$(subst .s.o,.s,$(subst _,/,$(subst __,../,$@))) -o $@ $(ERROR_FILTER)

-include $(DEPENDS)
 
#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------
