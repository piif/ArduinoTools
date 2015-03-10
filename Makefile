# it's just a library
TODO := lib

include ${ARDDUDE_DIR}/etc/target.mk

$(info TARGET_PLATFORM = ${TARGET_PLATFORM})

INCLUDE_FLAGS_EXTRA += $(addprefix -I,${LIBRARIES_DIRS} .)

SOURCE_EXCLUDE_PATTERNS := /examples/ /tests/

ifeq (${TARGET_PLATFORM},sam)
  SOURCE_EXCLUDES := events/ pwm/
  SOURCE_EXCLUDE_PATTERNS += ArduinoTools.cpp
endif


include ${ARDDUDE_DIR}/etc/main.mk
