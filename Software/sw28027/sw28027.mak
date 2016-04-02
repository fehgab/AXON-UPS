# Generated by Code Composer Studio

# Makefile for q:\sw28027\sw28027.pjt
# This makefile is intended to be compatible with any version of make.
#
# USAGE
# Step 1: Set up proper environment variables, by running DosRun.bat
#         (Windows platform) or set them up by yourself.
# Step 2: Change directory to the directory of this makefile, which is stored
#         in the macro MAKEFILE_DIR
# Step 3: In the makefile directory, you can perform one of these operations:
#         To build the configuration <config>, type make <config>
#         To clean the configuration <config>, type make <config>clean
#         To rebuild the configuration <config>, type make <config>rebuild
#         If <config> is not specified, the operations apply to the active
#         configuration, which is stored in the macro ACTIVE_CONFIG.
# For CCS gmake users: In the makefile directory, you can perform one of these operations:
#         To build the configuration <config>, type gmake projectName.mak <config>
#         To clean the configuration <config>, type gmake <config>clean
#         To rebuild the configuration <config>, type gmake <config>rebuild
#         If <config> is not specified, the operations apply to the active
#         configuration, which is stored in the macro ACTIVE_CONFIG.
#
# DETAILS
# Step 1:
#   In order for this makefile to work, you must have the environment variables
# set properly.  Specifically, the paths to build programs and the system-wide
# header files and library files must be set.
#   In Windows, a batch file "DosRun.bat", located at the root directory of
# the Code Composer Studio installation, is provided to set up these variables.
#   In UNIX, you can set up these environment variables yourself.  The "PATH"
# variable must include the paths to the build programs, and the "C_DIR" and
# "A_DIR" variables must include the paths to the system-wide header files and
# library files.
#
# Step 2:
#   System files referenced by this project are specified in full path names,
# while other files in this project are specified in path names relative to the
# directory of this makefile.  These directory names are stored in DIR_x macros.
# You can modify them to reflect the locations of the files on this system.
#   It is important that the command to make this makefile is issued from the
# directory of this makefile, which is stored in the macro MAKEFILE_DIR.
#
# Step 3:
#   There are three operations that can be performed on a build configuration:
# make, clean, and rebuild. A rebuild forces all files in the configuration to
# be rebuilt.
# To build the configuration <config>, type make <config>
# To clean the configuration <config>, type make <config>clean
# To rebuild the configuration <config>, type make <config>rebuild
#   If <config> is not specified, the operations apply to the active
# configuration. The active configuration can be any valid build configuration
# (including "all") and is specified in the macro ACTIVE_CONFIG.
# To build the active configuration, type make
# To clean the active configuration, type make clean
# To rebuild the active configuration, type make rebuild
#
# For CCS gmake users:
#   There are three operations that can be performed on a build configuration:
# make, clean, and rebuild. A rebuild forces all files in the configuration to
# be rebuilt.
# To build the configuration <config>, type gmake projectName.mak <config>
# To clean the configuration <config>, type gmake <config>clean
# To rebuild the configuration <config>, type gmake <config>rebuild
#   If <config> is not specified, the operations apply to the active
# configuration. The active configuration can be any valid build configuration
# (including "all") and is specified in the macro ACTIVE_CONFIG.
# To build the active configuration, type gmake
# To clean the active configuration, type gmake clean
# To rebuild the active configuration, type gmake rebuild

# The MKFLAGS macro takes the place of the MFLAGS or MAKEFLAGS macro. If it
# gives you any trouble, just delete the macro. It is used when recursively
# calling make (i.e. when rebuilding)
MKFLAGS= $(MFLAGS)$(MAKEFLAGS)

MAKEFILE_DIR= q:\sw28027

# The active configuration can be one of these valid build configurations:
# all, Debug
ACTIVE_CONFIG= Debug

# These DIR_x macros store the directories of the files used in this project.
# There must be no trailing spaces after these macros.

