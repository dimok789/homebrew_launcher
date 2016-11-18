#---------------------------------------------------------------------------------
# Clear the implicit built in rules
#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITPPC)),)
$(error "Please set DEVKITPPC in your environment. export DEVKITPPC=<path to>devkitPPC")
endif
ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>devkitPRO")
endif
ifeq ($(strip $(WUT_ROOT)),)
$(error "Please ensure WUT_ROOT is in your environment.")
endif
export PATH			:=	$(DEVKITPPC)/bin:$(PORTLIBS)/bin:$(PATH)
export LIBOGC_INC	:=	$(DEVKITPRO)/libogc/include
export LIBOGC_LIB	:=	$(DEVKITPRO)/libogc/lib/wii
export PORTLIBS		:=	$(DEVKITPRO)/portlibs/ppc

PREFIX	:=	powerpc-eabi-

export AS	:=	$(PREFIX)as
export CC	:=	$(PREFIX)gcc
export CXX	:=	$(PREFIX)g++
export AR	:=	$(PREFIX)ar
export OBJCOPY	:=	$(PREFIX)objcopy

export ELF2RPL	:= $(WUT_ROOT)/bin/elf2rpl

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
#---------------------------------------------------------------------------------
TARGET		:=	homebrew_launcher
BUILD		:=	build
BUILD_DBG	:=	$(TARGET)_dbg
SOURCES		:=	src \
				src/dynamic_libs \
				src/fs \
				src/game \
				src/gui \
				src/kernel \
				src/loader \
				src/menu \
				src/network \
				src/patcher \
				src/resources \
				src/settings \
				src/sounds \
				src/system \
				src/utils \
				src/video \
				src/video/shaders

INCLUDES	:=  src

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
CFLAGS	:=  -std=gnu11 -mrvl -mcpu=750 -meabi -mhard-float -ffast-math \
		    -O3 -Wall -Wextra -Wno-unused-parameter -Wno-strict-aliasing $(INCLUDE)
CXXFLAGS := -std=gnu++11 -mrvl -mcpu=750 -meabi -mhard-float -ffast-math \
		    -O3 -Wall -Wextra -Wno-unused-parameter -Wno-strict-aliasing $(INCLUDE)
ASFLAGS	:= -mregnames
LDFLAGS	:= -nostartfiles -T $(WUT_ROOT)/rules/rpl.ld -pie -fPIE -z common-page-size=64 -z max-page-size=64 -lcrt \
			-Wl,-wrap,malloc,-wrap,free,-wrap,memalign,-wrap,calloc,-wrap,realloc,-wrap,malloc_usable_size \
			-Wl,-wrap,_malloc_r,-wrap,_free_r,-wrap,_realloc_r,-wrap,_calloc_r,-wrap,_memalign_r,-wrap,_malloc_usable_size_r \
			-Wl,-wrap,valloc,-wrap,_valloc_r,-wrap,_pvalloc_r,-wrap,__eabi -Wl,--gc-sections

#---------------------------------------------------------------------------------
Q := @
MAKEFLAGS += --no-print-directory
#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project
#---------------------------------------------------------------------------------
LIBS	:= -lcrt -lcoreinit -lproc_ui -lnsysnet -lsndcore2 -lvpad -lgx2 -lsysapp -lgd -lpng -lz -lfreetype -lmad -lvorbisidec

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:=	$(CURDIR)	\
			$(DEVKITPPC)/  \
			$(DEVKITPPC)/lib/gcc/powerpc-eabi/4.8.2 \
			$(WUT_ROOT)/lib

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------
export PROJECTDIR := $(CURDIR)
export OUTPUT	:=	$(CURDIR)/$(TARGETDIR)/$(TARGET)
export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
					$(foreach dir,$(DATA),$(CURDIR)/$(dir))
export DEPSDIR	:=	$(CURDIR)/$(BUILD)

#---------------------------------------------------------------------------------
# automatically build a list of object files for our project
#---------------------------------------------------------------------------------
FILELIST	:=	$(shell bash ./filelist.sh)
CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
sFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.S)))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
	export LD	:=	$(CC)
else
	export LD	:=	$(CXX)
endif

export OFILES	:=	$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) \
					$(sFILES:.s=.o) $(SFILES:.S=.o)

#---------------------------------------------------------------------------------
# build a list of include paths
#---------------------------------------------------------------------------------
export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
					-I$(CURDIR)/$(BUILD) -I$(WUT_ROOT)/include \
					-I$(PORTLIBS)/include -I$(PORTLIBS)/include/freetype2

#---------------------------------------------------------------------------------
# build a list of library paths
#---------------------------------------------------------------------------------
export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)) \
					-L$(LIBOGC_LIB) -L$(PORTLIBS)/lib

export OUTPUT	:=	$(CURDIR)/$(TARGET)
.PHONY: $(BUILD) clean install

#---------------------------------------------------------------------------------
$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(Q)$(MAKE) -C sd_loader
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean: clean_channel
	@echo clean ...
	@rm -fr $(BUILD) $(OUTPUT).elf $(OUTPUT).bin $(BUILD_DBG).elf $(OUTPUT).rpx 
	@$(MAKE) -C sd_loader clean

#---------------------------------------------------------------------------------
install_channel: $(BUILD) NUSPacker.jar encryptKeyWith
	@cp $(OUTPUT).rpx channel/code/
	java -jar NUSPacker.jar -in "channel" -out "install_channel"

NUSPacker.jar:
	wget https://bitbucket.org/timogus/nuspacker/downloads/NUSPacker.jar

encryptKeyWith:
	@echo "Missing common key file \"encryptKeyWith\"! Insert the common key as string into \"encryptKeyWith\" file in the HBL Makefile path!"
	@exit 1
	
clean_channel:
	@rm -fr install_channel NUSPacker.jar fst.bin output tmp

#---------------------------------------------------------------------------------
else

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
$(OUTPUT).rpx:	$(OUTPUT).elf
$(OUTPUT).elf:  $(OFILES)

#---------------------------------------------------------------------------------
# This rule links in binary data with the .jpg extension
#---------------------------------------------------------------------------------
%.elf: $(OFILES)
	@echo "linking ... $(TARGET).elf"
	$(Q)$(LD) $^ $(LDFLAGS) -o $@ $(LIBPATHS) $(LIBS)
#	$(Q)$(OBJCOPY) -S -R .comment -R .gnu.attributes ../$(BUILD_DBG).elf $@

#---------------------------------------------------------------------------------
%.rpx: %.elf
#---------------------------------------------------------------------------------
	@echo "[RPX] $(notdir $@)"
	@$(ELF2RPL) $^ $@

#---------------------------------------------------------------------------------
%.a:
#---------------------------------------------------------------------------------
	@echo $(notdir $@)
	@rm -f $@
	@$(AR) -rc $@ $^

#---------------------------------------------------------------------------------
%.o: %.cpp
	@echo $(notdir $<)
	@$(CXX) -MMD -MP -MF $(DEPSDIR)/$*.d $(CXXFLAGS) -c $< -o $@ $(ERROR_FILTER)

#---------------------------------------------------------------------------------
%.o: %.c
	@echo $(notdir $<)
	@$(CC) -MMD -MP -MF $(DEPSDIR)/$*.d $(CFLAGS) -c $< -o $@ $(ERROR_FILTER)

#---------------------------------------------------------------------------------
%.o: %.S
	@echo $(notdir $<)
	@$(CC) -MMD -MP -MF $(DEPSDIR)/$*.d -x assembler-with-cpp $(ASFLAGS) -c $< -o $@ $(ERROR_FILTER)

-include $(DEPENDS)

#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------
