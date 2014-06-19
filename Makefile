# the directory this project is in
# must be defined for generic makefile to work
export PROJECT_DIR := $(dir $(realpath ${MAKEFILE_LIST}))

# to define to ArduinoCore root directory 
CORE_DIR := ${PROJECT_DIR}../ArduinoCore/

export EXCLUDE_DIRS := makePolice

# other arduino librairies project pathes this project depends on
export DEPENDENCIES := ${CORE_DIR}../ArduinoLibs/

# generate assembler source code also
export WITH_ASSEMBLY := yes

# generate eeprom image
export WITH_EEPROM := no

# print size of geretated segments 
export WITH_PRINT_SIZE := no

# only for programs : launch upload
export WITH_UPLOAD := no
# where to upload
# TODO : try to auto detect with lsusb + /proc exploration
#export UPLOAD_DEVICE := /dev/ttyACM0

# if different of containing dir
#export LIB_NAME := 
# TODO : add a "if" on TARGET to launch on this target, or for each known one

# call lib.makefile for a utilities library or bin.makefile for a program
all upload console:
	${MAKE} -f ${CORE_DIR}etc/lib.makefile $@

clean:
ifeq (${TARGET},)
	rm -rf ${PROJECT_DIR}target/*
endif
ifneq (${TARGET},)
	rm -rf ${PROJECT_DIR}target/${TARGET}
endif