DIR_1= debug

DIR_2= source

DIR_3= include

DIR_4= C:\CCStudio_v3.3\C2000\cgtools\bin

DIR_5= cmd

SOURCE=$(DIR_2)\DSP2802x_CodeStartBranch.asm
SOURCE=$(DIR_2)\DSP2802x_CpuTimers.c
SOURCE=$(DIR_2)\DSP2802x_usDelay.asm
SOURCE=$(DIR_2)\DSP2802x_GlobalVariableDefs.c
SOURCE=$(DIR_2)\DSP2802x_DefaultIsr.c
SOURCE=$(DIR_2)\DSP2802x_PieCtrl.c
SOURCE=$(DIR_2)\DSP2802x_PieVect.c
SOURCE=$(DIR_2)\bbx.c
SOURCE=$(DIR_2)\err_hyst.c
SOURCE=$(DIR_2)\HiMon.c
SOURCE=$(DIR_2)\inic.c
SOURCE=$(DIR_2)\i2c.c
SOURCE=$(DIR_2)\main.c
SOURCE=$(DIR_2)\Set_save.c


active_config: $(ACTIVE_CONFIG)

norecurse: $(ACTIVE_CONFIG)_norecurse

clean: $(ACTIVE_CONFIG)clean

clean_norecurse: $(ACTIVE_CONFIG)clean_norecurse

rebuild: $(ACTIVE_CONFIG)rebuild

force_rebuild: 

all: Debug 

allclean: Debugclean 

allrebuild: Debugrebuild 


Debugclean: Debugclean_norecurse

Debugclean_norecurse: 
	-@del /F $(DIR_1)\DSP2802x_CodeStartBranch.obj
	-@del /F $(DIR_1)\DSP2802x_CpuTimers.obj
	-@del /F $(DIR_1)\DSP2802x_usDelay.obj	
	-@del /F $(DIR_1)\DSP2802x_GlobalVariableDefs.obj
	-@del /F $(DIR_1)\DSP2802x_DefaultIsr.obj
	-@del /F $(DIR_1)\DSP2802x_PieCtrl.obj
	-@del /F $(DIR_1)\DSP2802x_PieVect.obj
	-@del /F $(DIR_1)\bbx.obj
	-@del /F $(DIR_1)\err_hyst.obj
	-@del /F $(DIR_1)\HiMon.obj
	-@del /F $(DIR_1)\inic.obj
	-@del /F $(DIR_1)\i2c.obj	
	-@del /F $(DIR_1)\main.obj
	-@del /F $(DIR_1)\Set_save.obj

Debugrebuild: 
	$(MAKE) $(MFLAGS) -f sw28027.mak Debug FRC=force_rebuild

Debug: Debug_norecurse

Debug_norecurse: $(DIR_1)\sw28027.out

$(DIR_1)\DSP2802x_CodeStartBranch.obj: $(FRC) $(DIR_2)\DSP2802x_CodeStartBranch.asm 
	cd $(DIR_2) & \
	"$(DIR_4)\cl2000" -gt -q -al -oi5000 -ol1 -o2 -fr"q:/sw28027/Debug" -i"q:/sw28027/Include" -d"_DEBUG" -d"LARGE_MODEL" -ml -v28 "DSP2802x_CodeStartBranch.asm" 

$(DIR_1)\DSP2802x_CpuTimers.obj: $(FRC) $(DIR_2)\DSP2802x_CpuTimers.c
	cd $(DIR_2) & \
	"$(DIR_4)\cl2000" -gt -q -al -oi5000 -ol1 -o2 -fr"q:/sw28027/Debug" -i"q:/sw28027/Include" -d"_DEBUG" -d"LARGE_MODEL" -ml -v28 "DSP2802x_CpuTimers.c" 

$(DIR_1)\DSP2802x_usDelay.obj: $(FRC) $(DIR_2)\DSP2802x_usDelay.asm 
	cd $(DIR_2) & \
	"$(DIR_4)\cl2000" -gt -q -al -oi5000 -ol1 -o2 -fr"q:/sw28027/Debug" -i"q:/sw28027/Include" -d"_DEBUG" -d"LARGE_MODEL" -ml -v28 "DSP2802x_usDelay.asm" 

$(DIR_1)\DSP2802x_GlobalVariableDefs.obj: $(FRC) $(DIR_2)\DSP2802x_GlobalVariableDefs.c
	cd $(DIR_2) & \
	"$(DIR_4)\cl2000" -gt -q -al -oi5000 -ol1 -o2 -fr"q:/sw28027/Debug" -i"q:/sw28027/Include" -d"_DEBUG" -d"LARGE_MODEL" -ml -v28 "DSP2802x_GlobalVariableDefs.c" 

$(DIR_1)\DSP2802x_DefaultIsr.obj: $(FRC) $(DIR_2)\DSP2802x_DefaultIsr.c 
	cd $(DIR_2) & \
	"$(DIR_4)\cl2000" -gt -q -al -oi5000 -ol1 -o2 -fr"q:/sw28027/Debug" -i"q:/sw28027/Include" -d"_DEBUG" -d"LARGE_MODEL" -ml -v28 "DSP2802x_DefaultIsr.c" 

$(DIR_1)\DSP2802x_PieCtrl.obj: $(FRC) $(DIR_2)\DSP2802x_PieCtrl.c 
	cd $(DIR_2) & \
	"$(DIR_4)\cl2000" -gt -q -al -oi5000 -ol1 -o2 -fr"q:/sw28027/Debug" -i"q:/sw28027/Include" -d"_DEBUG" -d"LARGE_MODEL" -ml -v28 "DSP2802x_PieCtrl.c" 

$(DIR_1)\DSP2802x_PieVect.obj: $(FRC) $(DIR_2)\DSP2802x_PieVect.c 
	cd $(DIR_2) & \
	"$(DIR_4)\cl2000" -gt -q -al -oi5000 -ol1 -o2 -fr"q:/sw28027/Debug" -i"q:/sw28027/Include" -d"_DEBUG" -d"LARGE_MODEL" -ml -v28 "DSP2802x_PieVect.c" 

$(DIR_1)\bbx.obj: $(FRC) $(DIR_2)\bbx.c
	cd $(DIR_2) & \
	"$(DIR_4)\cl2000" -gt -q -al -oi5000 -ol1 -o2 -fr"q:/sw28027/Debug" -i"q:/sw28027/Include" -d"_DEBUG" -d"LARGE_MODEL" -ml -v28 "bbx.c" 

$(DIR_1)\err_hyst.obj: $(FRC) $(DIR_2)\err_hyst.c
	cd $(DIR_2) & \
	"$(DIR_4)\cl2000" -gt -q -al -oi5000 -ol1 -o2 -fr"q:/sw28027/Debug" -i"q:/sw28027/Include" -d"_DEBUG" -d"LARGE_MODEL" -ml -v28 "err_hyst.c" 

$(DIR_1)\HiMon.obj: $(FRC) $(DIR_2)\HiMon.c 
	cd $(DIR_2) & \
	"$(DIR_4)\cl2000" -gt -q -al -oi5000 -ol1 -o2 -fr"q:/sw28027/Debug" -i"q:/sw28027/Include" -d"_DEBUG" -d"LARGE_MODEL" -ml -v28 "HiMon.c" 

$(DIR_1)\inic.obj: $(FRC) $(DIR_2)\inic.c 
	cd $(DIR_2) & \
	"$(DIR_4)\cl2000" -gt -q -al -oi5000 -ol1 -o2 -fr"q:/sw28027/Debug" -i"q:/sw28027/Include" -d"_DEBUG" -d"LARGE_MODEL" -ml -v28 "inic.c" 

$(DIR_1)\i2c.obj: $(FRC) $(DIR_2)\i2c.c 
	cd $(DIR_2) & \
	"$(DIR_4)\cl2000" -gt -q -al -oi5000 -ol1 -o2 -fr"q:/sw28027/Debug" -i"q:/sw28027/Include" -d"_DEBUG" -d"LARGE_MODEL" -ml -v28 "i2c.c" 

$(DIR_1)\main.obj: $(FRC) $(DIR_2)\main.c 
	cd $(DIR_2) & \
	"$(DIR_4)\cl2000" -gt -q -al -oi5000 -ol1 -o2 -fr"q:/sw28027/Debug" -i"q:/sw28027/Include" -d"_DEBUG" -d"LARGE_MODEL" -ml -v28 "main.c" 

$(DIR_1)\Set_save.obj: $(FRC) $(DIR_2)\Set_save.c 
	cd $(DIR_2) & \
	"$(DIR_4)\cl2000" -gt -q -al -oi5000 -ol1 -o2 -fr"q:/sw28027/Debug" -i"q:/sw28027/Include" -d"_DEBUG" -d"LARGE_MODEL" -ml -v28 "Set_save.c" 

$(DIR_1)\sw28027.out: $(FRC) $(DIR_5)\sw28027link.cmd $(DIR_1)\DSP2802x_CodeStartBranch.obj $(DIR_1)\DSP2802x_CpuTimers.obj $(DIR_1)\DSP2802x_usDelay.obj $(DIR_1)\DSP2802x_GlobalVariableDefs.obj $(DIR_1)\DSP2802x_DefaultIsr.obj $(DIR_1)\DSP2802x_PieCtrl.obj $(DIR_1)\DSP2802x_PieVect.obj $(DIR_1)\bbx.obj $(DIR_1)\err_hyst.obj $(DIR_1)\HiMon.obj $(DIR_1)\inic.obj $(DIR_1)\i2c.obj $(DIR_1)\main.obj $(DIR_1)\Set_save.obj
	-@echo -z -c -m"./Debug/sw28027.map" -o"./Debug/sw28027.out" -w -x> sw28027.Debug.lkf
	-@echo "$(DIR_5)\sw28027link.cmd">> sw28027.Debug.lkf
	-@echo "$(DIR_1)\DSP2802x_CodeStartBranch.obj">> sw28027.Debug.lkf
	-@echo "$(DIR_1)\DSP2802x_CpuTimers.obj">> sw28027.Debug.lkf
	-@echo "$(DIR_1)\DSP2802x_usDelay.obj">> sw28027.Debug.lkf
	-@echo "$(DIR_1)\DSP2802x_GlobalVariableDefs.obj">> sw28027.Debug.lkf
	-@echo "$(DIR_1)\DSP2802x_DefaultIsr.obj">> sw28027.Debug.lkf
	-@echo "$(DIR_1)\DSP2802x_PieCtrl.obj">> sw28027.Debug.lkf
	-@echo "$(DIR_1)\DSP2802x_PieVect.obj">> sw28027.Debug.lkf
	-@echo "$(DIR_1)\bbx.obj">> sw28027.Debug.lkf	
	-@echo "$(DIR_1)\err_hyst.obj">> sw28027.Debug.lkf
	-@echo "$(DIR_1)\HiMon.obj">> sw28027.Debug.lkf	
	-@echo "$(DIR_1)\inic.obj">> sw28027.Debug.lkf	
	-@echo "$(DIR_1)\i2c.obj">> sw28027.Debug.lkf		
	-@echo "$(DIR_1)\main.obj">> sw28027.Debug.lkf	
	-@echo "$(DIR_1)\Set_save.obj">> sw28027.Debug.lkf	
	"$(DIR_4)\cl2000" -@"sw28027.Debug.lkf"
	-@del /F sw28027.Debug.lkf
